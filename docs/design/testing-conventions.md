# Jackfruit Project: Testing Conventions & Strategy

Owner: Tanmayi (Testing & Coverage Lead)
Standard: Aligned with UE24CS341A Software Test Plan (STP) Specifications
Framework: GoogleTest (gtest) / CTest

---

## 1. Test Levels & Directory Layout

To maintain clear separation of test tiers, tests are organized into three distinct directories:

* tests/unit/: Module-level tests isolated with mocks/stubs where needed. Focuses on single classes/functions (e.g., BitReader, BitWriter, FrequencyTable, CRC32).
* tests/integration/: Cross-module interaction tests (e.g., ArchiveWriter <-> BitWriter, ArchiveReader <-> BitReader <-> HuffmanTree).
* tests/system/: End-to-end black-box CLI tests verifying full pipeline execution (compress -> decompress round trip for text, binary, 0-byte, and large payloads).

---

## 2. GoogleTest Naming Conventions

All test suites and test cases must use strict descriptive naming:

TEST(TestSuiteName, ScenarioUnderTest_ExpectedBehavior)

Examples:
* TEST(BitReaderTest, ReadBitsMSB_ReturnsCorrectSequence)
* TEST(DecoderTest, TruncatedBitstream_ThrowsUnexpectedEOF)
* TEST(ArchiveHeaderTest, CorruptMagicBytes_FailsGracefully)

---

## 3. Fixtures, Mocks & Parameterized Tests

* Test Fixtures (::testing::Test): Use for reusable setup/teardown (e.g., creating temporary test files, initializing memory buffers).
* Synthetic Streams: Unit tests for BitReader and decoder logic should run against statically defined byte arrays before full archive integration.
* Parameterized Tests (::testing::TestWithParamInterface): Used for verifying multiple file types (ASCII, UTF-8, binary ELF/EXE, empty file) against round-trip assertions.

---

## 4. Code & Branch Coverage Targets

Code coverage is monitored continuously via gcov and lcov integrated into CI:
* Line Coverage Target: >= 80% on all core algorithms (core/).
* Branch Coverage Target: >= 70% on core modules.
* Exemptions: CLI entry wrappers and terminal rendering routines are exempt from coverage gates.

---

## 5. Entry and Exit Criteria

### Entry Criteria:
* Source compiles clean without warnings (-Wall -Wextra -Werror).
* CMake targets build both production binaries and test runners.

### Exit Criteria:
* 100% automated tests passing in CI.
* Target code coverage achieved on core modules.
* Zero memory leaks or undefined behavior detected by ASan/UBSan.