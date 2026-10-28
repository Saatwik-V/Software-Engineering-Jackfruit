/**
 * @file menu.cpp
 * @brief Interactive terminal navigation menu shell implementation (JACK-F-001, JACK-NF-005).
 */

#include "cli/menu.hpp"
#include "stats/statistics_engine.hpp"
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "archive/archive.hpp"
#include "compression/compressor.hpp"
#include "decompression/decoder.hpp"
#include "io/file_validator.hpp"
#include "security/integrity_validator.hpp"

namespace jackfruit::cli {

namespace {

std::string trim(const std::string& str) {
    const size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    const size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace

std::string Menu::colorBold() const {
    return useColor_ ? "\033[1m" : "";
}

std::string Menu::colorCyan() const {
    return useColor_ ? "\033[36m" : "";
}

std::string Menu::colorGreen() const {
    return useColor_ ? "\033[32m" : "";
}

std::string Menu::colorYellow() const {
    return useColor_ ? "\033[33m" : "";
}

std::string Menu::colorRed() const {
    return useColor_ ? "\033[31m" : "";
}

std::string Menu::colorReset() const {
    return useColor_ ? "\033[0m" : "";
}

void Menu::displayMenu(std::ostream& out) const {
    out << "\n"
        << colorCyan() << colorBold()
        << "============================================================\n"
        << "         JACKFRUIT — Huffman File Compressor (v0.1.0)       \n"
        << "               Interactive Terminal Menu Shell              \n"
        << "============================================================\n"
        << colorReset()
        << "  " << colorBold() << "[1]" << colorReset() << " Compress File\n"
        << "  " << colorBold() << "[2]" << colorReset() << " Decompress Archive (.jack)\n"
        << "  " << colorBold() << "[3]" << colorReset() << " View Archive Statistics\n"
        << "  " << colorBold() << "[4]" << colorReset() << " Validate Archive Integrity\n"
        << "  " << colorBold() << "[5]" << colorReset() << " Exit\n"
        << colorCyan()
        << "------------------------------------------------------------\n"
        << colorReset();
}

void Menu::run(std::istream& in, std::ostream& out) {
    while (in.good()) {
        displayMenu(out);
        out << colorBold() << "Select an option (1-5): " << colorReset();

        std::string line;
        if (!std::getline(in, line)) {
            break;
        }

        const std::string trimmed = trim(line);
        if (trimmed.empty()) {
            continue;
        }

        int choice = 0;
        try {
            size_t idx = 0;
            choice = std::stoi(trimmed, &idx);
            if (idx != trimmed.length()) {
                out << colorYellow() << "\n[!] Invalid input '" << trimmed
                    << "'. Please enter a number between 1 and 5.\n" << colorReset();
                continue;
            }
        } catch (...) {
            out << colorYellow() << "\n[!] Invalid input '" << trimmed
                << "'. Please enter a number between 1 and 5.\n" << colorReset();
            continue;
        }

        if (choice < 1 || choice > 5) {
            out << colorYellow() << "\n[!] Option " << choice
                << " is out of range. Please choose between 1 and 5.\n" << colorReset();
            continue;
        }

        const bool keepGoing = handleChoice(choice, in, out);
        if (!keepGoing) {
            break;
        }
    }
}

bool Menu::handleChoice(int choice, std::istream& in, std::ostream& out) {
    switch (choice) {
        case 1:
            handleCompress(in, out);
            return true;
        case 2:
            handleDecompress(in, out);
            return true;
        case 3:
            handleStatistics(in, out);
            return true;
        case 4:
            handleValidate(in, out);
            return true;
        case 5:
            out << colorGreen() << "\n[+] Exiting Jackfruit. Goodbye!\n" << colorReset();
            return false;
        default:
            return true;
    }
}

void Menu::handleCompress(std::istream& in, std::ostream& out) {
    out << "\n" << colorCyan() << colorBold()
        << "--- [Option 1] Compress File ---" << colorReset() << "\n";
    out << "Enter path of file to compress: ";

    std::string inputLine;
    if (!std::getline(in, inputLine)) {
        return;
    }
    const std::string inputPath = trim(inputLine);
    if (inputPath.empty()) {
        out << colorRed() << "[-] Error: Input path cannot be empty.\n" << colorReset();
        return;
    }

    const auto valResult = io::FileValidator::validateInputFile(inputPath);
    if (!valResult.isValid) {
        out << colorRed() << "[-] Error: " << valResult.errorMessage << "\n" << colorReset();
        return;
    }

    const std::string defaultOutput = inputPath + ".jack";
    out << "Enter output archive path [default: " << defaultOutput << "]: ";

    std::string outputLine;
    if (!std::getline(in, outputLine)) {
        return;
    }
    std::string outputPath = trim(outputLine);
    if (outputPath.empty()) {
        outputPath = defaultOutput;
    }

    out << "[*] Compressing '" << inputPath << "' -> '" << outputPath << "'...\n";

    compression::Compressor compressor;
    const auto result = compressor.compressFile(inputPath, outputPath);

    if (result.status != compression::CompressionStatus::SUCCESS) {
        out << colorRed() << "[-] Compression failed: " << result.errorMessage << "\n" << colorReset();
        return;
    }

    out << colorGreen() << colorBold() << "[+] Compression successful!\n" << colorReset()
        << "    Original size:    " << result.originalBytes << " bytes\n"
        << "    Archive size:     " << result.compressedBytes << " bytes\n"
        << "    Checksum (CRC32): 0x" << std::hex << std::uppercase << std::setfill('0')
        << std::setw(8) << result.checksumCRC32 << std::dec << "\n"
        << "    Compression ratio:" << std::fixed << std::setprecision(2)
        << " " << result.compressionRatio << "x\n"
        << "    Space saved:      " << result.percentageSaved << "%\n"
        << "    Elapsed time:     " << result.executionTimeMs << " ms\n";
}

void Menu::handleDecompress(std::istream& in, std::ostream& out) {
    out << "\n" << colorCyan() << colorBold()
        << "--- [Option 2] Decompress Archive ---" << colorReset() << "\n";
    out << "Enter path of archive to decompress (.jack): ";

    std::string inputLine;
    if (!std::getline(in, inputLine)) {
        return;
    }
    const std::string archivePath = trim(inputLine);
    if (archivePath.empty()) {
        out << colorRed() << "[-] Error: Archive path cannot be empty.\n" << colorReset();
        return;
    }

    const auto valResult = io::FileValidator::validateInputFile(archivePath);
    if (!valResult.isValid) {
        out << colorRed() << "[-] Error: " << valResult.errorMessage << "\n" << colorReset();
        return;
    }

    std::string defaultOutput = "restored.txt";
    if (archivePath.length() >= 5 && archivePath.substr(archivePath.length() - 5) == ".jack") {
        defaultOutput = archivePath.substr(0, archivePath.length() - 5) + "_restored";
    }

    out << "Enter output restored path [default: " << defaultOutput << "]: ";

    std::string outputLine;
    if (!std::getline(in, outputLine)) {
        return;
    }
    std::string outputPath = trim(outputLine);
    if (outputPath.empty()) {
        outputPath = defaultOutput;
    }

    out << "[*] Decompressing '" << archivePath << "' -> '" << outputPath << "'...\n";

    const bool success = decompression::Decoder::decompressFile(archivePath, outputPath);
    if (!success) {
        out << colorRed() << "[-] Decompression failed. Archive may be corrupted or invalid.\n" << colorReset();
        return;
    }

    out << colorGreen() << colorBold()
        << "[+] Decompression successful! Restored to '" << outputPath << "'.\n" << colorReset();
}

void Menu::handleStatistics(std::istream& in, std::ostream& out) {
    out << "\n" << colorCyan() << colorBold()
        << "--- [Option 3] View Archive Statistics ---" << colorReset() << "\n";
    out << "Enter path of archive (.jack): ";

    std::string inputLine;
    if (!std::getline(in, inputLine)) {
        return;
    }
    const std::string archivePath = trim(inputLine);
    if (archivePath.empty()) {
        out << colorRed() << "[-] Error: Archive path cannot be empty.\n" << colorReset();
        return;
    }

    const auto valResult = io::FileValidator::validateInputFile(archivePath);
    if (!valResult.isValid) {
        out << colorRed() << "[-] Error: " << valResult.errorMessage << "\n" << colorReset();
        return;
    }

    try {
        const auto stats = stats::StatisticsEngine::computeStats(archivePath);
        stats::StatisticsEngine::printStatsTable(stats, archivePath);
    } catch (int errCode) {
        out << colorRed() << "[-] Failed: invalid or corrupted .jack archive (error "
            << errCode << ").\n" << colorReset();
    }
}

void Menu::handleValidate(std::istream& in, std::ostream& out) {
    out << "\n" << colorCyan() << colorBold()
        << "--- [Option 4] Validate Archive Integrity ---" << colorReset() << "\n";
    out << "Enter path of archive to validate (.jack): ";

    std::string inputLine;
    if (!std::getline(in, inputLine)) {
        return;
    }
    const std::string archivePath = trim(inputLine);
    if (archivePath.empty()) {
        out << colorRed() << "[-] Error: Archive path cannot be empty.\n" << colorReset();
        return;
    }

    const auto valResult = io::FileValidator::validateInputFile(archivePath);
    if (!valResult.isValid) {
        out << colorRed() << "[-] Error: " << valResult.errorMessage << "\n" << colorReset();
        return;
    }

    const auto result = security::IntegrityValidator::validateArchive(archivePath);

    if (result.status != security::ValidationStatus::VALID) {
        out << colorRed() << colorBold()
            << "[-] Integrity Check: FAILED (" << result.message << ")\n" << colorReset()
            << "    Exit Code:        " << result.exitCode << "\n"
            << "    Stored CRC-32:    0x" << std::hex << std::uppercase << std::setfill('0')
            << std::setw(8) << result.expectedChecksum << std::dec << "\n"
            << "    Computed CRC-32:  0x" << std::hex << std::uppercase << std::setfill('0')
            << std::setw(8) << result.computedChecksum << std::dec << "\n"
            << "    Payload Decoded:  " << result.payloadBytes << " bytes\n";
        return;
    }

    out << colorGreen() << colorBold()
        << "[+] Archive Header Integrity: VALID\n" << colorReset()
        << "    Magic Identifier:     JACK\n"
        << "    Format Version:       v" << static_cast<int>(result.version) << "\n"
        << "    Original Payload Size:" << result.payloadBytes << " bytes\n"
        << "    Unique Symbol Count:  " << result.symbolCount << "\n"
        << "    Stored CRC-32:        0x" << std::hex << std::uppercase << std::setfill('0')
        << std::setw(8) << result.expectedChecksum << std::dec << "\n"
        << "    Computed CRC-32:      0x" << std::hex << std::uppercase << std::setfill('0')
        << std::setw(8) << result.computedChecksum << std::dec << "\n"
        << colorGreen() << colorBold()
        << "[+] Result: Archive integrity verified successfully (PASS).\n" << colorReset();
}

} // namespace jackfruit::cli