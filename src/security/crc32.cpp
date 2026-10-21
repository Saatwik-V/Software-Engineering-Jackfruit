#include "security/crc32.hpp"

#include <array>
#include <fstream>
#include <stdexcept>

namespace {
constexpr uint32_t kReflectedPolynomial = 0xEDB88320U;
constexpr uint32_t kInitialAndFinalXor = 0xFFFFFFFFU;

constexpr std::array<uint32_t, 256> makeLookupTable() {
    std::array<uint32_t, 256> table{};
    for (uint32_t i = 0; i < table.size(); ++i) {
        uint32_t value = i;
        for (unsigned int bit = 0; bit < 8; ++bit) {
            value = (value & 1U) ? ((value >> 1U) ^ kReflectedPolynomial) : (value >> 1U);
        }
        table[i] = value;
    }
    return table;
}

constexpr auto kLookupTable = makeLookupTable();
}  // namespace

uint32_t CRC32::calculate(const uint8_t* data, size_t length) {
    return update(0U, data, length);
}

uint32_t CRC32::update(uint32_t currentCRC, const uint8_t* data, size_t length) {
    uint32_t crc = currentCRC ^ kInitialAndFinalXor;
    for (size_t i = 0; i < length; ++i) {
        const uint8_t tableIndex = static_cast<uint8_t>((crc ^ data[i]) & 0xFFU);
        crc = kLookupTable[tableIndex] ^ (crc >> 8U);
    }
    return crc ^ kInitialAndFinalXor;
}

uint32_t CRC32::calculateFile(const std::string& filePath) {
    std::ifstream input(filePath, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Unable to open file for CRC-32 calculation: " + filePath);
    }

    std::array<uint8_t, 8192> buffer{};
    uint32_t crc = 0U;
    while (input) {
        input.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));
        const std::streamsize bytesRead = input.gcount();
        if (bytesRead > 0) {
            crc = update(crc, buffer.data(), static_cast<size_t>(bytesRead));
        }
    }

    if (!input.eof()) {
        throw std::runtime_error("Unable to read file for CRC-32 calculation: " + filePath);
    }
    return crc;
}
