#include "decompression/decoder.hpp"
#include "archive/archive.hpp"
#include "compression/huffman_tree.hpp"
#include "io/bit_reader.hpp"
#include <fstream>
#include <vector>
#include <array>

namespace jackfruit::decompression {

bool Decoder::decompressFile(const std::string& archivePath, const std::string& outputPath) {
    ArchiveHeader header;
    std::vector<std::pair<uint8_t, uint64_t>> freqPairs;
    try {
        ArchiveReader::readValidatedHeader(archivePath, header, freqPairs);
    } catch (...) {
        return false;
    }

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) return false;

    if (header.originalSize == 0 || freqPairs.empty()) {
        outFile.close();
        return true;
    }

    std::array<uint64_t, 256> freqArray{};
    for (const auto& pair : freqPairs) {
        freqArray[pair.first] = pair.second;
    }
    jackfruit::compression::HuffmanTree tree;
    tree.buildFromFrequencies(freqArray);
    auto root = tree.getRoot();
    if (!root) return false;

    std::ifstream inFile(archivePath, std::ios::binary);
    if (!inFile) return false;

    size_t headerOffset = 28 + (header.symbolCount * 9);
    inFile.seekg(headerOffset, std::ios::beg);

    // Pass totalBits to BitReader constructor
    jackfruit::io::BitReader bitReader(inFile, header.totalBits);

    uint64_t bytesWritten = 0;
    auto currentNode = root;

    uint8_t bit = 0;
    while (bytesWritten < header.originalSize && bitReader.readBit(bit)) {
        currentNode = (bit == 0) ? currentNode->left : currentNode->right;
        if (!currentNode) return false;

        if (currentNode->isLeaf()) {
            outFile.put(static_cast<char>(currentNode->symbol));
            bytesWritten++;
            currentNode = root;
        }
    }

    outFile.close();
    return bytesWritten == header.originalSize;
}

} // namespace jackfruit::decompression