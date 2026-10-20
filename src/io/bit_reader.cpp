#include "io/bit_reader.hpp"

namespace jackfruit::io {

BitReader::BitReader(std::istream& inputStream, uint64_t totalBits)
    : in_(inputStream),
      totalBits_(totalBits),
      bitsRead_(0),
      buffer_(0),
      bitIndex_(8) {}

bool BitReader::readBit(uint8_t& bit) {
    if (bitsRead_ >= totalBits_) {
        return false;
    }

    if (bitIndex_ >= 8) {
        char ch = 0;
        if (!in_.get(ch)) {
            return false;
        }
        buffer_ = static_cast<uint8_t>(ch);
        bitIndex_ = 0;
    }

    // Extract MSB-first: bit 7 down to bit 0
    bit = static_cast<uint8_t>((buffer_ >> (7 - bitIndex_)) & 0x01);
    ++bitIndex_;
    ++bitsRead_;

    return true;
}

bool BitReader::hasMoreBits() const {
    return bitsRead_ < totalBits_;
}

uint64_t BitReader::getBitsRead() const {
    return bitsRead_;
}

uint64_t BitReader::getTotalBits() const {
    return totalBits_;
}

} // namespace jackfruit::io