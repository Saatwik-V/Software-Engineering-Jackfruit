#pragma once

// Structural validation of a .jack archive (docs/design/archive-format.md, Section 6).
//
// One shared implementation is used by both ArchiveWriter (so the writer can never
// emit an archive the reader would reject) and ArchiveReader::readValidatedHeader.

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "archive/archive.hpp"

using FrequencyList = std::vector<std::pair<uint8_t, uint64_t>>;

// Header flag bits (byte at offset 0x05).
constexpr uint8_t ARCHIVE_FLAG_IS_EMPTY = 0x01;         // original file was 0 bytes
constexpr uint8_t ARCHIVE_FLAG_IS_UNCOMPRESSED = 0x02;  // payload holds the raw bytes
constexpr uint8_t ARCHIVE_FLAG_RESERVED_MASK = 0xFC;    // bits 2-7 must be 0

// Layout sizes.
constexpr uint64_t ARCHIVE_FIXED_HEADER_SIZE = 18;  // magic+version+flags+size+crc
constexpr uint64_t ARCHIVE_SYMBOL_COUNT_SIZE = 2;
constexpr uint64_t ARCHIVE_SYMBOL_ENTRY_SIZE = 9;   // 1B symbol + 8B frequency
constexpr uint64_t ARCHIVE_TOTAL_BITS_SIZE = 8;
constexpr uint16_t ARCHIVE_MAX_SYMBOLS = 256;

// Exact on-disk size of an archive: 18 + 2 + (N * 9) + 8 + ceil(totalBits / 8).
uint64_t expectedArchiveSize(uint16_t symbolCount, uint64_t totalBits);

// Returns true when the header, symbol table and file size are mutually consistent.
// On failure returns false and, if `reason` is non-null, stores a human-readable
// explanation (usable directly in the statistics command's error message).
bool validateArchiveStructure(const ArchiveHeader& header,
                              const FrequencyList& frequencies,
                              uint64_t actualFileSize,
                              std::string* reason = nullptr);
