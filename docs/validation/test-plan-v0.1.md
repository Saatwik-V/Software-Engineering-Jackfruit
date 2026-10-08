# Software Test Plan (STP) — Jackfruit Compression System

**Project:** Jackfruit File Compression & Decompression Tool  
**Version:** 1.0  
**Author:** Tanmayi (Testing & Coverage Lead)  
**Date:** October 2026  
**Status:** Approved  

---

## 1. Introduction
* **Purpose:** This document defines the testing strategy, verification framework, test environment, and quality thresholds for the Jackfruit Huffman Compression system.
* **Scope:** Covers unit, integration, and end-to-end system testing for core modules: bitwise I/O, canonical/min-heap Huffman tree reconstruction, binary archive parsing, decompression byte restoration, CRC-32 integrity validation, and CLI execution.
* **References:** UE24CS341A Course Guidelines, Jackfruit SRS v0.1, Jackfruit Architecture SAD v0.1, Binary Archive Format Specification.
* **Definitions:** STP (Software Test Plan), SRS (Software Requirements Specification), RTM (Requirements Traceability Matrix), MSB (Most Significant Bit), CRC (Cyclic Redundancy Check).

## 2. Test Items
* BitReader / BitWriter bitwise streaming modules
* Frequency table generator & Huffman tree serializer/deserializer
* Archive header parsing & validation module (ArchiveReader)
* Decompression engine (bitstream parsing -> byte stream reconstruction)
* CRC-32 IEEE 802.3 checksum calculation and payload verifier
* Interactive CLI interface

## 3. Features to be Tested
Mapped directly to project requirements:
* JACK-F-001 to JACK-F-004: File compression, tree generation, and archive creation
* JACK-F-005: Archive header parsing and magic byte verification
* JACK-F-006: Huffman tree reconstruction from serialized frequency/code data
* JACK-F-007: BitReader sequential reading up to exact total_bits boundary
* JACK-F-008: Byte-identical file restoration verified via hash comparison
* JACK-F-009 to JACK-F-012: Archive statistics extraction and metrics calculation
* JACK-F-013 to JACK-F-016: Archive integrity checks and CRC-32 mismatch detection
* JACK-NF-001: Sub-second execution for standard payloads (< 10 MB)

## 4. Features Not to be Tested
* Third-party OS filesystem kernel drivers
* Third-party compression schemes (DEFLATE / LZMA / BZIP2)
* Hardware disk failures or physical media corruption

## 5. Test Approach / Strategy
* **Unit Testing:** Module-level isolated logic written using GoogleTest (gtest).
* **Integration Testing:** Interaction between BitReader, ArchiveReader, and the Huffman decoding tree.
* **System Testing:** Black-box end-to-end tests comparing original files against decompressed copies (cmp / SHA-256 byte-level verification) on empty, text, binary, and large inputs.
* **Coverage Standards:** Minimum 80% line coverage and 70% branch coverage collected via gcov/lcov.
* **Security & Robustness:** Address Sanitizer (ASan) and Undefined Behavior Sanitizer (UBSan) enabled on all automated CI test runs.
* **Entry Criteria:** Clean compilation with zero compiler warnings (-Wall -Wextra -Werror).
* **Exit Criteria:** 100% automated test pass rate, target test coverage achieved, and 0 memory leaks.

### 5.1 Security & Robustness Validation
* Fuzz testing against malformed and truncated archive headers.
* Verification of safe error exits upon CRC-32 checksum failures without buffer overflows.
* Path traversal sanitization on output filenames.

## 6. Test Environment
* **Platform:** Linux (Ubuntu 22.04 LTS / CI Runner) and Windows 11.
* **Toolchain:** CMake (>= 3.20), GCC / Clang / MSVC with C++17 support.
* **Frameworks:** GoogleTest (v1.14+), CTest, lcov/gcov.

## 7. Test Schedule
* **Sprint 1:** Test Plan definition, GoogleTest conventions, test environment scaffolding.
* **Sprint 2:** BitReader and isolated module unit tests with CI coverage reporting.
* **Sprint 3–4:** Decoder integration tests, round-trip compression/decompression verification.
* **Sprint 5–8:** Full manual test suites (10 cases per module), malformed input hardening, and final RTM sign-off.

## 8. Test Deliverables
* Software Test Plan (this document)
* GoogleTest suites in tests/unit/, tests/integration/, and tests/system/
* Automated test execution logs from GitHub Actions CI
* HTML line and branch coverage reports (lcov)
* 40 manual test execution records populated with Actual Results and Pass/Fail status

## 9. Roles and Responsibilities
| Role | Name | Responsibility |
|---|---|---|
| Testing & Coverage Lead | Tanmayi | Test Plan architecture, GoogleTest framework, CI coverage monitoring, UC-02 test execution |
| Architecture & Integration Lead | Saatwik | Core compression tests, round-trip validation, CI integration |
| CI/CD & Statistics Lead | Yuvraj | GitHub Actions test pipeline, metrics computation test cases |
| Security & Integrity Lead | Vibhav | CRC-32 tests, corrupted input fuzzing, ASan/UBSan sanitizer checks |

## 10. Risks and Mitigation
* **Risk:** Mismatch between bit-writing order and bit-reading order breaking decompression.  
  *Mitigation:* Formal sign-off on the MSB-first contract in the Binary Archive Specification before Sprint 2 implementation.
* **Risk:** Incomplete test coverage on edge cases.  
  *Mitigation:* CI gated checks enforcing 80% line coverage threshold.

## 11. Assumptions & Dependencies
* Standard C++17 library support on target execution environments.
* Continuous integration runners have access to CMake and GoogleTest packages.

## 12. Suspension & Resumption Criteria
* **Suspension:** CI build failures or severe memory faults crashing the test runner.
* **Resumption:** Fixing the breaking commit on the local feature branch prior to merging.

## 13. Test Case Management & Traceability
All functional requirements (JACK-F-001 through JACK-F-016) map directly to test cases in the Requirement Traceability Matrix (RTM).

## 14. Test Metrics & Reporting
* Percentage of test cases executed and passed.
* Line coverage percentage (target >= 80%) and branch coverage (target >= 70%).
* Defect density categorized by module.

## 15. Approvals
| Role | Name | Signature / Date |
|---|---|---|
| Testing Lead | Tanmayi | Approved — Sprint 1 |
| Project Lead | Saatwik | Approved — Sprint 1 |