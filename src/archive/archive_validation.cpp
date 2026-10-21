#include "archive/archive_validation.hpp"

#include <cstring>
#include <limits>

namespace {

bool fail(std::string* reason, const char* message) {
    if (reason != nullptr) {
        *reason = message;
    }
    return false;
}

}  // namespace

uint64_t expectedArchiveSize(uint16_t symbolCount, uint64_t totalBits) {
    const uint64_t payloadBytes = totalBits / 8 + ((totalBits % 8) != 0 ? 1 : 0);
    return ARCHIVE_FIXED_HEADER_SIZE + ARCHIVE_SYMBOL_COUNT_SIZE +
           static_cast<uint64_t>(symbolCount) * ARCHIVE_SYMBOL_ENTRY_SIZE +
           ARCHIVE_TOTAL_BITS_SIZE + payloadBytes;
}

bool validateArchiveStructure(const ArchiveHeader& header,
                              const FrequencyList& frequencies,
                              uint64_t actualFileSize,
                              std::string* reason) {
    // Rules 1-2: magic and version.
    if (std::memcmp(header.magic, "JACK", 4) != 0) {
        return fail(reason, "invalid magic bytes (not a .jack archive)");
    }
    if (header.version != SUPPORTED_ARCHIVE_VERSION) {
        return fail(reason, "unsupported archive format version");
    }

    // Rule 7: flag consistency.
    if ((header.flags & ARCHIVE_FLAG_RESERVED_MASK) != 0) {
        return fail(reason, "reserved flag bits must be zero");
    }
    const bool isEmpty = (header.flags & ARCHIVE_FLAG_IS_EMPTY) != 0;
    const bool isUncompressed = (header.flags & ARCHIVE_FLAG_IS_UNCOMPRESSED) != 0;
    if (isEmpty && isUncompressed) {
        return fail(reason, "IS_EMPTY and IS_UNCOMPRESSED flags cannot both be set");
    }

    // Rule 3: symbol count bounds, and the table must match the declared count.
    if (header.symbolCount > ARCHIVE_MAX_SYMBOLS) {
        return fail(reason, "symbol count exceeds 256");
    }
    if (frequencies.size() != header.symbolCount) {
        return fail(reason, "symbol table length does not match symbol count");
    }

    // Rule 4: exact length (shorter = truncated, longer = trailing data).
    const uint64_t expected = expectedArchiveSize(header.symbolCount, header.totalBits);
    if (actualFileSize < expected) {
        return fail(reason, "archive is truncated");
    }
    if (actualFileSize > expected) {
        return fail(reason, "archive has unexpected trailing data");
    }

    if (isEmpty) {
        if (header.originalSize != 0 || header.symbolCount != 0 || header.totalBits != 0 ||
            header.checksumCRC32 != 0) {
            return fail(reason, "empty archive must have zero size, symbols, bits and checksum");
        }
        return true;
    }

    if (isUncompressed) {
        if (header.symbolCount != 0) {
            return fail(reason, "uncompressed archive must not contain a symbol table");
        }
        if (header.originalSize > std::numeric_limits<uint64_t>::max() / 8 ||
            header.totalBits != header.originalSize * 8) {
            return fail(reason, "uncompressed archive bit count must equal original size * 8");
        }
        return true;
    }

    // Rule 6: symbol table consistency for a normally compressed archive.
    if (header.symbolCount == 0) {
        return fail(reason, "non-empty archive has no symbols");
    }
    uint64_t sum = 0;
    for (std::size_t i = 0; i < frequencies.size(); ++i) {
        if (i > 0 && frequencies[i].first <= frequencies[i - 1].first) {
            return fail(reason, "symbols must be strictly ascending without duplicates");
        }
        if (frequencies[i].second == 0) {
            return fail(reason, "symbol frequency must be greater than zero");
        }
        if (frequencies[i].second > std::numeric_limits<uint64_t>::max() - sum) {
            return fail(reason, "symbol frequencies overflow");
        }
        sum += frequencies[i].second;
    }
    if (sum != header.originalSize) {
        return fail(reason, "symbol frequencies do not add up to the original size");
    }
    return true;
}
