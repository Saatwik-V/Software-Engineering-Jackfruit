#include <gtest/gtest.h>
#include "io/file_validator.hpp"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;
using namespace jackfruit::io;

class FileValidatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempDir_ = fs::temp_directory_path() / "jackfruit_validator_test";
        fs::create_directories(tempDir_);

        validFilePath_ = (tempDir_ / "sample.txt").string();
        std::ofstream out(validFilePath_);
        out << "Hello Jackfruit!";
        out.close();

        emptyFilePath_ = (tempDir_ / "empty.txt").string();
        std::ofstream emptyOut(emptyFilePath_);
        emptyOut.close();
    }

    void TearDown() override {
        fs::remove_all(tempDir_);
    }

    fs::path tempDir_;
    std::string validFilePath_;
    std::string emptyFilePath_;
};

TEST_F(FileValidatorTest, ValidExistingFilePasses) {
    auto result = FileValidator::validateInputFile(validFilePath_);
    EXPECT_TRUE(result.isValid);
    EXPECT_EQ(result.status, FileStatus::OK);
    EXPECT_GT(result.fileSize, 0u);
    EXPECT_TRUE(result.errorMessage.empty());
}

TEST_F(FileValidatorTest, NonExistentFileFails) {
    std::string nonExistent = (tempDir_ / "does_not_exist.txt").string();
    auto result = FileValidator::validateInputFile(nonExistent);
    EXPECT_FALSE(result.isValid);
    EXPECT_EQ(result.status, FileStatus::NOT_FOUND);
    EXPECT_FALSE(result.errorMessage.empty());
}

TEST_F(FileValidatorTest, EmptyPathFails) {
    auto result = FileValidator::validateInputFile("");
    EXPECT_FALSE(result.isValid);
    EXPECT_EQ(result.status, FileStatus::NOT_FOUND);
}

TEST_F(FileValidatorTest, DirectoryAsInputFails) {
    auto result = FileValidator::validateInputFile(tempDir_.string());
    EXPECT_FALSE(result.isValid);
    EXPECT_EQ(result.status, FileStatus::NOT_A_REGULAR_FILE);
}

TEST_F(FileValidatorTest, EmptyFileHandledCorrectly) {
    auto result = FileValidator::validateInputFile(emptyFilePath_);
    EXPECT_TRUE(result.isValid);
    EXPECT_EQ(result.status, FileStatus::EMPTY_FILE);
    EXPECT_EQ(result.fileSize, 0u);
}

TEST_F(FileValidatorTest, ValidOutputFileCheck) {
    std::string outputPath = (tempDir_ / "output.jack").string();
    auto result = FileValidator::validateOutputFile(outputPath, true);
    EXPECT_TRUE(result.isValid);
    EXPECT_EQ(result.status, FileStatus::OK);
}

TEST_F(FileValidatorTest, OutputDirectoryNonExistentFails) {
    std::string invalidOutput = (tempDir_ / "no_such_dir" / "output.jack").string();
    auto result = FileValidator::validateOutputFile(invalidOutput, true);
    EXPECT_FALSE(result.isValid);
    EXPECT_EQ(result.status, FileStatus::NOT_FOUND);
}
