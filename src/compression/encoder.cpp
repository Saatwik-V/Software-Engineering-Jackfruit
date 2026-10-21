#include "compression/encoder.hpp"
#include <vector>

namespace jackfruit::compression {

Encoder::Encoder(const HuffmanTree& tree)
    : tree_(tree) {}

EncoderResult Encoder::encode(std::istream& in, io::BitWriter& writer) {
    EncoderResult result;
    result.success = false;

    if (tree_.empty()) {
        // Empty tree encodes empty data successfully with 0 bits
        result.success = true;
        result.originalBytes = 0;
        result.totalBitsEncoded = 0;
        result.compressedBytes = 0;
        return result;
    }

    constexpr size_t BUFFER_SIZE = 64 * 1024; // 64 KB chunk buffer
    std::vector<char> buffer(BUFFER_SIZE);

    uint64_t startBits = writer.getTotalBitsWritten();

    while (in) {
        in.read(buffer.data(), BUFFER_SIZE);
        std::streamsize bytesRead = in.gcount();
        if (bytesRead <= 0) {
            break;
        }

        const uint8_t* bytePtr = reinterpret_cast<const uint8_t*>(buffer.data());
        for (std::streamsize i = 0; i < bytesRead; ++i) {
            uint8_t sym = bytePtr[i];
            if (!tree_.hasCode(sym)) {
                result.errorMessage = "Symbol 0x" + std::to_string(sym) + " not present in Huffman codebook.";
                return result;
            }
            writer.writeBits(tree_.getCode(sym));
        }

        result.originalBytes += static_cast<uint64_t>(bytesRead);
    }

    // Flush any pending bits to byte boundary
    writer.flush();

    result.totalBitsEncoded = writer.getTotalBitsWritten() - startBits;
    result.compressedBytes = (result.totalBitsEncoded + 7) / 8;
    result.success = true;

    return result;
}

EncoderResult Encoder::encode(const uint8_t* data, size_t size, io::BitWriter& writer) {
    EncoderResult result;
    result.success = false;
    result.originalBytes = size;

    if (size == 0 || tree_.empty()) {
        result.success = true;
        result.totalBitsEncoded = 0;
        result.compressedBytes = 0;
        return result;
    }

    if (!data) {
        result.errorMessage = "Null data pointer provided to encoder.";
        return result;
    }

    uint64_t startBits = writer.getTotalBitsWritten();

    for (size_t i = 0; i < size; ++i) {
        uint8_t sym = data[i];
        if (!tree_.hasCode(sym)) {
            result.errorMessage = "Symbol 0x" + std::to_string(sym) + " not present in Huffman codebook.";
            return result;
        }
        writer.writeBits(tree_.getCode(sym));
    }

    writer.flush();

    result.totalBitsEncoded = writer.getTotalBitsWritten() - startBits;
    result.compressedBytes = (result.totalBitsEncoded + 7) / 8;
    result.success = true;

    return result;
}

} // namespace jackfruit::compression
