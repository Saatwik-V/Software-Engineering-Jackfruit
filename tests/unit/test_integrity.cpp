/**
 * @file test_integrity.cpp
 * @brief Unit tests for UC-04 IntegrityValidator (JACK-F-013..016, JACK-SR-001..005, TC-VALID-01, TC-SEC-01).
 */

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "compression/compressor.hpp"
#include "security/crc32.hpp"
#include "security/integrity_validator.hpp"

namespace {

namespace fs = std::filesystem;
using namespace jackfruit::security;

class IntegrityValidatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir_ = fs::temp_directory_path() / "jackfruit_integrity_tests";
        fs::create_directories(testDir_);
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove_all(testDir_, ec);
    }

    fs::path testDir_;

    std::string createTestFile(const std::string& filename, const std::string& content) {
        fs::path p = testDir_ / filename;
        std::ofstream out(p, std::ios::binary | std::ios::trunc);
        out.write(content.data(), static_cast<std::streamsize>(content.size()));
        return p.string();
    }

    std::string createBinaryTestFile(const std::string& filename, const std::vector<uint8_t>& data) {
        fs::path p = testDir_ / filename;
        std::ofstream out(p, std::ios::binary | std::ios::trunc);
        out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
        return p.string();
    }
};

TEST_F(IntegrityValidatorTest, NonExistentFileReturnsCode1) {
    const std::string nonExistent = (testDir_ / "missing.jack").string();
    const auto result = IntegrityValidator::validateArchive(nonExistent);

    EXPECT_EQ(result.status, ValidationStatus::ERROR_FILE_NOT_FOUND);
    EXPECT_EQ(result.exitCode, 1);
    EXPECT_FALSE(result.message.empty());
}

TEST_F(IntegrityValidatorTest, TooSmallFileReturnsInvalidHeaderCode2) {
    const std::string smallFile = createTestFile("small.jack", "JACK1");
    const auto result = IntegrityValidator::validateArchive(smallFile);

    EXPECT_EQ(result.status, ValidationStatus::ERROR_INVALID_HEADER);
    EXPECT_EQ(result.exitCode, 2);
}

TEST_F(IntegrityValidatorTest, ValidTextArchiveReturnsValidAndCode0) {
    const std::string input = createTestFile("input.txt", "The quick brown fox jumps over the lazy dog 1234567890!");
    const std::string archive = (testDir_ / "input.jack").string();

    jackfruit::compression::Compressor compressor;
    const auto compRes = compressor.compressFile(input, archive);
    ASSERT_EQ(compRes.status, jackfruit::compression::CompressionStatus::SUCCESS);

    const auto result = IntegrityValidator::validateArchive(archive);

    EXPECT_EQ(result.status, ValidationStatus::VALID);
    EXPECT_EQ(result.exitCode, 0);
    EXPECT_EQ(result.expectedChecksum, compRes.checksumCRC32);
    EXPECT_EQ(result.computedChecksum, compRes.checksumCRC32);
    EXPECT_EQ(result.payloadBytes, compRes.originalBytes);
}

TEST_F(IntegrityValidatorTest, ValidEmptyArchiveReturnsValidAndCode0) {
    const std::string input = createTestFile("empty.txt", "");
    const std::string archive = (testDir_ / "empty.jack").string();

    jackfruit::compression::Compressor compressor;
    const auto compRes = compressor.compressFile(input, archive);
    ASSERT_EQ(compRes.status, jackfruit::compression::CompressionStatus::SUCCESS);

    const auto result = IntegrityValidator::validateArchive(archive);

    EXPECT_EQ(result.status, ValidationStatus::VALID);
    EXPECT_EQ(result.exitCode, 0);
    EXPECT_EQ(result.expectedChecksum, 0U);
    EXPECT_EQ(result.computedChecksum, 0U);
    EXPECT_EQ(result.payloadBytes, 0U);
}

TEST_F(IntegrityValidatorTest, ValidSingleSymbolArchiveReturnsValid) {
    const std::string input = createTestFile("single.txt", "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    const std::string archive = (testDir_ / "single.jack").string();

    jackfruit::compression::Compressor compressor;
    const auto compRes = compressor.compressFile(input, archive);
    ASSERT_EQ(compRes.status, jackfruit::compression::CompressionStatus::SUCCESS);

    const auto result = IntegrityValidator::validateArchive(archive);

    EXPECT_EQ(result.status, ValidationStatus::VALID);
    EXPECT_EQ(result.exitCode, 0);
    EXPECT_EQ(result.expectedChecksum, compRes.checksumCRC32);
    EXPECT_EQ(result.computedChecksum, compRes.checksumCRC32);
}

TEST_F(IntegrityValidatorTest, ValidBinaryDataReturnsValid) {
    std::vector<uint8_t> binaryData(1024);
    for (size_t i = 0; i < binaryData.size(); ++i) {
        binaryData[i] = static_cast<uint8_t>(i % 256);
    }
    const std::string input = createBinaryTestFile("binary.bin", binaryData);
    const std::string archive = (testDir_ / "binary.jack").string();

    jackfruit::compression::Compressor compressor;
    const auto compRes = compressor.compressFile(input, archive);
    ASSERT_EQ(compRes.status, jackfruit::compression::CompressionStatus::SUCCESS);

    const auto result = IntegrityValidator::validateArchive(archive);

    EXPECT_EQ(result.status, ValidationStatus::VALID);
    EXPECT_EQ(result.exitCode, 0);
    EXPECT_EQ(result.expectedChecksum, compRes.checksumCRC32);
    EXPECT_EQ(result.computedChecksum, compRes.checksumCRC32);
}

TEST_F(IntegrityValidatorTest, DetectSingleBitPayloadCorruption_TC_VALID_01) {
    const std::string input = createTestFile("corrupt_target.txt",
        "Jackfruit UC-04 Payload Corruption Detection Test Vector (TC-VALID-01). Bit-rot integrity check.");
    const std::string archive = (testDir_ / "corrupt_target.jack").string();

    jackfruit::compression::Compressor compressor;
    const auto compRes = compressor.compressFile(input, archive);
    ASSERT_EQ(compRes.status, jackfruit::compression::CompressionStatus::SUCCESS);

    // Read full archive into memory
    std::ifstream in(archive, std::ios::binary);
    ASSERT_TRUE(in.is_open());
    std::vector<uint8_t> raw((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    // Flip 1 bit in an active payload byte (avoiding trailing padding bits)
    ASSERT_GT(raw.size(), 32U);
    const size_t activePayloadByte = raw.size() - 2;
    raw[activePayloadByte] ^= 0x80;

    // Overwrite the archive with the 1-bit corrupted payload
    std::ofstream out(archive, std::ios::binary | std::ios::trunc);
    ASSERT_TRUE(out.is_open());
    out.write(reinterpret_cast<const char*>(raw.data()), static_cast<std::streamsize>(raw.size()));
    out.close();

    // Validate must detect corruption and return exit code 3 or 4
    const auto result = IntegrityValidator::validateArchive(archive);

    EXPECT_TRUE(result.status == ValidationStatus::ERROR_CHECKSUM_MISMATCH ||
                result.status == ValidationStatus::ERROR_TRUNCATED_PAYLOAD);
    EXPECT_TRUE(result.exitCode == 3 || result.exitCode == 4);
}

TEST_F(IntegrityValidatorTest, DetectHeaderChecksumTampering) {
    const std::string input = createTestFile("header_tamper.txt", "Checksum tampering test string.");
    const std::string archive = (testDir_ / "header_tamper.jack").string();

    jackfruit::compression::Compressor compressor;
    const auto compRes = compressor.compressFile(input, archive);
    ASSERT_EQ(compRes.status, jackfruit::compression::CompressionStatus::SUCCESS);

    // Open file and tamper stored CRC-32 at offset 0x0E (14)
    std::fstream file(archive, std::ios::in | std::ios::out | std::ios::binary);
    ASSERT_TRUE(file.is_open());
    file.seekp(14, std::ios::beg);
    uint32_t tamperedCRC = compRes.checksumCRC32 ^ 0xFFFFFFFFU;
    file.write(reinterpret_cast<const char*>(&tamperedCRC), sizeof(tamperedCRC));
    file.close();

    const auto result = IntegrityValidator::validateArchive(archive);

    EXPECT_EQ(result.status, ValidationStatus::ERROR_CHECKSUM_MISMATCH);
    EXPECT_EQ(result.exitCode, 3);
}

TEST_F(IntegrityValidatorTest, RejectInvalidMagicSignature_TC_SEC_01) {
    const std::string input = createTestFile("magic.txt", "Magic byte verification test.");
    const std::string archive = (testDir_ / "bad_magic.jack").string();

    jackfruit::compression::Compressor compressor;
    const auto compRes = compressor.compressFile(input, archive);
    ASSERT_EQ(compRes.status, jackfruit::compression::CompressionStatus::SUCCESS);

    // Corrupt magic bytes from "JACK" to "NOPE"
    std::fstream file(archive, std::ios::in | std::ios::out | std::ios::binary);
    ASSERT_TRUE(file.is_open());
    file.seekp(0, std::ios::beg);
    const char badMagic[4] = {'N', 'O', 'P', 'E'};
    file.write(badMagic, 4);
    file.close();

    const auto result = IntegrityValidator::validateArchive(archive);

    EXPECT_EQ(result.status, ValidationStatus::ERROR_INVALID_HEADER);
    EXPECT_EQ(result.exitCode, 2);
}

TEST_F(IntegrityValidatorTest, RejectUnsupportedVersion) {
    const std::string input = createTestFile("version.txt", "Version test.");
    const std::string archive = (testDir_ / "bad_version.jack").string();

    jackfruit::compression::Compressor compressor;
    const auto compRes = compressor.compressFile(input, archive);
    ASSERT_EQ(compRes.status, jackfruit::compression::CompressionStatus::SUCCESS);

    // Corrupt version byte at offset 4 to 0x05
    std::fstream file(archive, std::ios::in | std::ios::out | std::ios::binary);
    ASSERT_TRUE(file.is_open());
    file.seekp(4, std::ios::beg);
    const uint8_t badVersion = 0x05;
    file.write(reinterpret_cast<const char*>(&badVersion), 1);
    file.close();

    const auto result = IntegrityValidator::validateArchive(archive);

    EXPECT_EQ(result.status, ValidationStatus::ERROR_INVALID_HEADER);
    EXPECT_EQ(result.exitCode, 2);
}

TEST_F(IntegrityValidatorTest, RejectTruncatedArchiveFile) {
    const std::string input = createTestFile("truncate.txt", "Truncate me into pieces please!");
    const std::string archive = (testDir_ / "truncated.jack").string();

    jackfruit::compression::Compressor compressor;
    const auto compRes = compressor.compressFile(input, archive);
    ASSERT_EQ(compRes.status, jackfruit::compression::CompressionStatus::SUCCESS);

    // Truncate file by 10 bytes
    const uintmax_t origSize = fs::file_size(archive);
    ASSERT_GT(origSize, 15U);
    fs::resize_file(archive, origSize - 10);

    const auto result = IntegrityValidator::validateArchive(archive);

    EXPECT_TRUE(result.status == ValidationStatus::ERROR_INVALID_HEADER ||
                result.status == ValidationStatus::ERROR_TRUNCATED_PAYLOAD);
    EXPECT_TRUE(result.exitCode == 2 || result.exitCode == 4);
}

TEST_F(IntegrityValidatorTest, FormatValidationReportOutputsReadableStrings) {
    ValidationResult passResult{ValidationStatus::VALID, 0, 0x12345678, 0x12345678, 100, "Integrity verified"};
    const std::string reportPlain = IntegrityValidator::formatValidationReport(passResult, false);
    EXPECT_NE(reportPlain.find("PASS (VALID)"), std::string::npos);
    EXPECT_NE(reportPlain.find("12345678"), std::string::npos);

    ValidationResult failResult{ValidationStatus::ERROR_CHECKSUM_MISMATCH, 3, 0x12345678, 0x9ABCDEF0, 100, "Checksum mismatch"};
    const std::string reportFail = IntegrityValidator::formatValidationReport(failResult, false);
    EXPECT_NE(reportFail.find("FAIL (CHECKSUM_MISMATCH)"), std::string::npos);
    EXPECT_NE(reportFail.find("Exit Code:          3"), std::string::npos);
}

} // namespace
