#include <gtest/gtest.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include "archive/archive.hpp"
#include "archive/archive_validation.hpp"
#include "archive/archive_writer.hpp"

namespace {

struct Archive {
    ArchiveHeader header{};
    FrequencyList frequencies;
    std::vector<uint8_t> payload;
};

// "AAB": A -> bit 0, B -> bit 1, bitstream 0,0,1 padded with zeros = 0x20.
Archive makeAab() {
    Archive a;
    a.header.flags = 0x00;
    a.header.originalSize = 3;
    a.header.checksumCRC32 = 0xDEADBEEF;
    a.header.totalBits = 3;
    a.frequencies = {{'A', 2}, {'B', 1}};
    a.payload = {0x20};
    return a;
}

std::vector<uint8_t> readAll(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

void writeAll(const std::string& path, const std::vector<uint8_t>& bytes) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
}

}  // namespace

class ArchiveWriterTest : public ::testing::Test {
protected:
    std::string path;

    void SetUp() override {
        // Per-test file name so parallel ctest runs never share a file.
        path = std::string("temp_writer_") +
               ::testing::UnitTest::GetInstance()->current_test_info()->name() + ".jack";
    }
    void TearDown() override { std::remove(path.c_str()); }
};

// ---------------------------------------------------------------- byte layout

TEST_F(ArchiveWriterTest, EmptyArchive_Is28BytesWithEmptyFlag) {
    Archive a;
    a.header.flags = ARCHIVE_FLAG_IS_EMPTY;

    const std::vector<uint8_t> bytes = ArchiveWriter::serialize(a.header, a.frequencies, a.payload);

    const std::vector<uint8_t> expected = {
        'J', 'A', 'C', 'K', 0x01, 0x01,                  // magic, version, flags
        0, 0, 0, 0, 0, 0, 0, 0,                          // original size
        0, 0, 0, 0,                                      // CRC-32
        0, 0,                                            // symbol_count
        0, 0, 0, 0, 0, 0, 0, 0};                         // total_bits
    EXPECT_EQ(bytes.size(), 28u);
    EXPECT_EQ(bytes, expected);
}

TEST_F(ArchiveWriterTest, SmallArchive_MatchesSpecByteForByte) {
    const Archive a = makeAab();

    const std::vector<uint8_t> bytes = ArchiveWriter::serialize(a.header, a.frequencies, a.payload);

    const std::vector<uint8_t> expected = {
        'J', 'A', 'C', 'K', 0x01, 0x00,
        0, 0, 0, 0, 0, 0, 0, 3,                          // original size = 3
        0xDE, 0xAD, 0xBE, 0xEF,                          // CRC-32
        0x00, 0x02,                                      // symbol_count = 2
        'A', 0, 0, 0, 0, 0, 0, 0, 2,                     // 'A' x2
        'B', 0, 0, 0, 0, 0, 0, 0, 1,                     // 'B' x1
        0, 0, 0, 0, 0, 0, 0, 3,                          // total_bits = 3
        0x20};                                           // payload
    EXPECT_EQ(bytes, expected);
    EXPECT_EQ(bytes.size(), expectedArchiveSize(2, 3));
}

TEST_F(ArchiveWriterTest, MultiByteFields_AreBigEndian) {
    // 258 = 0x0102 so every byte position of the 64-bit fields is distinguishable.
    Archive a;
    a.header.originalSize = 258;
    a.header.checksumCRC32 = 0x01020304;
    a.header.totalBits = 258;
    a.frequencies = {{'A', 258}};
    a.payload.assign(33, 0x00);  // ceil(258 / 8)

    const std::vector<uint8_t> bytes = ArchiveWriter::serialize(a.header, a.frequencies, a.payload);

    EXPECT_EQ(bytes[0x0D], 0x02);  // original size, least significant byte last
    EXPECT_EQ(bytes[0x0C], 0x01);
    EXPECT_EQ(bytes[0x0E], 0x01);  // CRC-32 big-endian
    EXPECT_EQ(bytes[0x11], 0x04);
    EXPECT_EQ(bytes[0x12], 0x00);  // symbol_count = 1
    EXPECT_EQ(bytes[0x13], 0x01);
    EXPECT_EQ(bytes[0x14], 'A');
    EXPECT_EQ(bytes[0x1B], 0x01);  // frequency 258, big-endian (offsets 0x15-0x1C)
    EXPECT_EQ(bytes[0x1C], 0x02);
    EXPECT_EQ(bytes[0x23], 0x01);  // total_bits 258 (offsets 0x1D-0x24)
    EXPECT_EQ(bytes[0x24], 0x02);
}

TEST_F(ArchiveWriterTest, Writer_OverridesMagicAndVersionFromCaller) {
    Archive a = makeAab();
    a.header.version = 9;  // caller garbage must not leak into the archive
    std::memcpy(a.header.magic, "XXXX", 4);

    const std::vector<uint8_t> bytes = ArchiveWriter::serialize(a.header, a.frequencies, a.payload);

    EXPECT_EQ(std::string(bytes.begin(), bytes.begin() + 4), "JACK");
    EXPECT_EQ(bytes[4], 0x01);
}

// ---------------------------------------------------------------- round trips

TEST_F(ArchiveWriterTest, WriteThenRead_RestoresEveryField) {
    const Archive a = makeAab();
    ArchiveWriter::write(path, a.header, a.frequencies, a.payload);

    ArchiveHeader h{};
    FrequencyList f;
    ArchiveReader::readValidatedHeader(path, h, f);

    EXPECT_EQ(std::string(h.magic, 4), "JACK");
    EXPECT_EQ(h.version, 1);
    EXPECT_EQ(h.flags, a.header.flags);
    EXPECT_EQ(h.originalSize, a.header.originalSize);
    EXPECT_EQ(h.checksumCRC32, a.header.checksumCRC32);
    EXPECT_EQ(h.symbolCount, 2);
    EXPECT_EQ(h.totalBits, a.header.totalBits);
    EXPECT_EQ(f, a.frequencies);
}

TEST_F(ArchiveWriterTest, WriteThenRead_EmptyArchive) {
    Archive a;
    a.header.flags = ARCHIVE_FLAG_IS_EMPTY;
    ArchiveWriter::write(path, a.header, a.frequencies, a.payload);

    ArchiveHeader h{};
    FrequencyList f;
    ASSERT_NO_THROW(ArchiveReader::readValidatedHeader(path, h, f));
    EXPECT_EQ(h.originalSize, 0u);
    EXPECT_EQ(h.symbolCount, 0);
    EXPECT_TRUE(f.empty());
    EXPECT_EQ(readAll(path).size(), 28u);
}

TEST_F(ArchiveWriterTest, WriteThenRead_AllTwoHundredFiftySixSymbols) {
    Archive a;
    a.header.originalSize = 256;
    a.header.totalBits = 256 * 8;
    for (int s = 0; s < 256; ++s) {
        a.frequencies.emplace_back(static_cast<uint8_t>(s), 1);
    }
    a.payload.assign(256, 0xAB);
    ArchiveWriter::write(path, a.header, a.frequencies, a.payload);

    const std::vector<uint8_t> bytes = readAll(path);
    EXPECT_EQ(bytes[0x12], 0x01);  // symbol_count = 256 = 0x0100
    EXPECT_EQ(bytes[0x13], 0x00);
    EXPECT_EQ(bytes.size(), 18u + 2u + 256u * 9u + 8u + 256u);

    ArchiveHeader h{};
    FrequencyList f;
    ASSERT_NO_THROW(ArchiveReader::readValidatedHeader(path, h, f));
    EXPECT_EQ(h.symbolCount, 256);
    EXPECT_EQ(f, a.frequencies);
}

TEST_F(ArchiveWriterTest, UncompressedFallback_RoundTrips) {
    Archive a;
    a.header.flags = ARCHIVE_FLAG_IS_UNCOMPRESSED;
    a.header.originalSize = 3;
    a.header.checksumCRC32 = 0x11223344;
    a.header.totalBits = 24;
    a.payload = {'A', 'A', 'B'};
    ArchiveWriter::write(path, a.header, a.frequencies, a.payload);

    ArchiveHeader h{};
    FrequencyList f;
    ASSERT_NO_THROW(ArchiveReader::readValidatedHeader(path, h, f));
    EXPECT_EQ(h.flags, ARCHIVE_FLAG_IS_UNCOMPRESSED);
    EXPECT_EQ(h.symbolCount, 0);
    EXPECT_EQ(readAll(path).size(), 28u + 3u);
}

// ------------------------------------------------- writer rejects bad input

TEST_F(ArchiveWriterTest, RejectsPayloadShorterThanTotalBits) {
    Archive a = makeAab();
    a.header.totalBits = 9;  // needs 2 payload bytes, only 1 supplied
    EXPECT_THROW(ArchiveWriter::serialize(a.header, a.frequencies, a.payload), std::invalid_argument);
}

TEST_F(ArchiveWriterTest, RejectsPayloadLongerThanTotalBits) {
    Archive a = makeAab();
    a.payload.push_back(0x00);
    EXPECT_THROW(ArchiveWriter::serialize(a.header, a.frequencies, a.payload), std::invalid_argument);
}

TEST_F(ArchiveWriterTest, RejectsUnsortedSymbols) {
    Archive a = makeAab();
    std::swap(a.frequencies[0], a.frequencies[1]);
    EXPECT_THROW(ArchiveWriter::serialize(a.header, a.frequencies, a.payload), std::invalid_argument);
}

TEST_F(ArchiveWriterTest, RejectsDuplicateSymbols) {
    Archive a = makeAab();
    a.frequencies[1].first = a.frequencies[0].first;
    EXPECT_THROW(ArchiveWriter::serialize(a.header, a.frequencies, a.payload), std::invalid_argument);
}

TEST_F(ArchiveWriterTest, RejectsZeroFrequency) {
    Archive a = makeAab();
    a.frequencies[1].second = 0;
    a.header.originalSize = 2;  // keep the sum consistent so only the zero is wrong
    EXPECT_THROW(ArchiveWriter::serialize(a.header, a.frequencies, a.payload), std::invalid_argument);
}

TEST_F(ArchiveWriterTest, RejectsFrequencySumNotEqualToOriginalSize) {
    Archive a = makeAab();
    a.header.originalSize = 4;
    EXPECT_THROW(ArchiveWriter::serialize(a.header, a.frequencies, a.payload), std::invalid_argument);
}

TEST_F(ArchiveWriterTest, RejectsReservedFlagBits) {
    Archive a = makeAab();
    a.header.flags = 0x04;
    EXPECT_THROW(ArchiveWriter::serialize(a.header, a.frequencies, a.payload), std::invalid_argument);
}

TEST_F(ArchiveWriterTest, RejectsBothEmptyAndUncompressedFlags) {
    Archive a;
    a.header.flags = ARCHIVE_FLAG_IS_EMPTY | ARCHIVE_FLAG_IS_UNCOMPRESSED;
    EXPECT_THROW(ArchiveWriter::serialize(a.header, a.frequencies, a.payload), std::invalid_argument);
}

TEST_F(ArchiveWriterTest, RejectsEmptyFlagOnNonEmptyData) {
    Archive a = makeAab();
    a.header.flags = ARCHIVE_FLAG_IS_EMPTY;
    EXPECT_THROW(ArchiveWriter::serialize(a.header, a.frequencies, a.payload), std::invalid_argument);
}

TEST_F(ArchiveWriterTest, RejectsNonEmptyArchiveWithoutSymbols) {
    Archive a;
    a.header.originalSize = 5;  // no IS_EMPTY / IS_UNCOMPRESSED and no symbols
    EXPECT_THROW(ArchiveWriter::serialize(a.header, a.frequencies, a.payload), std::invalid_argument);
}

TEST_F(ArchiveWriterTest, RejectsMoreThan256Symbols) {
    Archive a;
    a.header.originalSize = 257;
    for (int i = 0; i < 257; ++i) {
        a.frequencies.emplace_back(static_cast<uint8_t>(i % 256), 1);
    }
    EXPECT_THROW(ArchiveWriter::serialize(a.header, a.frequencies, a.payload), std::invalid_argument);
}

TEST_F(ArchiveWriterTest, RejectsUncompressedFlagWithSymbolTable) {
    Archive a = makeAab();
    a.header.flags = ARCHIVE_FLAG_IS_UNCOMPRESSED;
    EXPECT_THROW(ArchiveWriter::serialize(a.header, a.frequencies, a.payload), std::invalid_argument);
}

TEST_F(ArchiveWriterTest, RejectsUncompressedBitCountMismatch) {
    Archive a;
    a.header.flags = ARCHIVE_FLAG_IS_UNCOMPRESSED;
    a.header.originalSize = 3;
    a.header.totalBits = 23;  // must be exactly originalSize * 8
    a.payload = {'A', 'A', 'B'};
    EXPECT_THROW(ArchiveWriter::serialize(a.header, a.frequencies, a.payload), std::invalid_argument);
}

TEST_F(ArchiveWriterTest, WriteToUnwritablePath_Throws) {
    const Archive a = makeAab();
    EXPECT_THROW(ArchiveWriter::write("no_such_dir_xyz/out.jack", a.header, a.frequencies, a.payload),
                 std::runtime_error);
}

// ----------------------------------- reader rejects corrupted archives on disk

class ArchiveReaderValidationTest : public ArchiveWriterTest {
protected:
    std::vector<uint8_t> goodBytes() {
        const Archive a = makeAab();
        return ArchiveWriter::serialize(a.header, a.frequencies, a.payload);
    }
    void expectRejected(const std::vector<uint8_t>& bytes) {
        writeAll(path, bytes);
        ArchiveHeader h{};
        FrequencyList f;
        EXPECT_THROW(ArchiveReader::readValidatedHeader(path, h, f), int);
    }
};

TEST_F(ArchiveReaderValidationTest, AcceptsValidArchive) {
    writeAll(path, goodBytes());
    ArchiveHeader h{};
    FrequencyList f;
    EXPECT_NO_THROW(ArchiveReader::readValidatedHeader(path, h, f));
}

TEST_F(ArchiveReaderValidationTest, RejectsTrailingBytes) {
    std::vector<uint8_t> bytes = goodBytes();
    bytes.push_back(0xFF);
    expectRejected(bytes);
}

TEST_F(ArchiveReaderValidationTest, RejectsTruncatedPayload) {
    std::vector<uint8_t> bytes = goodBytes();
    bytes.pop_back();
    expectRejected(bytes);
}

TEST_F(ArchiveReaderValidationTest, RejectsReservedFlagBitOnDisk) {
    std::vector<uint8_t> bytes = goodBytes();
    bytes[5] |= 0x80;
    expectRejected(bytes);
}

TEST_F(ArchiveReaderValidationTest, RejectsOutOfOrderSymbolsOnDisk) {
    std::vector<uint8_t> bytes = goodBytes();
    std::swap(bytes[0x14], bytes[0x14 + 9]);  // swap the two symbol bytes
    expectRejected(bytes);
}

TEST_F(ArchiveReaderValidationTest, RejectsFrequencySumMismatchOnDisk) {
    std::vector<uint8_t> bytes = goodBytes();
    bytes[13] = 0x04;  // original size 3 -> 4
    expectRejected(bytes);
}

TEST_F(ArchiveReaderValidationTest, RejectsMissingFile) {
    ArchiveHeader h{};
    FrequencyList f;
    EXPECT_THROW(ArchiveReader::readValidatedHeader("does_not_exist.jack", h, f), int);
}

TEST_F(ArchiveReaderValidationTest, RejectsNonJackFile) {
    writeAll(path, std::vector<uint8_t>(64, 'x'));
    ArchiveHeader h{};
    FrequencyList f;
    EXPECT_THROW(ArchiveReader::readValidatedHeader(path, h, f), int);
}
