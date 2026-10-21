#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

class CRC32 {
public:
    // Calculate the standard IEEE 802.3 CRC-32 for a byte buffer.
    static uint32_t calculate(const uint8_t* data, size_t length);

    // Continue a CRC previously returned by calculate() or update().
    static uint32_t update(uint32_t currentCRC, const uint8_t* data, size_t length);

    // Calculate the CRC of a file read as binary data.
    static uint32_t calculateFile(const std::string& filePath);
};
