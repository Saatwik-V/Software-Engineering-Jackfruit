#include <gtest/gtest.h>
#include "stats/statistics_engine.hpp"
#include "compression/compressor.hpp"

#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

// Helper: write a temp text file
static std::string writeTempFile(const std::string& name, const std::string& content) {
    const fs::path dir = fs::path("build") / "test_stats_tmp";
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

// printStatsTable smoke test — must not crash
TEST_F(StatisticsEngineTest, PrintStatsTableDoesNotCrash) {
    const auto stats = jackfruit::stats::StatisticsEngine::computeStats(archivePath);
    EXPECT_NO_THROW(
        jackfruit::stats::StatisticsEngine::printStatsTable(stats, archivePath)
    );
}

