#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>
#include "io/bit_reader.hpp"

using jackfruit::io::BitReader;

TEST(BitReaderTest, EmptyStreamZeroBits) {
    std::istringstream stream("");
    BitReader reader(stream, 0);

    EXPECT_FALSE(reader.hasMoreBits());
    EXPECT_EQ(reader.getBitsRead(), 0);
    EXPECT_EQ(reader.getTotalBits(), 0);

    uint8_t bit = 99;
    EXPECT_FALSE(reader.readBit(bit));
    EXPECT_EQ(bit, 99);
}

TEST(BitReaderTest, SingleByteFullReadMSBFirst) {
    // 0b10110001 = 0xB1 (177)
    std::string data;
    data.push_back(static_cast<char>(0xB1));
    std::istringstream stream(data);

    BitReader reader(stream, 8);
    EXPECT_TRUE(reader.hasMoreBits());

    std::vector<uint8_t> expectedBits = {1, 0, 1, 1, 0, 0, 0, 1};
    for (size_t i = 0; i < expectedBits.size(); ++i) {
        uint8_t bit = 0;
        EXPECT_TRUE(reader.readBit(bit));
        EXPECT_EQ(bit, expectedBits[i]);
        EXPECT_EQ(reader.getBitsRead(), i + 1);
    }

    EXPECT_FALSE(reader.hasMoreBits());
    uint8_t extraBit = 0;
    EXPECT_FALSE(reader.readBit(extraBit));
}

// TC-DECOMP-03: BitReader Non-Byte-Aligned Boundary Reading
TEST(BitReaderTest, BaselineTCDecomp03UnalignedThirteenBits) {
    // Stream containing: 0b11010011 (0xD3) followed by 0b00101000 (0x28)
    // Total bits = 13
    // Expected sequence: 1, 1, 0, 1, 0, 0, 1, 1,   0, 0, 1, 0, 1
    // Trailing 3 bits (0, 0, 0) must be completely ignored.
    std::string data;
    data.push_back(static_cast<char>(0xD3));
    data.push_back(static_cast<char>(0x28));
    std::istringstream stream(data);

    BitReader reader(stream, 13);
    EXPECT_EQ(reader.getTotalBits(), 13);

    std::vector<uint8_t> expectedBits = {1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1};
    for (size_t i = 0; i < expectedBits.size(); ++i) {
        EXPECT_TRUE(reader.hasMoreBits());
        uint8_t bit = 0xFF;
        EXPECT_TRUE(reader.readBit(bit));
        EXPECT_EQ(bit, expectedBits[i]);
    }

    // 14th read must fail and not consume padding bits
    EXPECT_FALSE(reader.hasMoreBits());
    uint8_t bit14 = 0xFF;
    EXPECT_FALSE(reader.readBit(bit14));
    EXPECT_EQ(reader.getBitsRead(), 13);
}

TEST(BitReaderTest, TruncatedStreamReturnsFalseGracefully) {
    // Declare 16 bits, but provide only 1 byte (8 bits)
    std::string data;
    data.push_back(static_cast<char>(0xAA));
    std::istringstream stream(data);

    BitReader reader(stream, 16);

    for (int i = 0; i < 8; ++i) {
        uint8_t bit = 0;
        EXPECT_TRUE(reader.readBit(bit));
    }

    // Attempting to read 9th bit when stream has no more bytes
    uint8_t bit9 = 0;
    EXPECT_FALSE(reader.readBit(bit9));
    EXPECT_EQ(reader.getBitsRead(), 8);
}