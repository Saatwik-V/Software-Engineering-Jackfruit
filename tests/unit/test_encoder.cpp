#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>
#include "compression/encoder.hpp"
#include "compression/frequency_table.hpp"
#include "compression/huffman_tree.hpp"
#include "io/bit_writer.hpp"

using namespace jackfruit::compression;
using namespace jackfruit::io;

TEST(EncoderTest, EmptyDataEncodesZeroBits) {
    FrequencyTable table;
    HuffmanTree tree;
    tree.buildFromFrequencyTable(table);

    Encoder encoder(tree);
    std::ostringstream out;
    BitWriter writer(out);

    std::istringstream in("");
    auto result = encoder.encode(in, writer);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.originalBytes, 0u);
    EXPECT_EQ(result.totalBitsEncoded, 0u);
    EXPECT_EQ(result.compressedBytes, 0u);
    EXPECT_TRUE(out.str().empty());
}

TEST(EncoderTest, SimpleStringEncodingCorrectBitLength) {
    const std::string text = "abracadabra";

    FrequencyTable table;
    table.countFrequencies(reinterpret_cast<const uint8_t*>(text.data()), text.size());

    HuffmanTree tree;
    tree.buildFromFrequencyTable(table);

    Encoder encoder(tree);
    std::ostringstream out;
    BitWriter writer(out);

    std::istringstream in(text);
    auto result = encoder.encode(in, writer);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.originalBytes, 11u);
    EXPECT_GT(result.totalBitsEncoded, 0u);

    // Calculate theoretical expected bit count
    uint64_t expectedBits = 0;
    for (char c : text) {
        expectedBits += tree.getCode(static_cast<uint8_t>(c)).length();
    }
    EXPECT_EQ(result.totalBitsEncoded, expectedBits);

    // 11 bytes original (88 bits). Compressed bits should be significantly less than 88 bits
    EXPECT_LT(result.totalBitsEncoded, 88u);
    EXPECT_EQ(result.compressedBytes, (result.totalBitsEncoded + 7) / 8);
}

// TC-COMP-02: Single distinct character edge case
TEST(EncoderTest, SingleDistinctCharacterTC_COMP_02) {
    const std::string text(1000, 'A');

    FrequencyTable table;
    table.setFrequency('A', 1000);

    HuffmanTree tree;
    tree.buildFromFrequencyTable(table);

    Encoder encoder(tree);
    std::ostringstream out;
    BitWriter writer(out);

    auto result = encoder.encode(reinterpret_cast<const uint8_t*>(text.data()), text.size(), writer);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.originalBytes, 1000u);
    // Since 'A' is assigned single bit "0", 1000 'A's must take exactly 1000 bits
    EXPECT_EQ(result.totalBitsEncoded, 1000u);
    // 1000 bits / 8 = 125 bytes
    EXPECT_EQ(result.compressedBytes, 125u);
    EXPECT_EQ(out.str().size(), 125u);
}

TEST(EncoderTest, StreamVsBufferEquivalence) {
    const std::string text = "Jackfruit compression tool test stream vs buffer equality!";

    FrequencyTable table;
    table.countFrequencies(reinterpret_cast<const uint8_t*>(text.data()), text.size());

    HuffmanTree tree;
    tree.buildFromFrequencyTable(table);

    Encoder encoder(tree);

    // 1. Encode via stream
    std::ostringstream streamOut;
    BitWriter streamWriter(streamOut);
    std::istringstream in(text);
    auto resStream = encoder.encode(in, streamWriter);

    // 2. Encode via buffer
    std::ostringstream bufOut;
    BitWriter bufWriter(bufOut);
    auto resBuf = encoder.encode(reinterpret_cast<const uint8_t*>(text.data()), text.size(), bufWriter);

    EXPECT_TRUE(resStream.success);
    EXPECT_TRUE(resBuf.success);
    EXPECT_EQ(resStream.totalBitsEncoded, resBuf.totalBitsEncoded);
    EXPECT_EQ(resStream.compressedBytes, resBuf.compressedBytes);
    EXPECT_EQ(streamOut.str(), bufOut.str());
}

TEST(EncoderTest, MissingSymbolFailsGracefully) {
    FrequencyTable table;
    table.setFrequency('A', 10);
    table.setFrequency('B', 10);

    HuffmanTree tree;
    tree.buildFromFrequencyTable(table);

    Encoder encoder(tree);
    std::ostringstream out;
    BitWriter writer(out);

    // Contains 'C' which is not in the tree
    std::string text = "ABC";
    std::istringstream in(text);
    auto result = encoder.encode(in, writer);

    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.errorMessage.empty());
}
