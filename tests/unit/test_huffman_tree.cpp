#include <gtest/gtest.h>
#include "compression/huffman_tree.hpp"
#include "compression/frequency_table.hpp"
#include <algorithm>

using namespace jackfruit::compression;

// Helper function to verify prefix-free property
static bool isPrefixFree(const std::unordered_map<uint8_t, std::string>& codebook) {
    for (auto it1 = codebook.begin(); it1 != codebook.end(); ++it1) {
        for (auto it2 = codebook.begin(); it2 != codebook.end(); ++it2) {
            if (it1 == it2) continue;
            const std::string& c1 = it1->second;
            const std::string& c2 = it2->second;
            if (c2.rfind(c1, 0) == 0) { // c1 is a prefix of c2
                return false;
            }
        }
    }
    return true;
}

TEST(HuffmanTreeTest, EmptyTableYieldsEmptyTree) {
    FrequencyTable table;
    HuffmanTree tree;
    tree.buildFromFrequencyTable(table);

    EXPECT_TRUE(tree.empty());
    EXPECT_TRUE(tree.getCodebook().empty());
}

TEST(HuffmanTreeTest, SingleCharacterEdgeCaseTC_COMP_02) {
    FrequencyTable table;
    table.setFrequency('A', 1000);

    HuffmanTree tree;
    tree.buildFromFrequencyTable(table);

    EXPECT_FALSE(tree.empty());
    EXPECT_EQ(tree.getCodebook().size(), 1u);
    EXPECT_TRUE(tree.hasCode('A'));
    EXPECT_EQ(tree.getCode('A'), "0");
}

TEST(HuffmanTreeTest, PrefixFreePropertyHolds) {
    FrequencyTable table;
    const std::string text = "the quick brown fox jumps over the lazy dog";
    table.countFrequencies(reinterpret_cast<const uint8_t*>(text.data()), text.size());

    HuffmanTree tree;
    tree.buildFromFrequencyTable(table);

    EXPECT_FALSE(tree.empty());
    EXPECT_EQ(tree.getCodebook().size(), table.getUniqueSymbolCount());
    EXPECT_TRUE(isPrefixFree(tree.getCodebook()));
}

TEST(HuffmanTreeTest, HighFrequencyGetsShorterCode) {
    FrequencyTable table;
    table.setFrequency('E', 1000); // Very common
    table.setFrequency('T', 500);
    table.setFrequency('Q', 2);    // Rare
    table.setFrequency('Z', 1);    // Rare

    HuffmanTree tree;
    tree.buildFromFrequencyTable(table);

    std::string codeE = tree.getCode('E');
    std::string codeZ = tree.getCode('Z');

    EXPECT_FALSE(codeE.empty());
    EXPECT_FALSE(codeZ.empty());
    EXPECT_LT(codeE.length(), codeZ.length());
}

TEST(HuffmanTreeTest, DeterministicTieBreakingJACK_F_002) {
    // Two symbols with identical frequencies
    std::array<uint64_t, 256> freqs1{0};
    freqs1['A'] = 10;
    freqs1['B'] = 10;

    HuffmanTree tree1;
    tree1.buildFromFrequencies(freqs1);

    HuffmanTree tree2;
    tree2.buildFromFrequencies(freqs1);

    EXPECT_EQ(tree1.getCode('A'), tree2.getCode('A'));
    EXPECT_EQ(tree1.getCode('B'), tree2.getCode('B'));
}

TEST(HuffmanTreeTest, All256SymbolsPrefixTree) {
    std::array<uint64_t, 256> freqs{0};
    for (size_t i = 0; i < 256; ++i) {
        freqs[i] = i + 1; // frequencies 1 to 256
    }

    HuffmanTree tree;
    tree.buildFromFrequencies(freqs);

    EXPECT_EQ(tree.getCodebook().size(), 256u);
    EXPECT_TRUE(isPrefixFree(tree.getCodebook()));
}
