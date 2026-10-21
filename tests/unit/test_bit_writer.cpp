#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include <string>
#include "io/bit_writer.hpp"
#include "io/bit_reader.hpp"

using jackfruit::io::BitWriter;
using jackfruit::io::BitReader;

TEST(BitWriterTest, EmptyWriterEmitsZeroBytes) {
    std::ostringstream stream;
    {
        BitWriter writer(stream);
        EXPECT_EQ(writer.getTotalBitsWritten(), 0u);
        EXPECT_EQ(writer.getPendingBitCount(), 0u);
    }
    EXPECT_TRUE(stream.str().empty());
}

TEST(BitWriterTest, SingleByteExactMSBFirst) {
    std::ostringstream stream;
    {
        BitWriter writer(stream);
        // Write 0b10110001 = 0xB1 (177)
        std::vector<uint8_t> bits = {1, 0, 1, 1, 0, 0, 0, 1};
        for (uint8_t b : bits) {
            writer.writeBit(b);
        }
        EXPECT_EQ(writer.getTotalBitsWritten(), 8u);
        EXPECT_EQ(writer.getPendingBitCount(), 0u);
    }

    std::string out = stream.str();
    ASSERT_EQ(out.size(), 1u);
    EXPECT_EQ(static_cast<uint8_t>(out[0]), 0xB1);
}

TEST(BitWriterTest, WriteBitsStringSequence) {
    std::ostringstream stream;
    {
        BitWriter writer(stream);
        writer.writeBits("10110001");
        EXPECT_EQ(writer.getTotalBitsWritten(), 8u);
    }

    std::string out = stream.str();
    ASSERT_EQ(out.size(), 1u);
    EXPECT_EQ(static_cast<uint8_t>(out[0]), 0xB1);
}

TEST(BitWriterTest, UnalignedThirteenBitsFlushWithZeroPadding) {
    // 13 bits: 11010011 (0xD3) followed by 5 bits: 00101 (padded with 000 -> 00101000 = 0x28)
    std::ostringstream stream;
    {
        BitWriter writer(stream);
        writer.writeBits("11010011"); // 8 bits
        writer.writeBits("00101");    // 5 bits
        EXPECT_EQ(writer.getTotalBitsWritten(), 13u);
        EXPECT_EQ(writer.getPendingBitCount(), 5u);
        writer.flush();
    }

    std::string out = stream.str();
    ASSERT_EQ(out.size(), 2u);
    EXPECT_EQ(static_cast<uint8_t>(out[0]), 0xD3);
    EXPECT_EQ(static_cast<uint8_t>(out[1]), 0x28);
}

// Interoperability Test with Tanmayi's BitReader: BitWriter -> BitReader round-trip
TEST(BitWriterTest, InteropRoundTripWithBitReader) {
    std::vector<uint8_t> testBits = {
        1, 0, 1, 1, 0, 0, 1, 0, // Byte 1 (0xB2)
        1, 1, 1, 0, 0, 0, 1, 1, // Byte 2 (0xE3)
        0, 1, 0, 1, 1           // 5 unaligned bits -> padded with 3 zeros
    };
    uint64_t totalBits = testBits.size(); // 21 bits

    std::stringstream bitstream;
    {
        BitWriter writer(bitstream);
        for (uint8_t b : testBits) {
            writer.writeBit(b);
        }
        writer.flush();
    }

    // Now read back with BitReader
    BitReader reader(bitstream, totalBits);
    EXPECT_TRUE(reader.hasMoreBits());

    for (size_t i = 0; i < testBits.size(); ++i) {
        uint8_t readBit = 0xFF;
        EXPECT_TRUE(reader.readBit(readBit));
        EXPECT_EQ(readBit, testBits[i]) << "Mismatch at bit index " << i;
    }

    // Reader must not consume trailing padding bits
    EXPECT_FALSE(reader.hasMoreBits());
    uint8_t extra = 0xFF;
    EXPECT_FALSE(reader.readBit(extra));
}

TEST(BitWriterTest, StreamLargeRandomSequence) {
    constexpr size_t N_BITS = 1000;
    std::vector<uint8_t> randomBits(N_BITS);
    for (size_t i = 0; i < N_BITS; ++i) {
        randomBits[i] = static_cast<uint8_t>((i * 7 + 13) % 2);
    }

    std::stringstream ss;
    {
        BitWriter writer(ss);
        for (uint8_t b : randomBits) {
            writer.writeBit(b);
        }
        writer.flush();
    }

    BitReader reader(ss, N_BITS);
    for (size_t i = 0; i < N_BITS; ++i) {
        uint8_t b = 0xFF;
        ASSERT_TRUE(reader.readBit(b));
        ASSERT_EQ(b, randomBits[i]);
    }
    EXPECT_FALSE(reader.hasMoreBits());
}
