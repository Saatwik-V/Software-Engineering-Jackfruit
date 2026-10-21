/**
 * @file test_compressor.cpp
 * @brief Unit and integration tests for top-level Compressor pipeline (JACK-F-001, JACK-F-004, JACK-F-015).
 */

#include <gtest/gtest.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "archive/archive.hpp"
#include "archive/archive_validation.hpp"
#include "compression/compressor.hpp"
#include "security/crc32.hpp"

using namespace jackfruit::compression;

class CompressorTest : public ::testing::Test {
protected:
    std::filesystem::path tempDir_;
    std::string inputPath_;
    std::string outputPath_;

    void SetUp() override {
        tempDir_ = std::filesystem::temp_directory_path() / "jackfruit_compressor_test";
        std::filesystem::create_directories(tempDir_);

        const auto testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
        inputPath_ = (tempDir_ / (std::string(testName) + "_in.dat")).string();
        outputPath_ = (tempDir_ / (std::string(testName) + "_out.jack")).string();
    }

    void TearDown() override {
        std::error_code ec;
        std::filesystem::remove(inputPath_, ec);
        std::filesystem::remove(outputPath_, ec);
        std::filesystem::remove_all(tempDir_, ec);
    }

    void writeInputFile(const std::string& content) {
        std::ofstream out(inputPath_, std::ios::binary | std::ios::trunc);
        ASSERT_TRUE(out.is_open());
        out.write(content.data(), static_cast<std::streamsize>(content.size()));
        out.close();
    }

    void writeInputBinary(const std::vector<uint8_t>& bytes) {
        std::ofstream out(inputPath_, std::ios::binary | std::ios::trunc);
        ASSERT_TRUE(out.is_open());
        out.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        out.close();
    }
};

TEST_F(CompressorTest, CompressEmptyFile_Creates28ByteArchiveWithEmptyFlag) {
    writeInputFile(""); // 0 bytes

    Compressor compressor;
    const auto result = compressor.compressFile(inputPath_, outputPath_);

    EXPECT_EQ(result.status, CompressionStatus::SUCCESS);
    EXPECT_EQ(result.originalBytes, 0u);
    EXPECT_EQ(result.compressedBytes, 28u);
    EXPECT_EQ(result.checksumCRC32, 0x00000000U);
    EXPECT_DOUBLE_EQ(result.compressionRatio, 0.0);
    EXPECT_DOUBLE_EQ(result.percentageSaved, 0.0);

    // Verify written archive contents with ArchiveReader
    ArchiveHeader header{};
    const bool readOk = ArchiveReader::readFixedHeader(outputPath_, header);
    EXPECT_TRUE(readOk);
    EXPECT_EQ(std::string(header.magic, 4), "JACK");
    EXPECT_EQ(header.version, 1);
    EXPECT_EQ(header.flags, ARCHIVE_FLAG_IS_EMPTY);
    EXPECT_EQ(header.originalSize, 0u);
    EXPECT_EQ(header.checksumCRC32, 0u);
    EXPECT_EQ(header.symbolCount, 0u);
    EXPECT_EQ(header.totalBits, 0u);
}

TEST_F(CompressorTest, CompressSimpleAscii_ProducesValidArchiveWithCorrectCRC32) {
    const std::string text = "The quick brown fox jumps over the lazy dog. 1234567890!";
    writeInputFile(text);

    const uint32_t expectedCRC = CRC32::calculateFile(inputPath_);

    Compressor compressor;
    const auto result = compressor.compressFile(inputPath_, outputPath_);

    EXPECT_EQ(result.status, CompressionStatus::SUCCESS);
    EXPECT_EQ(result.originalBytes, text.size());
    EXPECT_GT(result.compressedBytes, 0u);
    EXPECT_EQ(result.checksumCRC32, expectedCRC);

    // Structural validation check
    ArchiveHeader header{};
    std::vector<std::pair<uint8_t, uint64_t>> frequencies;
    EXPECT_NO_THROW({
        ArchiveReader::readValidatedHeader(outputPath_, header, frequencies);
    });

    EXPECT_EQ(header.originalSize, text.size());
    EXPECT_EQ(header.checksumCRC32, expectedCRC);
    EXPECT_GT(header.symbolCount, 0u);
    EXPECT_GT(header.totalBits, 0u);

    // Verify frequency sum equals original size
    uint64_t totalFreq = 0;
    for (const auto& [sym, count] : frequencies) {
        totalFreq += count;
    }
    EXPECT_EQ(totalFreq, text.size());
}

TEST_F(CompressorTest, CompressSingleDistinctCharacter_TC_COMP_02) {
    const std::string text(1000, 'A');
    writeInputFile(text);

    Compressor compressor;
    const auto result = compressor.compressFile(inputPath_, outputPath_);

    EXPECT_EQ(result.status, CompressionStatus::SUCCESS);
    EXPECT_EQ(result.originalBytes, 1000u);

    ArchiveHeader header{};
    std::vector<std::pair<uint8_t, uint64_t>> frequencies;
    ArchiveReader::readValidatedHeader(outputPath_, header, frequencies);

    EXPECT_EQ(header.symbolCount, 1u);
    EXPECT_EQ(frequencies.size(), 1u);
    EXPECT_EQ(frequencies[0].first, 'A');
    EXPECT_EQ(frequencies[0].second, 1000u);
    // Single symbol should take 1 bit per character = 1000 total bits
    EXPECT_EQ(header.totalBits, 1000u);
}

TEST_F(CompressorTest, CompressNonExistentInput_ReturnsFileNotFound) {
    const std::string nonExistent = (tempDir_ / "missing_file.txt").string();

    Compressor compressor;
    const auto result = compressor.compressFile(nonExistent, outputPath_);

    EXPECT_EQ(result.status, CompressionStatus::ERROR_FILE_NOT_FOUND);
    EXPECT_FALSE(result.errorMessage.empty());
}

TEST_F(CompressorTest, CompressToInvalidDirectory_ReturnsCannotWriteArchive) {
    writeInputFile("Hello");
    const std::string badOutput = (tempDir_ / "non_existent_subdir" / "out.jack").string();

    Compressor compressor;
    const auto result = compressor.compressFile(inputPath_, badOutput);

    EXPECT_EQ(result.status, CompressionStatus::ERROR_CANNOT_WRITE_ARCHIVE);
    EXPECT_FALSE(result.errorMessage.empty());
}

TEST_F(CompressorTest, CompressBinaryDataAll256Symbols_ValidatesStructure) {
    std::vector<uint8_t> binaryData;
    binaryData.reserve(256 * 4);
    for (int rep = 0; rep < 4; ++rep) {
        for (int b = 0; b < 256; ++b) {
            binaryData.push_back(static_cast<uint8_t>(b));
        }
    }
    writeInputBinary(binaryData);

    Compressor compressor;
    const auto result = compressor.compressFile(inputPath_, outputPath_);

    EXPECT_EQ(result.status, CompressionStatus::SUCCESS);
    EXPECT_EQ(result.originalBytes, 1024u);

    ArchiveHeader header{};
    std::vector<std::pair<uint8_t, uint64_t>> frequencies;
    EXPECT_NO_THROW({
        ArchiveReader::readValidatedHeader(outputPath_, header, frequencies);
    });
    EXPECT_EQ(header.symbolCount, 256u);
    EXPECT_EQ(frequencies.size(), 256u);
}

TEST_F(CompressorTest, CompressPerformanceStatistics_ValidCalculations) {
    const std::string repeatedPattern = "Repeated pattern text for compression ratio test. "
                                        "Repeated pattern text for compression ratio test. "
                                        "Repeated pattern text for compression ratio test. ";
    writeInputFile(repeatedPattern);

    Compressor compressor;
    const auto result = compressor.compressFile(inputPath_, outputPath_);

    EXPECT_EQ(result.status, CompressionStatus::SUCCESS);
    EXPECT_GE(result.executionTimeMs, 0.0);
    EXPECT_GT(result.compressionRatio, 0.0);
    const double expectedSaved = (1.0 - (static_cast<double>(result.compressedBytes) /
                                         static_cast<double>(result.originalBytes))) * 100.0;
    EXPECT_NEAR(result.percentageSaved, expectedSaved, 1e-4);
}
