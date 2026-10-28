/**
 * @file integrity_validator.hpp
 * @brief Archive integrity validation and CRC-32 verification engine (UC-04, JACK-F-013..016, JACK-SR-001..005).
 *
 * Implements two-stage structural and cryptographic validation for .jack binary archives:
 * Stage 1: Structural header inspection (magic bytes, version, bounds, flags, symbol count, exact size).
 * Stage 2: In-memory payload decoding and IEEE 802.3 CRC-32 verification against the stored checksum.
 */

#pragma once

#include <cstdint>
#include <string>

namespace jackfruit::security {

/**
 * @brief Status enumeration for archive integrity validation.
 */
enum class ValidationStatus {
    VALID = 0,                  ///< Archive is structurally intact and CRC-32 matches.
    ERROR_FILE_NOT_FOUND = 1,   ///< Target archive does not exist or cannot be opened.
    ERROR_INVALID_HEADER = 2,   ///< Magic, version, flags, or symbol metadata is malformed/corrupted.
    ERROR_CHECKSUM_MISMATCH = 3,///< Decompressed payload CRC-32 does not match stored checksum.
    ERROR_TRUNCATED_PAYLOAD = 4 ///< Payload bitstream terminates unexpectedly or corrupt tree traversal.
};

/**
 * @brief Detailed outcome of an archive validation operation.
 */
struct ValidationResult {
    ValidationStatus status{ValidationStatus::VALID};
    int exitCode{0};                ///< Standard process exit code (0 = valid, 1 = not found, 2 = bad header, 3 = CRC mismatch, 4 = truncated).
    uint32_t expectedChecksum{0};   ///< Checksum stored in the fixed archive header.
    uint32_t computedChecksum{0};   ///< Recomputed CRC-32 from the decoded uncompressed payload.
    uint64_t payloadBytes{0};       ///< Restored payload size in bytes.
    uint16_t symbolCount{0};        ///< Unique symbol count.
    uint8_t version{1};             ///< Format specification version.
    std::string message;            ///< Human-readable diagnostic explanation.
};

/**
 * @brief Two-stage structural and cryptographic archive integrity validator.
 */
class IntegrityValidator {
public:
    IntegrityValidator() = default;
    ~IntegrityValidator() = default;

    /**
     * @brief Performs comprehensive two-stage validation on a .jack archive.
     *
     * Stage 1 verifies header magic ("JACK"), version, flags, symbol table consistency,
     * and exact file size bounds before decoding.
     * Stage 2 rebuilds the Huffman tree, decodes the payload in memory without writing to disk,
     * and re-computes the IEEE 802.3 CRC-32 to compare with the stored checksum.
     *
     * @param archivePath Path to the .jack file.
     * @return ValidationResult containing status, exit code, checksums, and message.
     */
    static ValidationResult validateArchive(const std::string& archivePath);

    /**
     * @brief Generates a formatted terminal report summarizing validation results.
     * @param result Result to format.
     * @param useColor Whether to use ANSI terminal colors.
     * @return Formatted multi-line string.
     */
    static std::string formatValidationReport(const ValidationResult& result, bool useColor = true);
};

} // namespace jackfruit::security
