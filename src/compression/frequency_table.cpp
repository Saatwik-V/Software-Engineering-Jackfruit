#include "compression/frequency_table.hpp"
#include <fstream>
#include <vector>

namespace jackfruit::compression {

FrequencyTable::FrequencyTable() {
    clear();
}

void FrequencyTable::clear() {
    frequencies_.fill(0);
    totalBytes_ = 0;
    uniqueSymbols_ = 0;
}

void FrequencyTable::countFrequencies(std::istream& stream) {
    constexpr size_t BUFFER_SIZE = 64 * 1024; // 64 KB buffer for high performance
    std::vector<char> buffer(BUFFER_SIZE);

    while (stream) {
        stream.read(buffer.data(), BUFFER_SIZE);
        std::streamsize bytesRead = stream.gcount();
        if (bytesRead <= 0) {
            break;
        }

        const uint8_t* bytePtr = reinterpret_cast<const uint8_t*>(buffer.data());
        for (std::streamsize i = 0; i < bytesRead; ++i) {
            frequencies_[bytePtr[i]]++;
        }
        totalBytes_ += static_cast<uint64_t>(bytesRead);
    }

    recalculateSummary();
}

void FrequencyTable::countFrequencies(const uint8_t* data, size_t size) {
    if (!data || size == 0) {
        return;
    }

    for (size_t i = 0; i < size; ++i) {
        frequencies_[data[i]]++;
    }
    totalBytes_ += size;

    recalculateSummary();
}

bool FrequencyTable::countFrequenciesFromFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    countFrequencies(file);
    return true;
}

uint64_t FrequencyTable::getFrequency(uint8_t symbol) const {
    return frequencies_[symbol];
}

void FrequencyTable::setFrequency(uint8_t symbol, uint64_t count) {
    frequencies_[symbol] = count;
    recalculateSummary();
}

const std::array<uint64_t, 256>& FrequencyTable::getFrequencies() const {
    return frequencies_;
}

size_t FrequencyTable::getUniqueSymbolCount() const {
    return uniqueSymbols_;
}

uint64_t FrequencyTable::getTotalBytes() const {
    return totalBytes_;
}

void FrequencyTable::recalculateSummary() {
    uniqueSymbols_ = 0;
    totalBytes_ = 0;

    for (size_t i = 0; i < 256; ++i) {
        if (frequencies_[i] > 0) {
            uniqueSymbols_++;
            totalBytes_ += frequencies_[i];
        }
    }
}

} // namespace jackfruit::compression
