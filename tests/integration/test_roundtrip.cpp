/**
 * @file test_roundtrip.cpp
 * @brief End-to-end integration tests verifying round-trip compression and decompression
 *        fidelity across diverse datasets (JACK-F-001..008, TC-COMP-01..03, TC-DECOMP-01).
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <vector>

#include "archive/archive.hpp"
#include "archive/archive_validation.hpp"
#include "compression/compressor.hpp"
#include "decompression/decoder.hpp"
#include "security/crc32.hpp"

namespace fs = std::filesystem;
using namespace jackfruit::compression;
using namespace jackfruit::decompression;

class RoundTripIntegrationTest : public ::testing::Test {
protected:
    fs::path tempDir_;

    void SetUp() override {
        tempDir_ = fs::temp_directory_path() / "jackfruit_roundtrip_test";
        fs::create_directories(tempDir_);
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove_all(tempDir_, ec);
    }

    std::string createTestFile(const std::string& filename, const std::string& content) {
        fs::path p = tempDir_ / filename;
        std::ofstream out(p, std::ios::binary);
        out.write(content.data(), static_cast<std::streamsize>(content.size()));
        out.close();
        return p.string();
    }

    std::string createBinaryTestFile(const std::string& filename, const std::vector<uint8_t>& data) {
        fs::path p = tempDir_ / filename;
        std::ofstream out(p, std::ios::binary);
        out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
        out.close();
        return p.string();
    }

    bool filesAreByteIdentical(const std::string& pathA, const std::string& pathB) {
        std::ifstream fileA(pathA, std::ios::binary);
        std::ifstream fileB(pathB, std::ios::binary);

        if (!fileA || !fileB) {
            return false;
        }

        std::string contentA((std::istreambuf_iterator<char>(fileA)), std::istreambuf_iterator<char>());
        std::string contentB((std::istreambuf_iterator<char>(fileB)), std::istreambuf_iterator<char>());

        return contentA == contentB;
    }
};

// TC-DECOMP-01: Standard ASCII prose round-trip restoration
TEST_F(RoundTripIntegrationTest, StandardEnglishProse_TC_DECOMP_01) {
    const std::string prose =
        "Software engineering is an engineering branch associated with development of software product "
        "using well-defined scientific principles, methods and procedures. The outcome of software engineering "
        "is an efficient and reliable software product.\n"
        "Project Jackfruit demonstrates canonical Huffman compression, CRC-32 integrity validation, and "
        "strict Big-Endian binary container encapsulation adhering to IEEE specifications.";

    std::string originalPath = createTestFile("prose.txt", prose);
    std::string archivePath = (tempDir_ / "prose.jack").string();
    std::string restoredPath = (tempDir_ / "prose_restored.txt").string();

    Compressor compressor;
    auto compResult = compressor.compressFile(originalPath, archivePath);
    ASSERT_EQ(compResult.status, CompressionStatus::SUCCESS);

    bool decompSuccess = Decoder::decompressFile(archivePath, restoredPath);
    ASSERT_TRUE(decompSuccess);

    EXPECT_TRUE(filesAreByteIdentical(originalPath, restoredPath));
}

// TC-COMP-01: Highly redundant text with positive compression ratio (> 1.0x)
TEST_F(RoundTripIntegrationTest, LargeRedundantText_PositiveRatio_TC_COMP_01) {
    std::string repetitive;
    repetitive.reserve(128 * 1024);
    for (int i = 0; i < 2000; ++i) {
        repetitive += "Repeated sentence for entropy reduction testing. Pattern 1234567890.\n";
    }

    std::string originalPath = createTestFile("redundant.txt", repetitive);
    std::string archivePath = (tempDir_ / "redundant.jack").string();
    std::string restoredPath = (tempDir_ / "redundant_restored.txt").string();

    Compressor compressor;
    auto compResult = compressor.compressFile(originalPath, archivePath);
    ASSERT_EQ(compResult.status, CompressionStatus::SUCCESS);

    EXPECT_GT(compResult.compressionRatio, 1.0);
    EXPECT_GT(compResult.percentageSaved, 0.0);
    EXPECT_LT(compResult.compressedBytes, compResult.originalBytes);

    bool decompSuccess = Decoder::decompressFile(archivePath, restoredPath);
    ASSERT_TRUE(decompSuccess);

    EXPECT_TRUE(filesAreByteIdentical(originalPath, restoredPath));
}

// TC-COMP-02: Single distinct symbol repeated
TEST_F(RoundTripIntegrationTest, SingleSymbolRepeated_TC_COMP_02) {
    std::string singleSymbol(20000, 'Z');

    std::string originalPath = createTestFile("single_symbol.txt", singleSymbol);
    std::string archivePath = (tempDir_ / "single_symbol.jack").string();
    std::string restoredPath = (tempDir_ / "single_symbol_restored.txt").string();

    Compressor compressor;
    auto compResult = compressor.compressFile(originalPath, archivePath);
    ASSERT_EQ(compResult.status, CompressionStatus::SUCCESS);

    bool decompSuccess = Decoder::decompressFile(archivePath, restoredPath);
    ASSERT_TRUE(decompSuccess);

    EXPECT_TRUE(filesAreByteIdentical(originalPath, restoredPath));
}

// TC-COMP-03: Zero-byte empty file handling
TEST_F(RoundTripIntegrationTest, EmptyFile_TC_COMP_03) {
    std::string originalPath = createTestFile("empty.txt", "");
    std::string archivePath = (tempDir_ / "empty.jack").string();
    std::string restoredPath = (tempDir_ / "empty_restored.txt").string();

    Compressor compressor;
    auto compResult = compressor.compressFile(originalPath, archivePath);
    ASSERT_EQ(compResult.status, CompressionStatus::SUCCESS);
    EXPECT_EQ(compResult.originalBytes, 0u);

    bool decompSuccess = Decoder::decompressFile(archivePath, restoredPath);
    ASSERT_TRUE(decompSuccess);

    EXPECT_TRUE(fs::exists(restoredPath));
    EXPECT_EQ(fs::file_size(restoredPath), 0u);
    EXPECT_TRUE(filesAreByteIdentical(originalPath, restoredPath));
}

// All 256 binary symbol values uniformly distributed
TEST_F(RoundTripIntegrationTest, All256BinarySymbols_PreservesData) {
    std::vector<uint8_t> binaryData(65536);
    std::mt19937 rng(42);
    for (size_t i = 0; i < binaryData.size(); ++i) {
        binaryData[i] = static_cast<uint8_t>(rng() % 256);
    }

    std::string originalPath = createBinaryTestFile("binary.bin", binaryData);
    std::string archivePath = (tempDir_ / "binary.jack").string();
    std::string restoredPath = (tempDir_ / "binary_restored.bin").string();

    Compressor compressor;
    auto compResult = compressor.compressFile(originalPath, archivePath);
    ASSERT_EQ(compResult.status, CompressionStatus::SUCCESS);

    bool decompSuccess = Decoder::decompressFile(archivePath, restoredPath);
    ASSERT_TRUE(decompSuccess);

    EXPECT_TRUE(filesAreByteIdentical(originalPath, restoredPath));
}

// CRC-32 integrity validation across compression round-trip
TEST_F(RoundTripIntegrationTest, CRC32HeaderChecksumMatchesOriginalAndRestored) {
    const std::string text = "CRC-32 verification round-trip payload test with diverse characters: ~!@#$%^&*()_+";
    std::string originalPath = createTestFile("crc_test.txt", text);
    std::string archivePath = (tempDir_ / "crc_test.jack").string();
    std::string restoredPath = (tempDir_ / "crc_test_restored.txt").string();

    const uint32_t expectedCRC = CRC32::calculateFile(originalPath);

    Compressor compressor;
    auto compResult = compressor.compressFile(originalPath, archivePath);
    ASSERT_EQ(compResult.status, CompressionStatus::SUCCESS);
    EXPECT_EQ(compResult.checksumCRC32, expectedCRC);

    ArchiveHeader header{};
    std::vector<std::pair<uint8_t, uint64_t>> freqs;
    ASSERT_NO_THROW(ArchiveReader::readValidatedHeader(archivePath, header, freqs));
    EXPECT_EQ(header.checksumCRC32, expectedCRC);

    bool decompSuccess = Decoder::decompressFile(archivePath, restoredPath);
    ASSERT_TRUE(decompSuccess);

    const uint32_t restoredCRC = CRC32::calculateFile(restoredPath);
    EXPECT_EQ(restoredCRC, expectedCRC);
}

// Small edge-case sizes to test BitWriter/BitReader boundary padding (1, 2, 7, 8, 9, 15 bytes)
TEST_F(RoundTripIntegrationTest, SmallBoundaryPayloadSizes) {
    const std::vector<size_t> testSizes = {1, 2, 3, 7, 8, 9, 15, 16, 17, 31, 32, 33};

    for (size_t sz : testSizes) {
        std::string payload(sz, 'X');
        for (size_t i = 0; i < sz; ++i) {
            payload[i] = static_cast<char>('A' + (i % 26));
        }

        std::string originalPath = createTestFile("size_" + std::to_string(sz) + ".txt", payload);
        std::string archivePath = (tempDir_ / ("size_" + std::to_string(sz) + ".jack")).string();
        std::string restoredPath = (tempDir_ / ("size_" + std::to_string(sz) + "_restored.txt")).string();

        Compressor compressor;
        auto compResult = compressor.compressFile(originalPath, archivePath);
        ASSERT_EQ(compResult.status, CompressionStatus::SUCCESS) << "Failed on size " << sz;

        bool decompSuccess = Decoder::decompressFile(archivePath, restoredPath);
        ASSERT_TRUE(decompSuccess) << "Decompression failed on size " << sz;

        EXPECT_TRUE(filesAreByteIdentical(originalPath, restoredPath)) << "Mismatch on size " << sz;
    }
}
