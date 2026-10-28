/**
 * @file main.cpp
 * @brief Entry point for the Jackfruit Huffman file compressor CLI.
 */

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "cli/menu.hpp"
#include "compression/compressor.hpp"
#include "decompression/decoder.hpp"
#include "security/integrity_validator.hpp"
#include "stats/statistics_engine.hpp" 

namespace {

void printUsage(const char* progName) {
    std::cout << "Jackfruit — Lossless Huffman File Compressor (v0.1.0)\n\n"
              << "Usage:\n"
              << "  " << progName << "                            Launch interactive terminal menu\n"
              << "  " << progName << " --menu | -m                Launch interactive terminal menu\n"
              << "  " << progName << " compress <in> [-o <out>]    Compress an input file into .jack archive\n"
              << "  " << progName << " decompress <in> [-o <out>]  Decompress a .jack archive\n"
              << "  " << progName << " validate <archive.jack>     Validate archive integrity and CRC-32 checksum\n"
              << "  " << progName << " --help | -h                Display this help message\n\n"
              << "Commands:\n"
              << "  compress    Compress an input file into a .jack binary archive\n"
              << "  decompress  Decompress a .jack binary archive back to original file\n"
              << "  validate    Validate structural integrity and verify IEEE 802.3 CRC-32\n"
              << "  menu        Launch the interactive numbered CLI navigation menu\n"
              << "  --help, -h  Display this help message\n";
}

int handleCompress(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Error: 'compress' requires an input file path.\n";
        printUsage(argv[0]);
        return 1;
    }

    std::string inputPath = argv[2];
    std::string outputPath = inputPath + ".jack";

    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputPath = argv[++i];
            } else {
                std::cerr << "Error: -o option requires an output path argument.\n";
                return 1;
            }
        } else {
            std::cerr << "Error: Unknown argument '" << arg << "'.\n";
            return 1;
        }
    }

    std::cout << "[*] Compressing '" << inputPath << "' -> '" << outputPath << "'...\n";

    jackfruit::compression::Compressor compressor;
    const auto result = compressor.compressFile(inputPath, outputPath);

    if (result.status != jackfruit::compression::CompressionStatus::SUCCESS) {
        std::cerr << "[-] Compression failed: " << result.errorMessage << "\n";
        return 1;
    }

    std::cout << "[+] Compression successful!\n"
              << "    Original size:    " << result.originalBytes << " bytes\n"
              << "    Archive size:     " << result.compressedBytes << " bytes\n"
              << "    Checksum (CRC32): 0x" << std::hex << std::uppercase << std::setfill('0')
              << std::setw(8) << result.checksumCRC32 << std::dec << "\n"
              << "    Compression ratio:" << std::fixed << std::setprecision(2)
              << " " << result.compressionRatio << "x\n"
              << "    Space saved:      " << result.percentageSaved << "%\n"
              << "    Elapsed time:     " << result.executionTimeMs << " ms\n";

    return 0;
}

int handleDecompress(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Error: 'decompress' requires an archive file path.\n";
        printUsage(argv[0]);
        return 1;
    }

    std::string archivePath = argv[2];
    std::string outputPath = "restored.txt"; // Default fallback if -o not provided

    // If input ends with .jack, default output strips it or names it restored_ + filename
    if (archivePath.length() >= 5 && archivePath.substr(archivePath.length() - 5) == ".jack") {
        outputPath = archivePath.substr(0, archivePath.length() - 5) + "_restored";
    }

    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputPath = argv[++i];
            } else {
                std::cerr << "Error: -o option requires an output path argument.\n";
                return 1;
            }
        } else {
            std::cerr << "Error: Unknown argument '" << arg << "'.\n";
            return 1;
        }
    }

    std::cout << "[*] Decompressing '" << archivePath << "' -> '" << outputPath << "'...\n";

    bool success = jackfruit::decompression::Decoder::decompressFile(archivePath, outputPath);

    if (!success) {
        std::cerr << "[-] Decompression failed.\n";
        return 1;
    }

    std::cout << "[+] Decompression successful! Restored to '" << outputPath << "'.\n";
    return 0;
}

int handleValidate(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Error: 'validate' requires an archive file path.\n";
        printUsage(argv[0]);
        return 1;
    }

    const std::string archivePath = argv[2];
    std::cout << "[*] Validating archive '" << archivePath << "'...\n";

    const auto result = jackfruit::security::IntegrityValidator::validateArchive(archivePath);
    std::cout << jackfruit::security::IntegrityValidator::formatValidationReport(result, true);

    return result.exitCode;
}

int handleStat(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Error: 'stat' requires an archive file path.\n";
        printUsage(argv[0]);
        return 1;
    }

    const std::string archivePath = argv[2];
    std::cout << "[*] Reading statistics for '" << archivePath << "'...\n";

    try {
        const auto stats = jackfruit::stats::StatisticsEngine::computeStats(archivePath);
        jackfruit::stats::StatisticsEngine::printStatsTable(stats, archivePath, std::cout);
    } catch (int errCode) {
        std::cerr << "[-] Failed: invalid or corrupted .jack archive (error "
                  << errCode << ").\n";
        return errCode;
    }

    return 0;
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        jackfruit::cli::Menu menu;
        menu.run(std::cin, std::cout);
        return 0;
    }

    const std::string command = argv[1];

    if (command == "-h" || command == "--help") {
        printUsage(argv[0]);
        return 0;
    }

    if (command == "-m" || command == "--menu" || command == "menu") {
        jackfruit::cli::Menu menu;
        menu.run(std::cin, std::cout);
        return 0;
    }

    if (command == "compress") {
        return handleCompress(argc, argv);
    }

    if (command == "decompress") {
        return handleDecompress(argc, argv);
    }

    if (command == "validate") {
        return handleValidate(argc, argv);
    }

    if (command == "stat") {
        return handleStat(argc, argv);
    }

    std::cerr << "Error: Unknown command '" << command << "'.\n";
    printUsage(argv[0]);
    return 1;
}

