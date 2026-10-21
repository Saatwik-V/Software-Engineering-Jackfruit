#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <utility>

constexpr int ERROR_INVALID_HEADER = 2;
constexpr uint8_t SUPPORTED_ARCHIVE_VERSION = 0x01;

struct ArchiveHeader {
    char magic[4];          // "JACK" (0x4A, 0x41, 0x43, 0x4B)
    uint8_t version;        // 1
    uint8_t flags;          // bitwise flags
    uint64_t originalSize;  // original uncompressed size in bytes
    uint32_t checksumCRC32; // CRC-32 checksum
    uint16_t symbolCount;   // unique symbol count (0 to 256)
    uint64_t totalBits;     // total valid encoded payload bits
};

class ArchiveReader {
public:
    static bool readFixedHeader(const std::string& archivePath, ArchiveHeader& header);
    static bool readHeader(const std::string& archivePath,
                           ArchiveHeader& header,
                           std::vector<std::pair<uint8_t, uint64_t>>& frequencies);

    // readHeader() plus the full structural checks from archive-format.md Section 6
    // (flag consistency, symbol-table consistency, exact file length).
    // Throws ERROR_INVALID_HEADER on any violation. Does not read the payload.
    static void readValidatedHeader(const std::string& archivePath,
                                    ArchiveHeader& header,
                                    std::vector<std::pair<uint8_t, uint64_t>>& frequencies);
};
