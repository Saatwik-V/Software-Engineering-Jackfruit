#include "io/bit_writer.hpp"

namespace jackfruit::io {

BitWriter::BitWriter(std::ostream& outputStream)
    : out_(outputStream),
      buffer_(0),
      bitCount_(0),
      totalBits_(0) {}

BitWriter::~BitWriter() {
    flush();
}

void BitWriter::writeBit(uint8_t bit) {
    // Accumulate MSB-first: shift buffer left and insert least significant bit
    buffer_ = static_cast<uint8_t>((buffer_ << 1) | (bit & 0x01));
    ++bitCount_;
    ++totalBits_;

    // Once 8 bits are collected, emit complete byte to stream
    if (bitCount_ == 8) {
        out_.put(static_cast<char>(buffer_));
        buffer_ = 0;
        bitCount_ = 0;
    }
}

void BitWriter::writeBits(const std::string& bitSequence) {
    for (char c : bitSequence) {
        if (c == '0') {
            writeBit(0);
        } else if (c == '1') {
            writeBit(1);
        }
    }
}

void BitWriter::flush() {
    if (bitCount_ > 0) {
        // Shift remaining bits to the upper MSB positions; lower bits become 0
        buffer_ = static_cast<uint8_t>(buffer_ << (8 - bitCount_));
        out_.put(static_cast<char>(buffer_));
        buffer_ = 0;
        bitCount_ = 0;
    }
    out_.flush();
}

uint64_t BitWriter::getTotalBitsWritten() const {
    return totalBits_;
}

uint8_t BitWriter::getPendingBitCount() const {
    return bitCount_;
}

} // namespace jackfruit::io
