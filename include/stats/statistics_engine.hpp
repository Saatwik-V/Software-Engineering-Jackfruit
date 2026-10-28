/**
 * @file statistics_engine.hpp
 * @brief Archive statistics inspection engine (UC-03, JACK-F-009..012).
 *
 * Reads .jack archive headers in <5 ms without full decompression and
 * computes compression ratio, space savings, and bit efficiency metrics.
 */

#pragma once

#include <cstdint>
#include <string>

namespace jackfruit::stats {

struct ArchiveStats {
    uint64_t originalBytes{0};
    uint64_t compressedBytes{0};
    uint64_t totalBits{0};
    uint32_t checksumCRC32{0};
    uint16_t symbolCount{0};
    double   compressionRatio{0.0};
    double   spaceSavedPercent{0.0};
    double   bitsPerSymbol{0.0};
};

class StatisticsEngine {
public:
    /**
     * @brief Reads archive header (no payload decompression) and computes stats.
     * @param archivePath Path to the .jack archive.
     * @return Populated ArchiveStats struct.
     * @throws ERROR_INVALID_HEADER (int 2) on malformed archive.
     */
    static ArchiveStats computeStats(const std::string& archivePath);

    /**
     * @brief Prints a formatted ASCII statistics table to stdout.
     * @param stats   Computed stats.
     * @param archivePath Path string shown in the table header.
     */
    static void printStatsTable(const ArchiveStats& stats,
                                const std::string& archivePath);
};

} // namespace jackfruit::stats