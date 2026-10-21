/**
 * @file compressor.hpp
 * @brief Top-level compression pipeline orchestrator (JACK-F-001, JACK-F-004, JACK-F-015).
 *
 * Coordinates file validation, CRC-32 checksum calculation, byte frequency analysis,
 * canonical Huffman tree construction, MSB-first stream bit packing, and .jack binary
 * archive serialization conforming to archive-format.md specification.
 */

#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace jackfruit::compression {

/**
 * @brief Detailed status code indicating the outcome of a compression operation.
 */
enum class CompressionStatus {
    SUCCESS = 0,
    ERROR_FILE_NOT_FOUND,
    ERROR_PERMISSION_DENIED,
    ERROR_NOT_A_REGULAR_FILE,
    ERROR_CANNOT_WRITE_ARCHIVE,
    ERROR_ENCODING_FAILED,
    ERROR_UNKNOWN
};

/**
 * @brief Performance statistics and metadata produced by the compression pipeline.
 */
struct CompressionResult {
    CompressionStatus status{CompressionStatus::ERROR_UNKNOWN};
    std::string errorMessage;
    uint64_t originalBytes{0};
    uint64_t compressedBytes{0};
    double compressionRatio{0.0};   ///< originalBytes / compressedBytes (e.g. 2.50x)
    double percentageSaved{0.0};    ///< Space reduction percentage (e.g. 60.0%)
    double executionTimeMs{0.0};    ///< Wall-clock duration in milliseconds
    uint32_t checksumCRC32{0};      ///< IEEE 802.3 CRC-32 checksum of original file
};

/**
 * @brief High-level compression engine implementing UC-01 and integrating UC-03 and UC-04.
 */
class Compressor {
public:
    Compressor() = default;
    ~Compressor() = default;

    /**
     * @brief Compresses an input file on disk into a .jack binary archive.
     *
     * Pipeline execution sequence:
     * 1. Validates input path (existence, regular file, readability) via FileValidator.
     * 2. Validates output path permissions and writability via FileValidator.
     * 3. Computes 32-bit uncompressed CRC-32 checksum via CRC32::calculateFile (Issue #36).
     * 4. If empty (0 bytes): creates compliant 28-byte archive with ARCHIVE_FLAG_IS_EMPTY.
     * 5. If non-empty:
     *    a. Counts byte frequencies via FrequencyTable.
     *    b. Builds optimal Huffman prefix tree with deterministic tie-breaking via HuffmanTree.
     *    c. Encodes byte stream into BitWriter via Encoder.
     *    d. Serializes Section A (Header), Section B (Frequencies), and Section C (Payload)
     *       using ArchiveWriter conforming to Section 6 structural specifications.
     * 6. Collects performance benchmarks (ratio, percentage saved, duration).
     *
     * @param inputPath Path to original uncompressed file.
     * @param outputPath Path where .jack archive should be written.
     * @return CompressionResult Detailed outcome and compression metrics.
     */
    CompressionResult compressFile(const std::string& inputPath,
                                   const std::string& outputPath);
};

} // namespace jackfruit::compression
