/**
 * @file compressor.cpp
 * @brief Implementation of top-level compression pipeline orchestrator.
 */

#include "compression/compressor.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "archive/archive.hpp"
#include "archive/archive_validation.hpp"
#include "archive/archive_writer.hpp"
#include "compression/encoder.hpp"
#include "compression/frequency_table.hpp"
#include "compression/huffman_tree.hpp"
#include "io/bit_writer.hpp"
#include "io/file_validator.hpp"
#include "security/crc32.hpp"

namespace jackfruit::compression {

CompressionResult Compressor::compressFile(const std::string& inputPath,
                                           const std::string& outputPath) {
    const auto startTime = std::chrono::high_resolution_clock::now();
    CompressionResult result;

    // 1. Validate input file
    const auto inputValidation = io::FileValidator::validateInputFile(inputPath);
    if (!inputValidation.isValid) {
        switch (inputValidation.status) {
            case io::FileStatus::NOT_FOUND:
                result.status = CompressionStatus::ERROR_FILE_NOT_FOUND;
                break;
            case io::FileStatus::PERMISSION_DENIED:
                result.status = CompressionStatus::ERROR_PERMISSION_DENIED;
                break;
            case io::FileStatus::NOT_A_REGULAR_FILE:
                result.status = CompressionStatus::ERROR_NOT_A_REGULAR_FILE;
                break;
            default:
                result.status = CompressionStatus::ERROR_UNKNOWN;
                break;
        }
        result.errorMessage = inputValidation.errorMessage;
        return result;
    }

    // 2. Validate output destination path
    const auto outputValidation = io::FileValidator::validateOutputFile(outputPath, true);
    if (!outputValidation.isValid) {
        result.status = CompressionStatus::ERROR_CANNOT_WRITE_ARCHIVE;
        result.errorMessage = outputValidation.errorMessage;
        return result;
    }

    result.originalBytes = inputValidation.fileSize;

    // 3. Compute IEEE 802.3 CRC-32 checksum of original file (Issue #36)
    uint32_t checksum = 0U;
    if (result.originalBytes > 0) {
        try {
            checksum = CRC32::calculateFile(inputPath);
        } catch (const std::exception& ex) {
            result.status = CompressionStatus::ERROR_UNKNOWN;
            result.errorMessage = std::string("CRC-32 computation failed: ") + ex.what();
            return result;
        }
    }
    result.checksumCRC32 = checksum;

    // 4. Handle 0-byte empty file edge case (TC-COMP-03 / archive-format Section 5.1)
    if (result.originalBytes == 0 || inputValidation.status == io::FileStatus::EMPTY_FILE) {
        ArchiveHeader header{};
        header.flags = ARCHIVE_FLAG_IS_EMPTY;
        header.originalSize = 0;
        header.checksumCRC32 = 0;
        header.symbolCount = 0;
        header.totalBits = 0;

        try {
            ArchiveWriter::write(outputPath, header, {}, {});
        } catch (const std::exception& ex) {
            result.status = CompressionStatus::ERROR_CANNOT_WRITE_ARCHIVE;
            result.errorMessage = std::string("Failed to write empty archive: ") + ex.what();
            return result;
        }

        result.compressedBytes = 28;
        result.compressionRatio = 0.0;
        result.percentageSaved = 0.0;
        result.status = CompressionStatus::SUCCESS;

        const auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        return result;
    }

    // 5. Standard non-empty file compression pipeline
    // 5a. Analyze byte frequency distribution (JACK-F-002)
    FrequencyTable freqTable;
    if (!freqTable.countFrequenciesFromFile(inputPath)) {
        result.status = CompressionStatus::ERROR_UNKNOWN;
        result.errorMessage = "Failed to read input file for frequency analysis: " + inputPath;
        return result;
    }

    // 5b. Construct optimal canonical Huffman prefix tree (JACK-F-002)
    HuffmanTree tree;
    tree.buildFromFrequencyTable(freqTable);

    // 5c. Populate frequency list sorted strictly ascending by byte value
    FrequencyList frequencies;
    frequencies.reserve(freqTable.getUniqueSymbolCount());
    for (int symbol = 0; symbol < 256; ++symbol) {
        const uint64_t count = freqTable.getFrequency(static_cast<uint8_t>(symbol));
        if (count > 0) {
            frequencies.emplace_back(static_cast<uint8_t>(symbol), count);
        }
    }

    // 5d. Encode uncompressed byte stream into packed MSB-first bitstream (JACK-F-004)
    std::ifstream inputStream(inputPath, std::ios::binary);
    if (!inputStream) {
        result.status = CompressionStatus::ERROR_UNKNOWN;
        result.errorMessage = "Failed to open input file for encoding: " + inputPath;
        return result;
    }

    std::stringstream payloadStream(std::ios::in | std::ios::out | std::ios::binary);
    uint64_t totalEncodedBits = 0;
    {
        io::BitWriter bitWriter(payloadStream);
        Encoder encoder(tree);
        const EncoderResult encodeResult = encoder.encode(inputStream, bitWriter);
        if (!encodeResult.success) {
            result.status = CompressionStatus::ERROR_ENCODING_FAILED;
            result.errorMessage = encodeResult.errorMessage;
            return result;
        }
        bitWriter.flush();
        totalEncodedBits = encodeResult.totalBitsEncoded;
    }

    const std::string payloadString = payloadStream.str();
    const std::vector<uint8_t> payload(payloadString.begin(), payloadString.end());

    // 5e. Build header and serialize archive to output path
    ArchiveHeader header{};
    header.flags = 0x00;
    header.originalSize = result.originalBytes;
    header.checksumCRC32 = checksum;
    header.symbolCount = static_cast<uint16_t>(frequencies.size());
    header.totalBits = totalEncodedBits;

    try {
        ArchiveWriter::write(outputPath, header, frequencies, payload);
    } catch (const std::exception& ex) {
        result.status = CompressionStatus::ERROR_CANNOT_WRITE_ARCHIVE;
        result.errorMessage = std::string("Archive serialization failed: ") + ex.what();
        return result;
    }

    // 6. Collect final performance statistics
    std::error_code ec;
    const uint64_t actualCompressedSize = std::filesystem::file_size(outputPath, ec);
    result.compressedBytes = ec ? expectedArchiveSize(header.symbolCount, header.totalBits)
                                : actualCompressedSize;

    if (result.compressedBytes > 0) {
        result.compressionRatio = static_cast<double>(result.originalBytes) /
                                  static_cast<double>(result.compressedBytes);
        result.percentageSaved = (1.0 - (static_cast<double>(result.compressedBytes) /
                                         static_cast<double>(result.originalBytes))) * 100.0;
    }

    const auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    result.status = CompressionStatus::SUCCESS;

    return result;
}

} // namespace jackfruit::compression
