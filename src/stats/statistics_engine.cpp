#include "stats/statistics_engine.hpp"
#include "archive/archive.hpp"

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace jackfruit::stats {

ArchiveStats StatisticsEngine::computeStats(const std::string& archivePath) {
    ArchiveHeader header{};
    std::vector<std::pair<uint8_t, uint64_t>> frequencies;

    // Reads and validates header without decompressing the payload
    ArchiveReader::readValidatedHeader(archivePath, header, frequencies);

    std::error_code ec;
    const uint64_t fileSize = std::filesystem::file_size(archivePath, ec);

    ArchiveStats stats{};
    stats.originalBytes   = header.originalSize;
    stats.compressedBytes = ec ? 0ULL : fileSize;
    stats.totalBits       = header.totalBits;
    stats.checksumCRC32   = header.checksumCRC32;
    stats.symbolCount     = header.symbolCount;

    if (stats.originalBytes > 0 && stats.compressedBytes > 0) {
        stats.compressionRatio  = static_cast<double>(stats.originalBytes)
                                / static_cast<double>(stats.compressedBytes);
        stats.spaceSavedPercent = (1.0 - static_cast<double>(stats.compressedBytes)
                                       / static_cast<double>(stats.originalBytes)) * 100.0;
    }

    if (stats.originalBytes > 0) {
        stats.bitsPerSymbol = static_cast<double>(stats.totalBits)
                            / static_cast<double>(stats.originalBytes);
    }

    return stats;
}

static std::string fmtBytes(uint64_t b) {
    std::ostringstream oss;
    if (b < 1024ULL) {
        oss << b << " B";
    } else if (b < 1024ULL * 1024ULL) {
        oss << (b / 1024ULL) << " KB  (" << b << " B)";
    } else {
        oss << std::fixed << std::setprecision(2)
            << (static_cast<double>(b) / (1024.0 * 1024.0))
            << " MB  (" << b << " B)";
    }
    return oss.str();
}

void StatisticsEngine::printStatsTable(const ArchiveStats& stats,
                                       const std::string& archivePath) {
    constexpr int W = 30;
    const std::string SEP(52, '=');

    std::cout << "\n" << SEP << "\n";
    std::cout << "  Archive Statistics\n";
    std::cout << "  File: " << archivePath << "\n";
    std::cout << SEP << "\n";

    std::cout << std::left
        << "  " << std::setw(W) << "Original Size"
                << fmtBytes(stats.originalBytes) << "\n"
        << "  " << std::setw(W) << "Compressed Size"
                << fmtBytes(stats.compressedBytes) << "\n"
        << "  " << std::setw(W) << "Compression Ratio"
                << std::fixed << std::setprecision(2)
                << stats.compressionRatio << "x\n"
        << "  " << std::setw(W) << "Space Saved"
                << std::fixed << std::setprecision(1)
                << stats.spaceSavedPercent << "%\n"
        << "  " << std::setw(W) << "Unique Symbols"
                << stats.symbolCount << "\n"
        << "  " << std::setw(W) << "Total Encoded Bits"
                << stats.totalBits << "\n"
        << "  " << std::setw(W) << "Avg Bits / Symbol"
                << std::fixed << std::setprecision(3)
                << stats.bitsPerSymbol << "\n"
        << "  " << std::setw(W) << "CRC-32 Checksum"
                << "0x" << std::hex << std::uppercase << std::setfill('0')
                << std::setw(8) << stats.checksumCRC32 << std::dec << "\n";

    std::cout << SEP << "\n\n";
}

} // namespace jackfruit::stats