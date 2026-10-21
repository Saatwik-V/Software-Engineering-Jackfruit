#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "security/crc32.hpp"

TEST(CRC32Test, EmptyBufferReturnsZero) {
    EXPECT_EQ(CRC32::calculate(nullptr, 0), 0x00000000U);
}

TEST(CRC32Test, StandardCheckValueMatchesIEEE8023) {
    const std::string input = "123456789";
    EXPECT_EQ(CRC32::calculate(reinterpret_cast<const uint8_t*>(input.data()), input.size()),
              0xCBF43926U);
}

TEST(CRC32Test, IncrementalUpdatesMatchSinglePassCalculation) {
    const std::string first = "1234";
    const std::string second = "56789";
    uint32_t crc = CRC32::calculate(reinterpret_cast<const uint8_t*>(first.data()), first.size());
    crc = CRC32::update(crc, reinterpret_cast<const uint8_t*>(second.data()), second.size());

    EXPECT_EQ(crc, 0xCBF43926U);
}

TEST(CRC32Test, FileCalculationReadsBinaryBytes) {
    const std::string path = "temp_crc32_test.bin";
    const std::vector<uint8_t> contents = {0x00, 0x80, 0xFF, 0x01};
    {
        std::ofstream output(path, std::ios::binary);
        ASSERT_TRUE(output);
        output.write(reinterpret_cast<const char*>(contents.data()),
                     static_cast<std::streamsize>(contents.size()));
        ASSERT_TRUE(output);
    }

    const uint32_t expected = CRC32::calculate(contents.data(), contents.size());
    EXPECT_EQ(CRC32::calculateFile(path), expected);
    std::remove(path.c_str());
}
