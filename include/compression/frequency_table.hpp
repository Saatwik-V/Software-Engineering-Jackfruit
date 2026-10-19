/**
 * @file frequency_table.hpp
 * @brief Byte frequency distribution analyzer for Huffman compression (JACK-F-002).
 *
 * Implements a 256-bin symbol frequency accumulator that scans input data
 * in high-throughput 64 KB chunks to record exact 8-bit character distributions.
 */

#pragma once

#include <array>
#include <cstdint>
#include <cstddef>
#include <istream>
#include <string>

namespace jackfruit::compression {

/**
 * @brief Frequency accumulator mapping 8-bit byte values (0x00 to 0xFF) to occurrence counts.
 */
class FrequencyTable {
public:
    /**
     * @brief Constructs an empty frequency table initialized with all zeros.
     */
    FrequencyTable();

    ~FrequencyTable() = default;

    /**
     * @brief Counts byte occurrences from an arbitrary binary input stream.
     *
     * Reads in 64 KB buffered chunks for high I/O throughput.
     *
     * @param stream Input stream opened in binary mode.
     */
    void countFrequencies(std::istream& stream);

    /**
     * @brief Counts byte occurrences from an in-memory byte buffer.
     *
     * @param data Pointer to raw byte array.
     * @param size Number of bytes in buffer.
     */
    void countFrequencies(const uint8_t* data, size_t size);

    /**
     * @brief Reads a file from disk and accumulates its symbol frequencies.
     *
     * @param filePath Path to the file.
     * @return true If file opened and was read successfully, false otherwise.
     */
    bool countFrequenciesFromFile(const std::string& filePath);

    /**
     * @brief Retrieves the occurrence count for a specific 8-bit symbol.
     *
     * @param symbol Byte value (0x00 - 0xFF).
     * @return uint64_t Number of times this symbol appeared.
     */
    uint64_t getFrequency(uint8_t symbol) const;

    /**
     * @brief Directly sets the frequency of a symbol (used during metadata deserialization).
     *
     * @param symbol Byte value (0x00 - 0xFF).
     * @param count Frequency count.
     */
    void setFrequency(uint8_t symbol, uint64_t count);

    /**
     * @brief Returns a reference to the underlying 256-element frequency array.
     *
     * @return const std::array<uint64_t, 256>& Array indexed by symbol value.
     */
    const std::array<uint64_t, 256>& getFrequencies() const;

    /**
     * @brief Returns the count of unique byte values having frequency > 0.
     *
     * @return size_t Count in range [0, 256].
     */
    size_t getUniqueSymbolCount() const;

    /**
     * @brief Returns the total count of accumulated bytes.
     *
     * @return uint64_t Total bytes analyzed.
     */
    uint64_t getTotalBytes() const;

    /**
     * @brief Resets all frequencies and counters to zero.
     */
    void clear();

private:
    std::array<uint64_t, 256> frequencies_;
    uint64_t totalBytes_{0};
    size_t uniqueSymbols_{0};

    void recalculateSummary();
};

} // namespace jackfruit::compression
