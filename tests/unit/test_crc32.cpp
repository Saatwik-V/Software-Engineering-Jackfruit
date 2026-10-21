#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "archive/archive.hpp"
#include "compression/compressor.hpp"
#include "security/crc32.hpp"

TEST(CRC32Test, EmptyBufferReturnsZero) {
    EXPECT_EQ(CRC32::calculate(nullptr, 0), 0x00000000U);
}

TEST(CRC32Test, StandardCheckValueMatchesIEEE8023) {
    const std::string input = "123456789";
    EXPECT_EQ(CRC32::calculate(reinterpret_cast<const uint8_t*>(input.data()), input.size()),
              0xCBF43926U);
}

TEST(CRC32Test, IncrementalUpdatesMatchSinglePassCalculation) {
    const std::string first = "1234";
    const std::string second = "56789";
    uint32_t crc = CRC32::calculate(reinterpret_cast<const uint8_t*>(first.data()), first.size());
    crc = CRC32::update(crc, reinterpret_cast<const uint8_t*>(second.data()), second.size());

    EXPECT_EQ(crc, 0xCBF43926U);
}

TEST(CRC32Test, FileCalculationReadsBinaryBytes) {
    const std::string path = "temp_crc32_test.bin";
    const std::vector<uint8_t> contents = {0x00, 0x80, 0xFF, 0x01};
    {
        std::ofstream output(path, std::ios::binary);
        ASSERT_TRUE(output);
        output.write(reinterpret_cast<const char*>(contents.data()),
                     static_cast<std::streamsize>(contents.size()));
        ASSERT_TRUE(output);
    }

    const uint32_t expected = CRC32::calculate(contents.data(), contents.size());
    EXPECT_EQ(CRC32::calculateFile(path), expected);
    std::remove(path.c_str());
}

TEST(CRC32Test, ArchiveHeaderChecksumMatchesOriginalFileCRC_Issue36) {
    namespace fs = std::filesystem;
    const fs::path tempDir = fs::temp_directory_path() / "jackfruit_crc_verify_test";
    fs::create_directories(tempDir);

    const std::string inputPath = (tempDir / "input_crc_test.txt").string();
    const std::string archivePath = (tempDir / "output_crc_test.jack").string();

    const std::string testData = "Jackfruit IEEE 802.3 CRC-32 integrity pipeline verification test string! Issue #36.";
    {
        std::ofstream out(inputPath, std::ios::binary | std::ios::trunc);
        ASSERT_TRUE(out.is_open());
        out.write(testData.data(), static_cast<std::streamsize>(testData.size()));
    }

    const uint32_t expectedCRC = CRC32::calculateFile(inputPath);
    EXPECT_NE(expectedCRC, 0U);

    jackfruit::compression::Compressor compressor;
    const auto result = compressor.compressFile(inputPath, archivePath);

    ASSERT_EQ(result.status, jackfruit::compression::CompressionStatus::SUCCESS);
    EXPECT_EQ(result.checksumCRC32, expectedCRC);

    // Read archive header and verify header checksum matches computed CRC-32
    ArchiveHeader header{};
    std::vector<std::pair<uint8_t, uint64_t>> frequencies;
    ASSERT_NO_THROW({
        ArchiveReader::readValidatedHeader(archivePath, header, frequencies);
    });

    EXPECT_EQ(header.checksumCRC32, expectedCRC);
    EXPECT_EQ(header.originalSize, testData.size());

    std::error_code ec;
    fs::remove(inputPath, ec);
    fs::remove(archivePath, ec);
    fs::remove_all(tempDir, ec);
}

TEST(CRC32Test, EmptyArchiveHeaderChecksumIsZero_Issue36) {
    namespace fs = std::filesystem;
    const fs::path tempDir = fs::temp_directory_path() / "jackfruit_crc_empty_test";
    fs::create_directories(tempDir);

    const std::string inputPath = (tempDir / "empty.txt").string();
    const std::string archivePath = (tempDir / "empty.jack").string();

    {
        std::ofstream out(inputPath, std::ios::binary | std::ios::trunc);
        ASSERT_TRUE(out.is_open());
    }

    jackfruit::compression::Compressor compressor;
    const auto result = compressor.compressFile(inputPath, archivePath);

    ASSERT_EQ(result.status, jackfruit::compression::CompressionStatus::SUCCESS);
    EXPECT_EQ(result.checksumCRC32, 0U);

    ArchiveHeader header{};
    std::vector<std::pair<uint8_t, uint64_t>> frequencies;
    ASSERT_NO_THROW({
        ArchiveReader::readValidatedHeader(archivePath, header, frequencies);
    });

    EXPECT_EQ(header.checksumCRC32, 0U);

    std::error_code ec;
    fs::remove(inputPath, ec);
    fs::remove(archivePath, ec);
    fs::remove_all(tempDir, ec);
}
