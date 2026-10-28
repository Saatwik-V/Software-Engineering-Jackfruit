#include <gtest/gtest.h>
#include "stats/statistics_engine.hpp"
#include "compression/compressor.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

// Helper: write a temp text file
static std::string writeTempFile(const std::string& name, const std::string& content) {
    const fs::path dir = fs::path("build") / "test_stats_tmp"; // NOSONAR
    fs::create_directories(dir);
    const std::string path = (dir / name).string();
    std::ofstream f(path);
    f << content;
    return path;
}

class StatisticsEngineTest : public ::testing::Test {
protected:
    std::string srcPath;
    std::string archivePath;

    void SetUp() override {
    std::string corpus;
    for (int i = 0; i < 300; ++i)
        corpus += "the quick brown fox jumps over the lazy dog ";
    srcPath = writeTempFile("stat_test_input.txt", corpus);
    archivePath = srcPath + ".jack";

    jackfruit::compression::Compressor compressor;
    auto result = compressor.compressFile(srcPath, archivePath);
    ASSERT_EQ(result.status, jackfruit::compression::CompressionStatus::SUCCESS)
        << "SetUp compression failed: " << result.errorMessage;
}

    void TearDown() override {
        fs::remove_all(fs::path("build") / "test_stats_tmp");
    }
};

// TC-STAT-01: valid archive returns populated stats
TEST_F(StatisticsEngineTest, TC_STAT_01_ValidArchiveReturnsStats) {
    jackfruit::stats::ArchiveStats stats;
    ASSERT_NO_THROW(stats = jackfruit::stats::StatisticsEngine::computeStats(archivePath));

    EXPECT_GT(stats.originalBytes,    0ULL) << "Original size must be > 0";
    EXPECT_GT(stats.compressedBytes,  0ULL) << "Compressed size must be > 0";
    EXPECT_GT(stats.symbolCount,      0)    << "Symbol count must be > 0";
    EXPECT_GT(stats.totalBits,        0ULL) << "Total bits must be > 0";
    EXPECT_GT(stats.compressionRatio, 0.0)  << "Ratio must be positive";
    EXPECT_NE(stats.checksumCRC32,    0U)   << "CRC-32 must be non-zero";
}

// Huffman should compress a repetitive text file better than 1:1
TEST_F(StatisticsEngineTest, CompressionRatioAboveOne) {
    const auto stats = jackfruit::stats::StatisticsEngine::computeStats(archivePath);
    EXPECT_GT(stats.compressionRatio, 1.0);
}

// Space saved must be a valid percentage
TEST_F(StatisticsEngineTest, SpaceSavedIsValidPercentage) {
    const auto stats = jackfruit::stats::StatisticsEngine::computeStats(archivePath);
    EXPECT_GT(stats.spaceSavedPercent,   0.0);
    EXPECT_LT(stats.spaceSavedPercent, 100.0);
}

// Bits per symbol must be positive
TEST_F(StatisticsEngineTest, BitsPerSymbolIsPositive) {
    const auto stats = jackfruit::stats::StatisticsEngine::computeStats(archivePath);
    EXPECT_GT(stats.bitsPerSymbol, 0.0);
}

// Missing file must throw
TEST(StatisticsEngineStandaloneTest, ThrowsOnMissingFile) {
    EXPECT_THROW(
        jackfruit::stats::StatisticsEngine::computeStats("/nonexistent/fake.jack"),
        int
    );
}

// printStatsTable smoke test — must not crash and must honour the output stream
TEST_F(StatisticsEngineTest, PrintStatsTableDoesNotCrash) {
    const auto stats = jackfruit::stats::StatisticsEngine::computeStats(archivePath);
    std::ostringstream out;
    EXPECT_NO_THROW(
        jackfruit::stats::StatisticsEngine::printStatsTable(stats, archivePath, out)
    );
    EXPECT_NE(out.str().find("Archive Statistics"), std::string::npos);
}

// Table formatting must use megabyte units for megabyte-scale payloads
TEST_F(StatisticsEngineTest, PrintStatsTableHandlesMegabytePayload) {
    const fs::path dir = fs::path("build") / "test_stats_tmp"; // NOSONAR - test-only temp dir
    const std::string bigSrc = (dir / "big_input.txt").string();
    {
        std::ofstream f(bigSrc, std::ios::binary);
        const std::string chunk(1024, 'x');
        for (int i = 0; i < 1100; ++i) {
            f << chunk;
        }
    }
    const std::string bigArchive = bigSrc + ".jack";

    jackfruit::compression::Compressor compressor;
    const auto result = compressor.compressFile(bigSrc, bigArchive);
    ASSERT_EQ(result.status, jackfruit::compression::CompressionStatus::SUCCESS);

    const auto stats = jackfruit::stats::StatisticsEngine::computeStats(bigArchive);
    ASSERT_GE(stats.originalBytes, 1024ULL * 1024ULL);

    std::ostringstream out;
    jackfruit::stats::StatisticsEngine::printStatsTable(stats, bigArchive, out);
    EXPECT_NE(out.str().find("MB"), std::string::npos);
}

// Zero-value stats must render without dividing by zero
TEST(StatisticsEngineStandaloneTest, PrintStatsTableHandlesZeroStats) {
    const jackfruit::stats::ArchiveStats stats{};
    std::ostringstream out;
    jackfruit::stats::StatisticsEngine::printStatsTable(stats, "empty.jack", out);
    EXPECT_NE(out.str().find("0 B"), std::string::npos);
}

// An empty payload archive yields zero original size and no ratio
TEST_F(StatisticsEngineTest, EmptyFileStatsAreZero) {
    const fs::path dir = fs::path("build") / "test_stats_tmp"; // NOSONAR - test-only temp dir
    const std::string emptySrc = (dir / "empty_input.txt").string();
    {
        std::ofstream f(emptySrc, std::ios::binary);
    }
    const std::string emptyArchive = emptySrc + ".jack";

    jackfruit::compression::Compressor compressor;
    const auto result = compressor.compressFile(emptySrc, emptyArchive);
    ASSERT_EQ(result.status, jackfruit::compression::CompressionStatus::SUCCESS);

    const auto stats = jackfruit::stats::StatisticsEngine::computeStats(emptyArchive);
    EXPECT_EQ(stats.originalBytes, 0ULL);
    EXPECT_DOUBLE_EQ(stats.compressionRatio, 0.0);
    EXPECT_DOUBLE_EQ(stats.spaceSavedPercent, 0.0);
    EXPECT_DOUBLE_EQ(stats.bitsPerSymbol, 0.0);
}

