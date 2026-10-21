#include "archive/archive_writer.hpp"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <stdexcept>

namespace {

void putU16(std::vector<uint8_t>& out, uint16_t value) {
    out.push_back(static_cast<uint8_t>(value >> 8));
    out.push_back(static_cast<uint8_t>(value & 0xFF));
}

void putU32(std::vector<uint8_t>& out, uint32_t value) {
    for (int shift = 24; shift >= 0; shift -= 8) {
        out.push_back(static_cast<uint8_t>((value >> shift) & 0xFF));
    }
}

void putU64(std::vector<uint8_t>& out, uint64_t value) {
    for (int shift = 56; shift >= 0; shift -= 8) {
        out.push_back(static_cast<uint8_t>((value >> shift) & 0xFF));
    }
}

}  // namespace

std::vector<uint8_t> ArchiveWriter::serialize(const ArchiveHeader& header,
                                              const FrequencyList& frequencies,
                                              const std::vector<uint8_t>& payload) {
    if (frequencies.size() > ARCHIVE_MAX_SYMBOLS) {
        throw std::invalid_argument("ArchiveWriter: symbol count exceeds 256");
    }

    // Normalise the fields the writer owns.
    ArchiveHeader h = header;
    std::memcpy(h.magic, "JACK", 4);
    h.version = SUPPORTED_ARCHIVE_VERSION;
    h.symbolCount = static_cast<uint16_t>(frequencies.size());

    const uint64_t totalSize = ARCHIVE_FIXED_HEADER_SIZE + ARCHIVE_SYMBOL_COUNT_SIZE +
                               frequencies.size() * ARCHIVE_SYMBOL_ENTRY_SIZE +
                               ARCHIVE_TOTAL_BITS_SIZE + payload.size();

    std::string reason;
    if (!validateArchiveStructure(h, frequencies, totalSize, &reason)) {
        throw std::invalid_argument("ArchiveWriter: " + reason);
    }

    std::vector<uint8_t> out;
    out.reserve(static_cast<std::size_t>(totalSize));

    // Fixed header (18 bytes).
    out.insert(out.end(), h.magic, h.magic + 4);
    out.push_back(h.version);
    out.push_back(h.flags);
    putU64(out, h.originalSize);
    putU32(out, h.checksumCRC32);

    // Section B: tree reconstruction metadata.
    putU16(out, h.symbolCount);
    for (const auto& entry : frequencies) {
        out.push_back(entry.first);
        putU64(out, entry.second);
    }

    // Section C: payload.
    putU64(out, h.totalBits);
    out.insert(out.end(), payload.begin(), payload.end());

    return out;
}

void ArchiveWriter::write(const std::string& archivePath,
                          const ArchiveHeader& header,
                          const FrequencyList& frequencies,
                          const std::vector<uint8_t>& payload) {
    const std::vector<uint8_t> bytes = serialize(header, frequencies, payload);

    std::ofstream out(archivePath, std::ios::binary | std::ios::trunc);
    if (!out.is_open()) {
        throw std::runtime_error("ArchiveWriter: cannot open '" + archivePath + "' for writing");
    }
    out.write(reinterpret_cast<const char*>(bytes.data()),
              static_cast<std::streamsize>(bytes.size()));
    out.flush();
    if (!out) {
        out.close();
        std::remove(archivePath.c_str());
        throw std::runtime_error("ArchiveWriter: failed while writing '" + archivePath + "'");
    }
}
