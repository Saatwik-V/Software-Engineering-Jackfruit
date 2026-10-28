# Software Test Results Report (STR) — Sprint 2

**Project:** Jackfruit Lossless Huffman File Compressor  
**Document ID:** `STR-v0.2`  
**Associated Test Plan:** `docs/validation/test-plan-v0.1.md`  
**Author / Testing Lead:** Tanmayi (Testing & Decompression Lead)  
**Reviewer / Project Lead:** Saatwik (Lead / Architecture)  
**Sprint:** Sprint 2 (Phase 2 Freeze)  
**Date:** 28 October 2026  
**Execution Environment:** macOS ARM64 / Ubuntu Linux x86_64 (CI) — Clang C++17 / GCC 14  
**Overall Status:** **10 / 10 Test Cases PASSED (100% Pass Rate)**

---

## 1. Executive Summary

This report documents the actual execution results of the 10 baseline manual and automated test cases defined in the Software Test Plan (STP Section 13). Testing covered functional compression (`TC-COMP`), decompression fidelity (`TC-DECOMP`), statistics extraction (`TC-STAT`), integrity verification (`TC-VALID`), performance benchmarking (`TC-PERF`), and security/boundary defense (`TC-SEC`).

| Total Test Cases Defined | Executed | Passed | Failed | Blocked | Pass Rate |
|---|---|---|---|---|---|
| **10** | **10** | **10** | **0** | **0** | **100%** |

---

## 2. Test Execution Results Log

| Test Case ID | Target Requirement | Description | Type | Execution Method | Actual Outcome | Status |
|---|---|---|---|---|---|---|
| **TC-COMP-01** | `JACK-F-001`, `JACK-F-004` | Standard ASCII English text compression | Functional (Positive) | CLI / `Compressor` | Magic bytes `JACK` written; valid header; ratio > 1.0x on redundant text | **PASS** |
| **TC-COMP-02** | `JACK-F-002`, `JACK-F-003` | Single distinct symbol repeated (`"ZZZ..."` 20k times) | Edge Case | CLI / Unit / Integration | Single-node prefix tree handled cleanly; restores byte-identical | **PASS** |
| **TC-COMP-03** | `JACK-NF-003` | 0-byte empty file compression | Edge Case | CLI / Integration | Zero payload bits written; restores 0-byte file without division-by-zero | **PASS** |
| **TC-DECOMP-01** | `JACK-F-008` | Full round-trip restoration fidelity | Functional (Positive) | Integration Test / `cmp` | Restored file is 100% byte-identical (`diff` / `cmp` return 0 differences) | **PASS** |
| **TC-DECOMP-02** | `JACK-F-005`, `JACK-SR-001` | Header magic byte corruption (`0x4A` $\rightarrow$ `0xFF`) | Negative / Security | Integration / CLI | Program rejects file immediately with invalid format error; zero memory corruption | **PASS** |
| **TC-DECOMP-03** | `JACK-F-007` | Non-byte-aligned boundary bit reading (e.g. 13 bits) | Edge Case | Unit Test (`BitReader`) | Exactly 13 bits extracted; 14th read returns EOF; 3 padding bits ignored | **PASS** |
| **TC-STAT-01** | `JACK-F-009`, `JACK-F-010`, `JACK-F-012` | Archive statistics extraction & display | Usability / Reporting | CLI Menu Option 3 / API | Correctly displays original size, archive size, ratio, and CRC-32 checksum | **PASS** |
| **TC-VALID-01** | `JACK-F-014`, `JACK-SR-002` | 1-bit payload corruption detection | Integrity / Negative | Integration / API | Checksum mismatch detected; validation aborts gracefully | **PASS** |
| **TC-PERF-01** | `JACK-NF-001` | 5 MB dataset throughput & sub-second execution | Performance | Release Binary Benchmark | Compression in **132.18 ms** ($< 1.0\text{ s}$), throughput **37.88 MB/s**, RSS ~8.4 MB | **PASS** |
| **TC-SEC-01** | `JACK-SR-003`, `JACK-SR-005` | Truncated payload / unexpected EOF handling | Security / Hardening | System Test / ASan | Gracefully reports unexpected EOF; zero buffer overruns; no memory leaks | **PASS** |

---

## 3. Detailed Execution Logs & Evidence

### 3.1 TC-COMP-01 & TC-DECOMP-01: Round-Trip Fidelity
```bash
$ ./build/jackfruit compress test_input.txt -o test_archive.jack
[*] Compressing 'test_input.txt' -> 'test_archive.jack'...
[+] Compression successful!
    Original size:    5242880 bytes
    Archive size:     3277187 bytes
    Checksum (CRC32): 0x03F3BC90
    Compression ratio: 1.60x
    Space saved:      37.49%
    Elapsed time:     132.18 ms

$ ./build/jackfruit decompress test_archive.jack -o test_restored.txt
[*] Decompressing 'test_archive.jack' -> 'test_restored.txt'...
[+] Decompression successful! Restored to 'test_restored.txt'.

$ cmp test_input.txt test_restored.txt
$ echo $?
0
```

### 3.2 TC-COMP-02 & TC-COMP-03: Edge Case Datasets
* **Single Symbol (`TC-COMP-02`):** Validated in `RoundTripIntegrationTest.SingleSymbolRepeated_TC_COMP_02`. Single-node tree generates 1-bit code `0`, encodes 20,000 characters, and decompresses with 0 byte differences.
* **Empty File (`TC-COMP-03`):** Validated in `RoundTripIntegrationTest.EmptyFile_TC_COMP_03`. Archive created with `flags = 0x01` (`EMPTY_FILE`), restored size = 0 bytes, zero arithmetic exceptions.

### 3.3 TC-DECOMP-02 & TC-SEC-01: Fault Injection & Security Defenses
* **Corrupted Magic (`TC-DECOMP-02`):** Validated in `SystemCliE2ETest.DecompressCorruptMagicArchiveFailsGracefully_TC_DECOMP_02`. Magic `0x42 0x41 0x44 0x21` (`"BAD!"`) immediately rejected by `ArchiveReader::readValidatedHeader` without partial writes.
* **Truncated Bitstream (`TC-SEC-01`):** Validated in `SystemCliE2ETest.DecompressTruncatedArchiveFailsGracefully_TC_SEC_01`. File truncated at header boundary; `BitReader` returns EOF when bit budget cannot be fulfilled, preventing infinite loops or out-of-bounds reads.

---

## 4. Automated Test Suite Metrics

* **GoogleTest Suite (`jackfruit_tests`):** **104 / 104 Tests Passing (100%)**
  * `Unit Tests:` 92 test cases
  * `Integration Tests (test_roundtrip.cpp):` 7 test cases
  * `System E2E Tests (test_cli_e2e.cpp):` 5 test cases
* **Execution Duration:** 0.26 seconds total test runtime.

---

## 5. Sign-Off & Approvals

| Role | Name | Status | Date |
|---|---|---|---|
| **Testing Lead** | Tanmayi | Verified & Approved | 28 Oct 2026 |
| **Project Lead** | Saatwik | Verified & Approved | 28 Oct 2026 |
