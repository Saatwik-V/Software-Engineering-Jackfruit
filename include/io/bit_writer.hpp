/**
 * @file bit_writer.hpp
 * @brief Bitstream accumulator packing variable-length bits MSB-first into bytes (JACK-F-003).
 *
 * Implements a stateful bit accumulator that writes individual bits or bit sequences
 * into a 1-byte buffer MSB-first (bit 7 down to bit 0), flushing complete bytes to an
 * output stream and zero-padding any remainder on flush().
 */

#pragma once

#include <cstdint>
#include <iostream>
#include <string>

namespace jackfruit::io {

/**
 * @brief Stateful bitstream writer emitting bits packed MSB-first.
 *
 * Conforms to the Jackfruit Binary Archive Format Specification (Section 4).
 * Mirrors jackfruit::io::BitReader directly.
 */
class BitWriter {
public:
    /**
     * @brief Constructs a BitWriter wrapping an output stream.
     * @param outputStream Reference to an opened std::ostream in binary mode.
     */
    explicit BitWriter(std::ostream& outputStream);

    /**
     * @brief Destructor automatically flushes any pending bits.
     */
    ~BitWriter();

    /**
     * @brief Writes a single bit (0 or 1) to the bitstream.
     * @param bit Bit value (least significant bit is used).
     */
    void writeBit(uint8_t bit);

    /**
     * @brief Writes a sequence of bits represented as a string of '0' and '1' characters.
     * @param bitSequence String composed of '0' and '1' characters.
     */
    void writeBits(const std::string& bitSequence);

    /**
     * @brief Flushes pending bits into the final byte with zero-padding in the lower bits.
     *
     * If unwritten bits remain (bitCount > 0), they are shifted to the upper bits
     * (MSB-aligned) and padded with zeros in the lower bits before writing.
     */
    void flush();

    /**
     * @brief Returns the total count of valid payload bits written so far.
     * @return uint64_t Total valid bit count.
     */
    uint64_t getTotalBitsWritten() const;

    /**
     * @brief Returns the number of unaligned bits currently waiting in the internal byte buffer.
     * @return uint8_t Count in range [0, 7].
     */
    uint8_t getPendingBitCount() const;

private:
    std::ostream& out_;
    uint8_t buffer_{0};
    uint8_t bitCount_{0};
    uint64_t totalBits_{0};
};

} // namespace jackfruit::io
