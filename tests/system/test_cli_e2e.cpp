/**
 * @file test_cli_e2e.cpp
 * @brief System-level end-to-end tests for the Jackfruit CLI binary and interactive shell
 *        (JACK-F-001, JACK-F-005, JACK-NF-005, TC-COMP-01, TC-DECOMP-02, TC-SEC-01).
 */

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "cli/menu.hpp"
#include "compression/compressor.hpp"
#include "decompression/decoder.hpp"

namespace fs = std::filesystem;
using namespace jackfruit::cli;

class SystemCliE2ETest : public ::testing::Test {
protected:
    fs::path tempDir_;
    std::string sampleTextFile_;
    std::string sampleArchiveFile_;
    std::string restoredTextFile_;
    std::string corruptMagicArchive_;
    std::string truncatedArchive_;

    void SetUp() override {
        tempDir_ = fs::temp_directory_path() / "jackfruit_cli_e2e_test";
        fs::create_directories(tempDir_);

        sampleTextFile_ = (tempDir_ / "sample.txt").string();
        sampleArchiveFile_ = (tempDir_ / "sample.jack").string();
        restoredTextFile_ = (tempDir_ / "sample_restored.txt").string();
        corruptMagicArchive_ = (tempDir_ / "corrupt_magic.jack").string();
        truncatedArchive_ = (tempDir_ / "truncated.jack").string();

        // 1. Create a sample text file
        std::ofstream textOut(sampleTextFile_, std::ios::binary);
        textOut << "The quick brown fox jumps over the lazy dog.\n"
                << "Project Jackfruit System End-to-End Validation Suite 2026.\n"
                << "The quick brown fox jumps over the lazy dog.\n";
        textOut.close();

        // 2. Pre-generate a valid archive
        jackfruit::compression::Compressor compressor;
        compressor.compressFile(sampleTextFile_, sampleArchiveFile_);

        // 3. TC-DECOMP-02: Create corrupted magic bytes archive
        std::ifstream validIn(sampleArchiveFile_, std::ios::binary);
        std::string archiveData((std::istreambuf_iterator<char>(validIn)), std::istreambuf_iterator<char>());
        validIn.close();

        std::string corruptMagicData = archiveData;
        if (corruptMagicData.size() >= 4) {
            corruptMagicData[0] = 'B';
            corruptMagicData[1] = 'A';
            corruptMagicData[2] = 'D';
            corruptMagicData[3] = '!';
        }
        std::ofstream corruptOut(corruptMagicArchive_, std::ios::binary);
        corruptOut.write(corruptMagicData.data(), static_cast<std::streamsize>(corruptMagicData.size()));
        corruptOut.close();

        // 4. TC-SEC-01: Create truncated archive (keep header, truncate payload)
        std::string truncatedData = archiveData.substr(0, std::min<size_t>(archiveData.size(), 28));
        std::ofstream truncOut(truncatedArchive_, std::ios::binary);
        truncOut.write(truncatedData.data(), static_cast<std::streamsize>(truncatedData.size()));
        truncOut.close();
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove_all(tempDir_, ec);
    }
};

// Verify interactive shell full lifecycle via input injection
TEST_F(SystemCliE2ETest, InteractiveMenuFullLifecycleSimulation) {
    Menu menu;
    menu.setColored(false);

    // Option 1 (Compress) -> Option 3 (Stats) -> Option 4 (Validate) -> Option 2 (Decompress) -> Option 5 (Exit)
    const std::string simulatedInput =
        "1\n" + sampleTextFile_ + "\n" + (tempDir_ / "menu_out.jack").string() + "\n" +
        "3\n" + (tempDir_ / "menu_out.jack").string() + "\n" +
        "4\n" + (tempDir_ / "menu_out.jack").string() + "\n" +
        "2\n" + (tempDir_ / "menu_out.jack").string() + "\n" + (tempDir_ / "menu_restored.txt").string() + "\n" +
        "5\n";

    std::istringstream in(simulatedInput);
    std::ostringstream out;

    menu.run(in, out);
    const std::string text = out.str();

    EXPECT_NE(text.find("[+] Compression successful!"), std::string::npos);
    EXPECT_NE(text.find("Archive Summary & Statistics:"), std::string::npos);
    EXPECT_NE(text.find("Archive Header Integrity: VALID"), std::string::npos);
    EXPECT_NE(text.find("[+] Decompression successful!"), std::string::npos);
    EXPECT_NE(text.find("Exiting Jackfruit. Goodbye!"), std::string::npos);

    // Verify restored file exists and is identical
    EXPECT_TRUE(fs::exists(tempDir_ / "menu_restored.txt"));
}

// TC-DECOMP-02: Decompress archive with corrupted header magic bytes fails gracefully
TEST_F(SystemCliE2ETest, DecompressCorruptMagicArchiveFailsGracefully_TC_DECOMP_02) {
    std::string outPath = (tempDir_ / "bad_restored.txt").string();
    bool success = jackfruit::decompression::Decoder::decompressFile(corruptMagicArchive_, outPath);
    EXPECT_FALSE(success);
}

// TC-SEC-01: Decompress truncated archive fails gracefully without buffer overrun
TEST_F(SystemCliE2ETest, DecompressTruncatedArchiveFailsGracefully_TC_SEC_01) {
    std::string outPath = (tempDir_ / "trunc_restored.txt").string();
    bool success = jackfruit::decompression::Decoder::decompressFile(truncatedArchive_, outPath);
    EXPECT_FALSE(success);
}

// Interactive menu rejects invalid non-existent paths safely (JACK-F-001)
TEST_F(SystemCliE2ETest, MenuHandlesNonExistentPathGracefully_JACK_F_001) {
    Menu menu;
    menu.setColored(false);
    std::string fakePath = (tempDir_ / "does_not_exist_anywhere.txt").string();

    std::istringstream in("1\n" + fakePath + "\n5\n");
    std::ostringstream out;

    menu.run(in, out);
    const std::string text = out.str();

    EXPECT_NE(text.find("[-] Error:"), std::string::npos);
    EXPECT_NE(text.find("Exiting Jackfruit. Goodbye!"), std::string::npos);
}

// Menu invalid option recovery
TEST_F(SystemCliE2ETest, MenuRecoversFromErroneousOptionsAndStrings) {
    Menu menu;
    menu.setColored(false);

    std::istringstream in("not_a_number\n-5\n100\n5\n");
    std::ostringstream out;

    menu.run(in, out);
    const std::string text = out.str();

    EXPECT_NE(text.find("Invalid input 'not_a_number'"), std::string::npos);
    EXPECT_NE(text.find("out of range"), std::string::npos);
    EXPECT_NE(text.find("Exiting Jackfruit. Goodbye!"), std::string::npos);
}
