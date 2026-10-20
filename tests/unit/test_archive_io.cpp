#include <gtest/gtest.h>
#include "archive/archive.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>

class ArchiveIOTest : public ::testing::Test {
protected:
    std::string testFilePath = "temp_test_archive.jack";

    void TearDown() override {
        std::remove(testFilePath.c_str());
    }

    void writeDummyFile(const std::vector<uint8_t>& bytes) {
        std::ofstream out(testFilePath, std::ios::binary);
        out.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        out.close();
    }
};

// AC: Correctly reads 18-byte header into ArchiveHeader struct
TEST_F(ArchiveIOTest, ValidFixedHeaderRead) {
    std::vector<uint8_t> validHeader = {
        'J', 'A', 'C', 'K',             // Magic (4 bytes)
        0x01,                           // Version = 1 (1 byte)
        0x00,                           // Flags (1 byte)
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x04, 0xD2,         // originalSize = 1234 (8 bytes, big-endian)
        0x12, 0x34, 0x56, 0x78          // checksumCRC32 = 0x12345678 (4 bytes, big-endian)
    };
    writeDummyFile(validHeader);

    ArchiveHeader header{};
    bool status = false;
    EXPECT_NO_THROW({
        status = ArchiveReader::readFixedHeader(testFilePath, header);
    });

    EXPECT_TRUE(status);
    EXPECT_EQ(std::string(header.magic, 4), "JACK");
    EXPECT_EQ(header.version, 1);
    EXPECT_EQ(header.flags, 0);
    EXPECT_EQ(header.originalSize, 1234ULL);
    EXPECT_EQ(header.checksumCRC32, 0x12345678U);
}

// AC: Rejects files with invalid magic bytes with exit code 2 (ERROR_INVALID_HEADER)
TEST_F(ArchiveIOTest, RejectsInvalidMagicBytes) {
    std::vector<uint8_t> badMagic = {
        'B', 'A', 'D', '!',             // Bad Magic
        0x01,
        0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
        0x00, 0x00, 0x00, 0x00
    };
    writeDummyFile(badMagic);

    ArchiveHeader header{};
    try {
        ArchiveReader::readFixedHeader(testFilePath, header);
        FAIL() << "Expected ERROR_INVALID_HEADER exception";
    } catch (int err) {
        EXPECT_EQ(err, ERROR_INVALID_HEADER);
        EXPECT_EQ(err, 2);
    }
}

// AC: Rejects unsupported format version with exit code 2
TEST_F(ArchiveIOTest, RejectsUnsupportedVersion) {
    std::vector<uint8_t> badVersion = {
        'J', 'A', 'C', 'K',
        0x02,                           // Unsupported version 2
        0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
        0x00, 0x00, 0x00, 0x00
    };
    writeDummyFile(badVersion);

    ArchiveHeader header{};
    try {
        ArchiveReader::readFixedHeader(testFilePath, header);
        FAIL() << "Expected ERROR_INVALID_HEADER exception";
    } catch (int err) {
        EXPECT_EQ(err, ERROR_INVALID_HEADER);
        EXPECT_EQ(err, 2);
    }
}

// AC: Rejects truncated header (< 18 bytes) with exit code 2
TEST_F(ArchiveIOTest, RejectsTruncatedHeader) {
    std::vector<uint8_t> truncated = {'J', 'A', 'C', 'K', 0x01}; // only 5 bytes
    writeDummyFile(truncated);

    ArchiveHeader header{};
    try {
        ArchiveReader::readFixedHeader(testFilePath, header);
        FAIL() << "Expected ERROR_INVALID_HEADER exception";
    } catch (int err) {
        EXPECT_EQ(err, ERROR_INVALID_HEADER);
        EXPECT_EQ(err, 2);
    }
}
