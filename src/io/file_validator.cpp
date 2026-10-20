#include "io/file_validator.hpp"
#include <filesystem>
#include <fstream>
#include <system_error>

namespace fs = std::filesystem;

namespace jackfruit::io {

FileValidationResult FileValidator::validateInputFile(const std::string& filePath) {
    FileValidationResult result;
    result.isValid = false;

    if (filePath.empty()) {
        result.status = FileStatus::NOT_FOUND;
        result.errorMessage = "File path is empty.";
        return result;
    }

    std::error_code ec;
    fs::path p(filePath);

    // 1. Check existence
    if (!fs::exists(p, ec)) {
        result.status = FileStatus::NOT_FOUND;
        result.errorMessage = "File does not exist: " + filePath;
        return result;
    }

    // 2. Check regular file
    if (!fs::is_regular_file(p, ec)) {
        result.status = FileStatus::NOT_A_REGULAR_FILE;
        result.errorMessage = "Path is not a regular file: " + filePath;
        return result;
    }

    // 3. Check read permissions / attempt open
    std::ifstream file(p, std::ios::binary);
    if (!file.is_open()) {
        result.status = FileStatus::PERMISSION_DENIED;
        result.errorMessage = "Cannot open file for reading (permission denied): " + filePath;
        return result;
    }

    // 4. Check file size
    auto size = fs::file_size(p, ec);
    if (ec) {
        result.status = FileStatus::IO_ERROR;
        result.errorMessage = "Failed to determine file size: " + ec.message();
        return result;
    }

    result.fileSize = static_cast<uint64_t>(size);

    if (result.fileSize == 0) {
        result.status = FileStatus::EMPTY_FILE;
        result.errorMessage = "File is empty (0 bytes): " + filePath;
        result.isValid = true; // An empty file is technically valid to process per TC-COMP-03
        return result;
    }

    result.isValid = true;
    result.status = FileStatus::OK;
    result.errorMessage = "";
    return result;
}

FileValidationResult FileValidator::validateOutputFile(const std::string& filePath, bool overwriteAllowed) {
    FileValidationResult result;
    result.isValid = false;

    if (filePath.empty()) {
        result.status = FileStatus::NOT_FOUND;
        result.errorMessage = "Output file path is empty.";
        return result;
    }

    std::error_code ec;
    fs::path p(filePath);

    // Check parent directory existence
    fs::path parent = p.parent_path();
    if (!parent.empty() && !fs::exists(parent, ec)) {
        result.status = FileStatus::NOT_FOUND;
        result.errorMessage = "Destination directory does not exist: " + parent.string();
        return result;
    }

    // Check if output file already exists and whether overwrite is allowed
    if (fs::exists(p, ec)) {
        if (!overwriteAllowed) {
            result.status = FileStatus::PERMISSION_DENIED;
            result.errorMessage = "Output file already exists and overwrite is disabled: " + filePath;
            return result;
        }
        if (fs::is_directory(p, ec)) {
            result.status = FileStatus::NOT_A_REGULAR_FILE;
            result.errorMessage = "Destination path is an existing directory: " + filePath;
            return result;
        }
    }

    // Check write permissions by opening
    std::ofstream out(p, std::ios::binary | std::ios::app);
    if (!out.is_open()) {
        result.status = FileStatus::PERMISSION_DENIED;
        result.errorMessage = "Cannot open destination file for writing (permission denied): " + filePath;
        return result;
    }

    result.isValid = true;
    result.status = FileStatus::OK;
    result.errorMessage = "";
    return result;
}

} // namespace jackfruit::io
