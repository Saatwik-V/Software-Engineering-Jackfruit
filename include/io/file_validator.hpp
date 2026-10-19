/**
 * @file file_validator.hpp
 * @brief Input and output file path validation routines (JACK-F-001).
 *
 * Provides static validation methods to verify file existence, file type,
 * read permissions, and output destination viability without throwing exceptions.
 */

#pragma once

#include <string>
#include <cstdint>

namespace jackfruit::io {

/**
 * @brief Status enumeration detailing the outcome of file validation checks.
 */
enum class FileStatus {
    OK = 0,               ///< File exists, is regular, and is readable.
    NOT_FOUND,            ///< File or target directory does not exist.
    NOT_A_REGULAR_FILE,   ///< Path exists but is a directory, FIFO, socket, or device file.
    PERMISSION_DENIED,    ///< File cannot be opened due to insufficient operating system permissions.
    EMPTY_FILE,           ///< File exists and is readable, but has a size of 0 bytes.
    IO_ERROR              ///< Operating system I/O or filesystem query failure.
};

/**
 * @brief Detailed validation result structure returned by FileValidator.
 */
struct FileValidationResult {
    bool isValid{false};                  ///< True if file is acceptable for processing.
    FileStatus status{FileStatus::NOT_FOUND}; ///< Specific validation status code.
    std::string errorMessage;             ///< Human-readable error message for CLI/UI display.
    uint64_t fileSize{0};                 ///< Size of the file in bytes (if accessible).
};

/**
 * @brief Utility class providing static methods for filesystem path validation.
 */
class FileValidator {
public:
    /**
     * @brief Validates an input file path for existence, regular file status, and readability.
     *
     * Performs sequential filesystem checks:
     * 1. Rejects empty path strings.
     * 2. Checks existence via std::filesystem::exists.
     * 3. Verifies that the path points to a regular file (not directory or symlink loop).
     * 4. Attempts binary read open to verify file permissions.
     * 5. Queries file size, marking 0-byte files as EMPTY_FILE while remaining valid.
     *
     * @param filePath Path to the input file.
     * @return FileValidationResult Struct containing status, error message, and size.
     */
    static FileValidationResult validateInputFile(const std::string& filePath);

    /**
     * @brief Validates an output file path for writable directory location.
     *
     * Checks that the parent directory exists, path is not an existing directory,
     * and destination file can be opened for writing.
     *
     * @param filePath Path where output will be written.
     * @param overwriteAllowed If false, returns an error if target file already exists.
     * @return FileValidationResult Struct containing status and any error message.
     */
    static FileValidationResult validateOutputFile(const std::string& filePath, bool overwriteAllowed = true);
};

} // namespace jackfruit::io
