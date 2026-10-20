/**
 * @file huffman_tree.hpp
 * @brief Canonical Huffman prefix tree builder and codebook generator (JACK-F-002, JACK-F-003).
 *
 * Implements a min-heap priority queue based prefix tree construction algorithm
 * that guarantees deterministic tie-breaking by lowest symbol value per JACK-F-002,
 * gracefully handling edge cases including empty inputs and single-symbol datasets (TC-COMP-02).
 */

#pragma once

#include "compression/frequency_table.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <array>
#include <cstdint>

namespace jackfruit::compression {

/**
 * @brief Node in a binary Huffman prefix tree.
 */
struct HuffmanNode {
    uint8_t symbol{0};                   ///< Byte symbol represented by this leaf (0 for internal nodes).
    uint64_t frequency{0};               ///< Aggregate frequency of symbols in this subtree.
    uint8_t minSymbol{0};                ///< Smallest byte value in subtree for deterministic tie-breaking (JACK-F-002).
    std::shared_ptr<HuffmanNode> left{nullptr};  ///< Left child pointer (encodes bit '0').
    std::shared_ptr<HuffmanNode> right{nullptr}; ///< Right child pointer (encodes bit '1').

    /**
     * @brief Checks whether this node is a leaf node holding a symbol.
     * @return true if both left and right children are null.
     */
    bool isLeaf() const {
        return left == nullptr && right == nullptr;
    }

    /**
     * @brief Factory creating a leaf node.
     * @param sym Symbol byte value.
     * @param freq Occurrence frequency.
     * @return std::shared_ptr<HuffmanNode> Initialized leaf node.
     */
    static std::shared_ptr<HuffmanNode> createLeaf(uint8_t sym, uint64_t freq) {
        auto node = std::make_shared<HuffmanNode>();
        node->symbol = sym;
        node->frequency = freq;
        node->minSymbol = sym;
        return node;
    }

    /**
     * @brief Factory creating an internal node joining two subtrees.
     * @param l Left child subtree.
     * @param r Right child subtree (can be null in single-symbol edge cases).
     * @return std::shared_ptr<HuffmanNode> Initialized internal node.
     */
    static std::shared_ptr<HuffmanNode> createInternal(std::shared_ptr<HuffmanNode> l, std::shared_ptr<HuffmanNode> r) {
        auto node = std::make_shared<HuffmanNode>();
        node->symbol = 0;
        node->frequency = (l ? l->frequency : 0) + (r ? r->frequency : 0);
        if (l && r) {
            node->minSymbol = std::min(l->minSymbol, r->minSymbol);
        } else if (l) {
            node->minSymbol = l->minSymbol;
        } else if (r) {
            node->minSymbol = r->minSymbol;
        }
        node->left = std::move(l);
        node->right = std::move(r);
        return node;
    }
};

/**
 * @brief Min-heap priority comparator for HuffmanNode pointers.
 *
 * Enforces:
 * 1. Primary order: Lower frequency has higher priority.
 * 2. Deterministic tie-breaking: Lower minSymbol byte has higher priority (JACK-F-002).
 */
struct HuffmanNodeComparator {
    bool operator()(const std::shared_ptr<HuffmanNode>& a, const std::shared_ptr<HuffmanNode>& b) const {
        if (a->frequency != b->frequency) {
            return a->frequency > b->frequency; // Min-heap: lowest frequency first
        }
        // Deterministic tie-breaking by lowest byte value per JACK-F-002
        return a->minSymbol > b->minSymbol;
    }
};

/**
 * @brief Builds and navigates a binary Huffman prefix tree for encoding and decoding.
 */
class HuffmanTree {
public:
    HuffmanTree() = default;
    ~HuffmanTree() = default;

    /**
     * @brief Builds the optimal prefix tree from a populated FrequencyTable.
     * @param table Analyzed frequency table.
     */
    void buildFromFrequencyTable(const FrequencyTable& table);

    /**
     * @brief Builds the optimal prefix tree directly from a 256-element frequency array.
     * @param frequencies Array of frequencies indexed by byte value.
     */
    void buildFromFrequencies(const std::array<uint64_t, 256>& frequencies);

    /**
     * @brief Returns the complete codebook mapping symbols to their variable-length bit strings.
     * @return const std::unordered_map<uint8_t, std::string>& Map of symbol to binary string (e.g. 'e' -> "10").
     */
    const std::unordered_map<uint8_t, std::string>& getCodebook() const;

    /**
     * @brief Retrieves the binary prefix code for a specific symbol.
     * @param symbol Byte value.
     * @return std::string Bit sequence as a string of '0' and '1' characters, or empty string if not found.
     */
    std::string getCode(uint8_t symbol) const;

    /**
     * @brief Checks if a symbol exists in the current codebook.
     * @param symbol Byte value.
     * @return true if code exists.
     */
    bool hasCode(uint8_t symbol) const;

    /**
     * @brief Exposes the root node of the tree for bit-by-bit traversal during decompression.
     * @return std::shared_ptr<HuffmanNode> Root of tree (null if tree is empty).
     */
    std::shared_ptr<HuffmanNode> getRoot() const;

    /**
     * @brief Checks if the tree is empty (no symbols).
     * @return true if tree has no nodes.
     */
    bool empty() const;

    /**
     * @brief Clears the tree and codebook.
     */
    void clear();

private:
    std::shared_ptr<HuffmanNode> root_{nullptr};
    std::unordered_map<uint8_t, std::string> codebook_;

    void generateCodesRecursive(const std::shared_ptr<HuffmanNode>& node, const std::string& currentCode);
};

} // namespace jackfruit::compression
