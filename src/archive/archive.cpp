#include "archive/archive.hpp"
#include <fstream>
#include <cstring>
#include <stdexcept>

bool ArchiveReader::readFixedHeader(const std::string& archivePath, ArchiveHeader& header) {
    std::ifstream in(archivePath, std::ios::binary);
    if (!in.is_open()) {
        throw ERROR_INVALID_HEADER;
    }

    // Read fixed 18-byte segment:
    // 4 bytes magic + 1 byte version + 1 byte flags + 8 bytes originalSize + 4 bytes checksumCRC32
    char buffer[18];
    in.read(buffer, 18);
    if (in.gcount() < 18) {
        throw ERROR_INVALID_HEADER;
    }

    std::memcpy(header.magic, buffer, 4);
    header.version = static_cast<uint8_t>(buffer[4]);
    header.flags = static_cast<uint8_t>(buffer[5]);

    // Big-Endian deserialization for multi-byte fields
    header.originalSize = 0;
    for (int i = 0; i < 8; ++i) {
        header.originalSize = (header.originalSize << 8) | static_cast<uint8_t>(buffer[6 + i]);
    }

    header.checksumCRC32 = 0;
    for (int i = 0; i < 4; ++i) {
        header.checksumCRC32 = (header.checksumCRC32 << 8) | static_cast<uint8_t>(buffer[14 + i]);
    }

    // Validate Magic Bytes: "JACK" (0x4A, 0x41, 0x43, 0x4B)
    if (std::memcmp(header.magic, "JACK", 4) != 0) {
        throw ERROR_INVALID_HEADER;
    }

    // Validate Format Version: 0x01
    if (header.version != SUPPORTED_ARCHIVE_VERSION) {
        throw ERROR_INVALID_HEADER;
    }

    return true;
}

bool ArchiveReader::readHeader(const std::string& archivePath,
                               ArchiveHeader& header,
                               std::vector<std::pair<uint8_t, uint64_t>>& frequencies) {
    if (!readFixedHeader(archivePath, header)) {
        return false;
    }

    std::ifstream in(archivePath, std::ios::binary);
    if (!in.is_open()) {
        throw ERROR_INVALID_HEADER;
    }

    // Skip the 18-byte fixed header
    in.seekg(18, std::ios::beg);

    // Read symbolCount (2 bytes, big-endian)
    uint8_t countBytes[2];
    in.read(reinterpret_cast<char*>(countBytes), 2);
    if (in.gcount() < 2) {
        throw ERROR_INVALID_HEADER;
    }
    header.symbolCount = (static_cast<uint16_t>(countBytes[0]) << 8) | static_cast<uint16_t>(countBytes[1]);

    if (header.symbolCount > 256) {
        throw ERROR_INVALID_HEADER;
    }

    frequencies.clear();
    frequencies.reserve(header.symbolCount);

    // Read frequency table entries: each entry is 1 byte symbol + 8 bytes count (9 bytes each)
    for (uint16_t i = 0; i < header.symbolCount; ++i) {
        char entry[9];
        in.read(entry, 9);
        if (in.gcount() < 9) {
            throw ERROR_INVALID_HEADER;
        }

        uint8_t symbol = static_cast<uint8_t>(entry[0]);
        uint64_t freq = 0;
        for (int b = 0; b < 8; ++b) {
            freq = (freq << 8) | static_cast<uint8_t>(entry[1 + b]);
        }
        frequencies.emplace_back(symbol, freq);
    }

    // Read totalBits (8 bytes, big-endian)
    char bitsBuf[8];
    in.read(bitsBuf, 8);
    if (in.gcount() < 8) {
        throw ERROR_INVALID_HEADER;
    }
    header.totalBits = 0;
    for (int b = 0; b < 8; ++b) {
        header.totalBits = (header.totalBits << 8) | static_cast<uint8_t>(bitsBuf[b]);
    }

    return true;
}
