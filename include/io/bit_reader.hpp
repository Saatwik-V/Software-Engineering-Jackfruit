#pragma once

#include <cstdint>
#include <iostream>

namespace jackfruit::io {

/**
 * @brief Stateful bit-level stream reader extracting bits MSB-first.
 * 
 * Conforms to the Jackfruit Binary Archive Format Specification (Section 4).
 * Halts strictly when totalBits is reached to ensure trailing padding bits
 * in the final byte are never emitted.
 */
class BitReader {
public:
    /**
     * @brief Constructs a BitReader wrapping an input byte stream.
     * @param inputStream Reference to an opened std::istream.
     * @param totalBits The exact number of valid payload bits to consume.
     */
    explicit BitReader(std::istream& inputStream, uint64_t totalBits);

    ~BitReader() = default;

    /**
     * @brief Reads a single bit from the stream (MSB-first: Bit 7 down to Bit 0).
     * @param bit Output reference receiving the read bit value (0 or 1).
     * @return true if a bit was successfully read, false if totalBits or EOF was reached.
     */
    bool readBit(uint8_t& bit);

    /**
     * @brief Checks if there are more valid bits remaining to be read.
     * @return true if bitsRead < totalBits, false otherwise.
     */
    bool hasMoreBits() const;

    /**
     * @brief Returns the number of bits read so far.
     */
    uint64_t getBitsRead() const;

    /**
     * @brief Returns the total valid bits configured for this stream.
     */
    uint64_t getTotalBits() const;

private:
    std::istream& in_;
    uint64_t totalBits_{0};
    uint64_t bitsRead_{0};
    uint8_t buffer_{0};
    uint8_t bitIndex_{8}; // 8 indicates the buffer is empty and a new byte must be fetched
};

} // namespace jackfruit::io