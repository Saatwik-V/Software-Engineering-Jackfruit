#include "compression/huffman_tree.hpp"
#include <queue>
#include <vector>

namespace jackfruit::compression {

void HuffmanTree::clear() {
    root_ = nullptr;
    codebook_.clear();
}

bool HuffmanTree::empty() const {
    return root_ == nullptr;
}

std::shared_ptr<HuffmanNode> HuffmanTree::getRoot() const {
    return root_;
}

const std::unordered_map<uint8_t, std::string>& HuffmanTree::getCodebook() const {
    return codebook_;
}

std::string HuffmanTree::getCode(uint8_t symbol) const {
    auto it = codebook_.find(symbol);
    if (it != codebook_.end()) {
        return it->second;
    }
    return "";
}

bool HuffmanTree::hasCode(uint8_t symbol) const {
    return codebook_.find(symbol) != codebook_.end();
}

void HuffmanTree::buildFromFrequencyTable(const FrequencyTable& table) {
    buildFromFrequencies(table.getFrequencies());
}

void HuffmanTree::buildFromFrequencies(const std::array<uint64_t, 256>& frequencies) {
    clear();

    std::priority_queue<
        std::shared_ptr<HuffmanNode>,
        std::vector<std::shared_ptr<HuffmanNode>>,
        HuffmanNodeComparator
    > pq;

    for (size_t i = 0; i < 256; ++i) {
        if (frequencies[i] > 0) {
            pq.push(HuffmanNode::createLeaf(static_cast<uint8_t>(i), frequencies[i]));
        }
    }

    // Case 0: Empty file / zero symbols
    if (pq.empty()) {
        return;
    }

    // Case 1: Single unique symbol (Edge case TC-COMP-02)
    if (pq.size() == 1) {
        auto singleLeaf = pq.top();
        pq.pop();
        // Create an internal root pointing to the leaf on the left (bit 0)
        root_ = HuffmanNode::createInternal(singleLeaf, nullptr);
        codebook_[singleLeaf->symbol] = "0";
        return;
    }

    // Case 2: 2 or more unique symbols (Standard Huffman tree algorithm)
    while (pq.size() > 1) {
        auto left = pq.top();
        pq.pop();

        auto right = pq.top();
        pq.pop();

        auto parent = HuffmanNode::createInternal(left, right);
        pq.push(parent);
    }

    root_ = pq.top();
    pq.pop();

    generateCodesRecursive(root_, "");
}

void HuffmanTree::generateCodesRecursive(const std::shared_ptr<HuffmanNode>& node, const std::string& currentCode) {
    if (!node) {
        return;
    }

    if (node->isLeaf()) {
        codebook_[node->symbol] = currentCode.empty() ? "0" : currentCode;
        return;
    }

    if (node->left) {
        generateCodesRecursive(node->left, currentCode + "0");
    }

    if (node->right) {
        generateCodesRecursive(node->right, currentCode + "1");
    }
}

} // namespace jackfruit::compression
