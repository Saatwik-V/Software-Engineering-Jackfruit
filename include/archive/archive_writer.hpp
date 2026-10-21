#pragma once

// ArchiveWriter: serializes a .jack archive (docs/design/archive-format.md).
//
// Layout (all multi-byte integers Big-Endian):
//   Fixed header (18 B): magic "JACK", version, flags, original size (8), CRC-32 (4)
//   Section B: symbol_count (2) + N * [symbol (1) + frequency (8)]
//   Section C: total_bits (8) + packed bitstream (ceil(total_bits / 8) bytes)

#include <cstdint>
#include <string>
#include <vector>

#include "archive/archive.hpp"
#include "archive/archive_validation.hpp"

class ArchiveWriter {
public:
    // Builds the complete archive in memory.
    //
    // The writer fills in magic ("JACK"), format version and symbolCount itself, so the
    // caller only needs to set flags, originalSize, checksumCRC32 and totalBits.
    //
    // Throws std::invalid_argument (with a readable message) if the data would not form
    // a valid archive, e.g. unsorted/duplicate symbols, frequencies not summing to
    // originalSize, or a payload whose length does not match totalBits.
    static std::vector<uint8_t> serialize(const ArchiveHeader& header,
                                          const FrequencyList& frequencies,
                                          const std::vector<uint8_t>& payload);

    // serialize() + write to disk. Throws std::runtime_error if the file cannot be written.
    static void write(const std::string& archivePath,
                      const ArchiveHeader& header,
                      const FrequencyList& frequencies,
                      const std::vector<uint8_t>& payload);
};
