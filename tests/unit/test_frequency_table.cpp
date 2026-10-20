#include <gtest/gtest.h>
#include "compression/frequency_table.hpp"
#include <sstream>

using namespace jackfruit::compression;

TEST(FrequencyTableTest, EmptyStreamYieldsZeroCounts) {
    FrequencyTable table;
    std::istringstream stream("");
    table.countFrequencies(stream);

    EXPECT_EQ(table.getTotalBytes(), 0u);
    EXPECT_EQ(table.getUniqueSymbolCount(), 0u);
}

TEST(FrequencyTableTest, SimpleBufferFrequencies) {
    FrequencyTable table;
    const std::string text = "abracadabra";
    table.countFrequencies(reinterpret_cast<const uint8_t*>(text.data()), text.size());

    // 'a' = 5, 'b' = 2, 'r' = 2, 'c' = 1, 'd' = 1 -> 5 unique, 11 total
    EXPECT_EQ(table.getTotalBytes(), 11u);
    EXPECT_EQ(table.getUniqueSymbolCount(), 5u);
    EXPECT_EQ(table.getFrequency('a'), 5u);
    EXPECT_EQ(table.getFrequency('b'), 2u);
    EXPECT_EQ(table.getFrequency('r'), 2u);
    EXPECT_EQ(table.getFrequency('c'), 1u);
    EXPECT_EQ(table.getFrequency('d'), 1u);
    EXPECT_EQ(table.getFrequency('z'), 0u);
}

TEST(FrequencyTableTest, ManualSetFrequency) {
    FrequencyTable table;
    table.setFrequency('X', 42);
    table.setFrequency('Y', 10);

    EXPECT_EQ(table.getFrequency('X'), 42u);
    EXPECT_EQ(table.getFrequency('Y'), 10u);
    EXPECT_EQ(table.getUniqueSymbolCount(), 2u);
    EXPECT_EQ(table.getTotalBytes(), 52u);

    table.clear();
    EXPECT_EQ(table.getTotalBytes(), 0u);
    EXPECT_EQ(table.getUniqueSymbolCount(), 0u);
}

TEST(FrequencyTableTest, All256ByteValuesCounted) {
    FrequencyTable table;
    std::vector<uint8_t> allBytes(256);
    for (size_t i = 0; i < 256; ++i) {
        allBytes[i] = static_cast<uint8_t>(i);
    }

    table.countFrequencies(allBytes.data(), allBytes.size());
    EXPECT_EQ(table.getUniqueSymbolCount(), 256u);
    EXPECT_EQ(table.getTotalBytes(), 256u);
    for (size_t i = 0; i < 256; ++i) {
        EXPECT_EQ(table.getFrequency(static_cast<uint8_t>(i)), 1u);
    }
}
