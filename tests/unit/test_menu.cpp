/**
 * @file test_menu.cpp
 * @brief Comprehensive unit tests for interactive CLI navigation menu shell (JACK-F-001, JACK-NF-005).
 */

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "cli/menu.hpp"
#include "compression/compressor.hpp"

namespace fs = std::filesystem;
using namespace jackfruit::cli;

class MenuTest : public ::testing::Test {
protected:
    fs::path tempDir_;
    std::string sampleFile_;
    std::string sampleArchive_;
    std::string restoredFile_;
    std::string corruptArchive_;

    void SetUp() override {
        tempDir_ = fs::temp_directory_path() / "jackfruit_menu_test";
        fs::create_directories(tempDir_);

        sampleFile_ = (tempDir_ / "sample.txt").string();
        sampleArchive_ = (tempDir_ / "sample.jack").string();
        restoredFile_ = (tempDir_ / "sample_restored.txt").string();
        corruptArchive_ = (tempDir_ / "corrupt.jack").string();

        // Create sample file with repeatable text
        std::ofstream out(sampleFile_, std::ios::binary);
        out << "Jackfruit Huffman Compression Interactive Menu Test Suite 2026.\n"
            << "Repeated payload line for compression testing.\n"
            << "Repeated payload line for compression testing.\n";
        out.close();

        // Compress sampleFile into sampleArchive for testing options 2, 3, 4
        jackfruit::compression::Compressor compressor;
        compressor.compressFile(sampleFile_, sampleArchive_);

        // Create a corrupt archive file
        std::ofstream corruptOut(corruptArchive_, std::ios::binary);
        corruptOut << "NOT_A_VALID_JACK_ARCHIVE_HEADER_CORRUPT";
        corruptOut.close();
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove_all(tempDir_, ec);
    }
};

TEST_F(MenuTest, DisplayMenuContainsAllExpectedOptions) {
    Menu menu;
    menu.setColored(false);
    std::ostringstream out;

    menu.displayMenu(out);
    const std::string text = out.str();

    EXPECT_NE(text.find("JACKFRUIT"), std::string::npos);
    EXPECT_NE(text.find("[1] Compress File"), std::string::npos);
    EXPECT_NE(text.find("[2] Decompress Archive (.jack)"), std::string::npos);
    EXPECT_NE(text.find("[3] View Archive Statistics"), std::string::npos);
    EXPECT_NE(text.find("[4] Validate Archive Integrity"), std::string::npos);
    EXPECT_NE(text.find("[5] Exit"), std::string::npos);
}

TEST_F(MenuTest, Option5ExitsGracefully) {
    Menu menu;
    menu.setColored(false);
    std::istringstream in("5\n");
    std::ostringstream out;

    menu.run(in, out);
    const std::string output = out.str();

    EXPECT_NE(output.find("Exiting Jackfruit. Goodbye!"), std::string::npos);
}

TEST_F(MenuTest, InvalidNumericAndStringChoicesRecoverGracefully) {
    Menu menu;
    menu.setColored(false);
    // Send string, out-of-range negative, out-of-range high, then exit 5
    std::istringstream in("invalid_text\n-1\n99\n5\n");
    std::ostringstream out;

    menu.run(in, out);
    const std::string output = out.str();

    EXPECT_NE(output.find("Invalid input 'invalid_text'"), std::string::npos);
    EXPECT_NE(output.find("out of range"), std::string::npos);
    EXPECT_NE(output.find("Exiting Jackfruit. Goodbye!"), std::string::npos);
}

TEST_F(MenuTest, CompressNonExistentFileReturnsSafeError_JACK_F_001) {
    Menu menu;
    menu.setColored(false);
    const std::string missingPath = (tempDir_ / "missing_file.txt").string();
    std::istringstream in("1\n" + missingPath + "\n5\n");
    std::ostringstream out;

    menu.run(in, out);
    const std::string output = out.str();

    EXPECT_NE(output.find("[-] Error:"), std::string::npos);
    EXPECT_NE(output.find("Exiting Jackfruit. Goodbye!"), std::string::npos);
}

TEST_F(MenuTest, CompressValidFileGeneratesArchive) {
    Menu menu;
    menu.setColored(false);
    const std::string customArchive = (tempDir_ / "custom.jack").string();
    std::istringstream in("1\n" + sampleFile_ + "\n" + customArchive + "\n5\n");
    std::ostringstream out;

    menu.run(in, out);
    const std::string output = out.str();

    EXPECT_NE(output.find("[+] Compression successful!"), std::string::npos);
    EXPECT_NE(output.find("Original size:"), std::string::npos);
    EXPECT_NE(output.find("Archive size:"), std::string::npos);
    EXPECT_TRUE(fs::exists(customArchive));
    EXPECT_GT(fs::file_size(customArchive), 0u);
}

TEST_F(MenuTest, DecompressValidArchiveRestoresOriginalData) {
    Menu menu;
    menu.setColored(false);
    std::istringstream in("2\n" + sampleArchive_ + "\n" + restoredFile_ + "\n5\n");
    std::ostringstream out;

    menu.run(in, out);
    const std::string output = out.str();

    EXPECT_NE(output.find("[+] Decompression successful!"), std::string::npos);
    EXPECT_TRUE(fs::exists(restoredFile_));

    // Verify restored file content matches original
    std::ifstream origStream(sampleFile_, std::ios::binary);
    std::string origContent((std::istreambuf_iterator<char>(origStream)),
                             std::istreambuf_iterator<char>());
    origStream.close();

    std::ifstream restStream(restoredFile_, std::ios::binary);
    std::string restContent((std::istreambuf_iterator<char>(restStream)),
                             std::istreambuf_iterator<char>());
    restStream.close();

    EXPECT_EQ(origContent, restContent);
}

TEST_F(MenuTest, DecompressMissingFileReturnsError) {
    Menu menu;
    menu.setColored(false);
    const std::string missingArchive = (tempDir_ / "nonexistent.jack").string();
    std::istringstream in("2\n" + missingArchive + "\n5\n");
    std::ostringstream out;

    menu.run(in, out);
    const std::string output = out.str();

    EXPECT_NE(output.find("[-] Error:"), std::string::npos);
}

TEST_F(MenuTest, StatisticsValidArchiveDisplaysMetricsTable) {
    Menu menu;
    menu.setColored(false);
    std::istringstream in("3\n" + sampleArchive_ + "\n5\n");
    std::ostringstream out;

    menu.run(in, out);
    const std::string output = out.str();

    EXPECT_NE(output.find("Archive Statistics"), std::string::npos);
    EXPECT_NE(output.find("Original Size"), std::string::npos);
    EXPECT_NE(output.find("Compression Ratio"), std::string::npos);
    EXPECT_NE(output.find("CRC-32 Checksum"), std::string::npos);
}

TEST_F(MenuTest, StatisticsMissingArchiveReturnsError) {
    Menu menu;
    menu.setColored(false);
    const std::string missingArchive = (tempDir_ / "nonexistent.jack").string();
    std::istringstream in("3\n" + missingArchive + "\n5\n");
    std::ostringstream out;

    menu.run(in, out);
    const std::string output = out.str();

    EXPECT_NE(output.find("[-] Error:"), std::string::npos);
}

TEST_F(MenuTest, ValidateValidArchivePasses) {
    Menu menu;
    menu.setColored(false);
    std::istringstream in("4\n" + sampleArchive_ + "\n5\n");
    std::ostringstream out;

    menu.run(in, out);
    const std::string output = out.str();

    EXPECT_NE(output.find("[+] Archive Header Integrity: VALID"), std::string::npos);
    EXPECT_NE(output.find("Result: Archive integrity verified successfully (PASS)."), std::string::npos);
}

TEST_F(MenuTest, ValidateCorruptedArchiveReportsFailure) {
    Menu menu;
    menu.setColored(false);
    std::istringstream in("4\n" + corruptArchive_ + "\n5\n");
    std::ostringstream out;

    menu.run(in, out);
    const std::string output = out.str();

    EXPECT_NE(output.find("[-] Integrity Check: FAILED"), std::string::npos);
}

TEST_F(MenuTest, StreamEOFTerminatesCleanlyWithoutHanging) {
    Menu menu;
    menu.setColored(false);
    std::istringstream emptyStream("");
    std::ostringstream out;

    menu.run(emptyStream, out);
    // Must return cleanly and not hang
    SUCCEED();
}
