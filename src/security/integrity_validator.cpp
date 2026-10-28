/**
 * @file integrity_validator.cpp
 * @brief Implementation of archive integrity validation and CRC-32 verification (UC-04).
 */

#include "security/integrity_validator.hpp"

#include <array>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

#include "archive/archive.hpp"
#include "archive/archive_validation.hpp"
#include "compression/huffman_tree.hpp"
#include "io/bit_reader.hpp"
#include "security/crc32.hpp"

namespace jackfruit::security {

ValidationResult IntegrityValidator::validateArchive(const std::string& archivePath) {
    namespace fs = std::filesystem;
    std::error_code ec;

    // Check 1: Target file existence
    if (!fs::exists(archivePath, ec) || fs::is_directory(archivePath, ec)) {
        return {
            ValidationStatus::ERROR_FILE_NOT_FOUND,
            1,
            0,
            0,
            0,
            "Archive file does not exist or cannot be accessed: " + archivePath
        };
    }

    // Check 2: File size sanity check (must be at least 18 bytes for fixed header)
    const uint64_t fileSize = fs::file_size(archivePath, ec);
    if (ec || fileSize < ARCHIVE_FIXED_HEADER_SIZE) {
        return {
            ValidationStatus::ERROR_INVALID_HEADER,
            2,
            0,
            0,
            0,
            "Archive is too small to contain a valid Jackfruit header (minimum 18 bytes required)."
        };
    }

    // Stage 1: Structural Header and Section 6 Bounds Validation
    ArchiveHeader header{};
    std::vector<std::pair<uint8_t, uint64_t>> frequencies;
    try {
        ArchiveReader::readValidatedHeader(archivePath, header, frequencies);
    } catch (int) {
        return {
            ValidationStatus::ERROR_INVALID_HEADER,
            2,
            header.checksumCRC32,
            0,
            0,
            "Structural validation failed: archive header or metadata is corrupt."
        };
    } catch (const std::exception& e) {
        return {
            ValidationStatus::ERROR_INVALID_HEADER,
            2,
            header.checksumCRC32,
            0,
            0,
            std::string("Structural validation error: ") + e.what()
        };
    } catch (...) {
        return {
            ValidationStatus::ERROR_INVALID_HEADER,
            2,
            header.checksumCRC32,
            0,
            0,
            "Unknown structural validation exception encountered."
        };
    }

    // Stage 2: Payload Cryptographic Integrity (Recompute CRC-32 in-memory)
    // Case A: Empty Archive (original size 0)
    if (header.originalSize == 0 || (header.flags & ARCHIVE_FLAG_IS_EMPTY)) {
        constexpr uint32_t expectedEmptyCRC = 0U;
        if (header.checksumCRC32 != expectedEmptyCRC) {
            return {
                ValidationStatus::ERROR_CHECKSUM_MISMATCH,
                3,
                header.checksumCRC32,
                expectedEmptyCRC,
                0,
                header.symbolCount,
                header.version,
                "Checksum mismatch: empty archive header contains non-zero CRC-32."
            };
        }
        return {
            ValidationStatus::VALID,
            0,
            header.checksumCRC32,
            expectedEmptyCRC,
            0,
            header.symbolCount,
            header.version,
            "Archive integrity verified successfully (PASS)."
        };
    }

    // Case B: Uncompressed Archive Fallback
    if (header.flags & ARCHIVE_FLAG_IS_UNCOMPRESSED) {
        std::ifstream inFile(archivePath, std::ios::binary);
        if (!inFile) {
            return {
                ValidationStatus::ERROR_FILE_NOT_FOUND,
                1,
                header.checksumCRC32,
                0,
                0,
                "Unable to open archive file for payload verification."
            };
        }

        // Uncompressed payload starts at offset 28 (18 fixed + 2 symbol count + 8 total bits)
        constexpr uint64_t uncompressedPayloadOffset = 28;
        inFile.seekg(uncompressedPayloadOffset, std::ios::beg);

        std::array<uint8_t, 8192> chunk{};
        uint32_t runningCRC = 0U;
        uint64_t bytesRead = 0;

        while (bytesRead < header.originalSize && inFile) {
            const size_t toRead = static_cast<size_t>(
                std::min<uint64_t>(chunk.size(), header.originalSize - bytesRead));
            inFile.read(reinterpret_cast<char*>(chunk.data()), static_cast<std::streamsize>(toRead));
            const std::streamsize g = inFile.gcount();
            if (g <= 0) break;

            runningCRC = CRC32::update(runningCRC, chunk.data(), static_cast<size_t>(g));
            bytesRead += static_cast<uint64_t>(g);
        }

        if (bytesRead != header.originalSize) {
            return {
                ValidationStatus::ERROR_TRUNCATED_PAYLOAD,
                4,
                header.checksumCRC32,
                runningCRC,
                bytesRead,
                header.symbolCount,
                header.version,
                "Uncompressed payload is truncated before reaching declared original size."
            };
        }

        if (runningCRC != header.checksumCRC32) {
            return {
                ValidationStatus::ERROR_CHECKSUM_MISMATCH,
                3,
                header.checksumCRC32,
                runningCRC,
                bytesRead,
                header.symbolCount,
                header.version,
                "CRC-32 checksum mismatch: uncompressed payload has been modified or corrupted."
            };
        }

        return {
            ValidationStatus::VALID,
            0,
            header.checksumCRC32,
            runningCRC,
            bytesRead,
            header.symbolCount,
            header.version,
            "Archive integrity verified successfully (PASS)."
        };
    }

    // Case C: Standard Huffman Compressed Payload
    std::array<uint64_t, 256> freqArray{};
    for (const auto& pair : frequencies) {
        freqArray[pair.first] = pair.second;
    }

    jackfruit::compression::HuffmanTree tree;
    tree.buildFromFrequencies(freqArray);
    auto root = tree.getRoot();
    if (!root) {
        return {
            ValidationStatus::ERROR_INVALID_HEADER,
            2,
            header.checksumCRC32,
            0,
            0,
            header.symbolCount,
            header.version,
            "Failed to reconstruct Huffman decoding tree from symbol table."
        };
    }

    std::ifstream inFile(archivePath, std::ios::binary);
    if (!inFile) {
        return {
            ValidationStatus::ERROR_FILE_NOT_FOUND,
            1,
            header.checksumCRC32,
            0,
            0,
            header.symbolCount,
            header.version,
            "Unable to open archive stream for bitstream decoding."
        };
    }

    // Offset of compressed bitstream: 18 (fixed) + 2 (symbol count) + (N * 9) + 8 (total bits)
    const uint64_t headerOffset = 28 + (static_cast<uint64_t>(header.symbolCount) * 9);
    inFile.seekg(headerOffset, std::ios::beg);

    jackfruit::io::BitReader bitReader(inFile, header.totalBits);

    std::array<uint8_t, 8192> decodeBuffer{};
    size_t bufferIndex = 0;
    uint32_t runningCRC = 0U;
    uint64_t bytesDecoded = 0;

    auto currentNode = root;
    uint8_t bit = 0;

    while (bytesDecoded < header.originalSize && bitReader.readBit(bit)) {
        currentNode = (bit == 0) ? currentNode->left : currentNode->right;
        if (!currentNode) {
            // Invalid tree traversal branch indicates corrupted bitstream
            return {
                ValidationStatus::ERROR_CHECKSUM_MISMATCH,
                3,
                header.checksumCRC32,
                runningCRC,
                bytesDecoded,
                header.symbolCount,
                header.version,
                "Bitstream corruption: invalid Huffman code encountered during payload decoding."
            };
        }

        if (currentNode->isLeaf()) {
            decodeBuffer[bufferIndex++] = currentNode->symbol;
            bytesDecoded++;

            if (bufferIndex == decodeBuffer.size()) {
                runningCRC = CRC32::update(runningCRC, decodeBuffer.data(), bufferIndex);
                bufferIndex = 0;
            }

            currentNode = root;
        }
    }

    // Flush any remaining buffered bytes into the CRC-32 calculation
    if (bufferIndex > 0) {
        runningCRC = CRC32::update(runningCRC, decodeBuffer.data(), bufferIndex);
    }

    // Verify whether the declared number of bytes was fully reconstructed
    if (bytesDecoded != header.originalSize) {
        return {
            ValidationStatus::ERROR_TRUNCATED_PAYLOAD,
            4,
            header.checksumCRC32,
            runningCRC,
            bytesDecoded,
            header.symbolCount,
            header.version,
            "Premature end of bitstream: decoded payload fewer bytes than declared original size."
        };
    }

    // Final Cryptographic Verification against Stored IEEE 802.3 Checksum
    if (runningCRC != header.checksumCRC32) {
        return {
            ValidationStatus::ERROR_CHECKSUM_MISMATCH,
            3,
            header.checksumCRC32,
            runningCRC,
            bytesDecoded,
            header.symbolCount,
            header.version,
            "Checksum mismatch: computed CRC-32 differs from stored archive header checksum."
        };
    }

    return {
        ValidationStatus::VALID,
        0,
        header.checksumCRC32,
        runningCRC,
        bytesDecoded,
        header.symbolCount,
        header.version,
        "Archive integrity verified successfully (PASS)."
    };
}

std::string IntegrityValidator::formatValidationReport(const ValidationResult& result, bool useColor) {
    const std::string colorGreen = useColor ? "\033[32m\033[1m" : "";
    const std::string colorRed   = useColor ? "\033[31m\033[1m" : "";
    const std::string colorCyan  = useColor ? "\033[36m\033[1m" : "";
    const std::string colorReset = useColor ? "\033[0m" : "";

    std::ostringstream oss;
    oss << "\n" << colorCyan << "============================================================" << colorReset << "\n"
        << colorCyan << "            Archive Integrity Verification Report           " << colorReset << "\n"
        << colorCyan << "============================================================" << colorReset << "\n";

    if (result.status == ValidationStatus::VALID) {
        oss << colorGreen << "[+] Integrity Status:   PASS (VALID)" << colorReset << "\n"
            << "    Exit Code:          0\n"
            << "    Payload Verified:   " << result.payloadBytes << " bytes\n"
            << "    Stored CRC-32:      0x" << std::hex << std::uppercase << std::setfill('0')
            << std::setw(8) << result.expectedChecksum << std::dec << "\n"
            << "    Computed CRC-32:    0x" << std::hex << std::uppercase << std::setfill('0')
            << std::setw(8) << result.computedChecksum << std::dec << "\n"
            << "    Checksum Match:     YES (Identical)\n"
            << "------------------------------------------------------------\n"
            << colorGreen << "[+] " << result.message << colorReset << "\n";
    } else {
        oss << colorRed << "[-] Integrity Status:   FAIL ("
            << (result.status == ValidationStatus::ERROR_CHECKSUM_MISMATCH ? "CHECKSUM_MISMATCH"
                : result.status == ValidationStatus::ERROR_INVALID_HEADER ? "INVALID_HEADER"
                : result.status == ValidationStatus::ERROR_FILE_NOT_FOUND ? "FILE_NOT_FOUND"
                : "TRUNCATED_PAYLOAD")
            << ")" << colorReset << "\n"
            << "    Exit Code:          " << result.exitCode << "\n"
            << "    Payload Decoded:    " << result.payloadBytes << " bytes\n"
            << "    Stored CRC-32:      0x" << std::hex << std::uppercase << std::setfill('0')
            << std::setw(8) << result.expectedChecksum << std::dec << "\n"
            << "    Computed CRC-32:    0x" << std::hex << std::uppercase << std::setfill('0')
            << std::setw(8) << result.computedChecksum << std::dec << "\n"
            << "    Checksum Match:     NO (Mismatch)\n"
            << "------------------------------------------------------------\n"
            << colorRed << "[-] " << result.message << colorReset << "\n";
    }
    oss << colorCyan << "============================================================" << colorReset << "\n";

    return oss.str();
}

} // namespace jackfruit::security
