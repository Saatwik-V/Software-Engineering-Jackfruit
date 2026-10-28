#include "stats/statistics_engine.hpp"
#include "archive/archive.hpp"

#include <filesystem>
#include <iomanip>
#include <ostream>
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
        oss << std::fixed << std::setprecision(2);
        oss << (static_cast<double>(b) / (1024.0 * 1024.0)) << " MB  (" << b << " B)";
    }
    return oss.str();
}

static std::string fmtDecimal(double value, int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}

static std::string fmtCrc32(uint32_t crc) {
    std::ostringstream oss;
    oss << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << crc;
    return oss.str();
}

void StatisticsEngine::printStatsTable(const ArchiveStats& stats,
                                       const std::string& archivePath,
                                       std::ostream& out) {
    constexpr int W = 30;
    const std::string SEP(52, '=');

    const auto row = [&out](const std::string& label, const std::string& value) {
        out << "  " << std::left << std::setw(W) << label << value << "\n";
    };

    out << "\n" << SEP << "\n";
    out << "  Archive Statistics\n";
    out << "  File: " << archivePath << "\n";
    out << SEP << "\n";

    row("Original Size", fmtBytes(stats.originalBytes));
    row("Compressed Size", fmtBytes(stats.compressedBytes));
    row("Compression Ratio", fmtDecimal(stats.compressionRatio, 2) + "x");
    row("Space Saved", fmtDecimal(stats.spaceSavedPercent, 1) + "%");
    row("Unique Symbols", std::to_string(stats.symbolCount));
    row("Total Encoded Bits", std::to_string(stats.totalBits));
    row("Avg Bits / Symbol", fmtDecimal(stats.bitsPerSymbol, 3));
    row("CRC-32 Checksum", fmtCrc32(stats.checksumCRC32));

    out << SEP << "\n\n";
}

} // namespace jackfruit::stats