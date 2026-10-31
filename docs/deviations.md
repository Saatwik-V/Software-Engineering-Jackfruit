# Project Deviations Report

**Project:** Jackfruit — C++17 Lossless Huffman File Compressor  
**Sprint:** Sprint 2 (Full Product & Hardening)  
**Date:** 29th October 2026  
**Status:** Frozen & Reconciled with SRS v0.1 and SAD v0.1  

---

## 1. Executive Summary

As required by **Part-2 Deliverables (Section 2.i & Section 3.c)**:
> *"Deviations from the submitted document should be documented and explained during the final demo."*

Following the completion and end-to-end verification of Sprint 2, the team conducted a formal audit comparing the final working software against the **Software Requirements Specification (SRS v0.1)**, **Software Architecture Document (SAD v0.1)**, and **Binary Archive Specification (`archive-format.md`)**.

**Conclusion:**  
**There are zero negative deviations or functional omissions.** All 16 Functional Requirements (`JACK-F-001` through `JACK-F-016`), 5 Non-Functional Requirements (`JACK-NF-001` through `JACK-NF-005`), and 5 Security Requirements (`JACK-SR-001` through `JACK-SR-005`) have been fully implemented, verified, and hardened.

---

## 2. Requirement Traceability Matrix (RTM) Audit

| Requirement ID | Specification Description | Final Implementation | Deviation Status |
|---|---|---|---|
| **JACK-F-001** | File input path validation & permissions check | `FileValidator::validateInputFile()` | **None (100% Conformance)** |
| **JACK-F-002** | Frequency table & Huffman tree generation | `FrequencyTable`, `HuffmanTree` | **None (100% Conformance)** |
| **JACK-F-003** | MSB-first bitstream encoding | `BitWriter` | **None (100% Conformance)** |
| **JACK-F-004** | Serialized `.jack` archive writer with 18B header | `ArchiveWriter`, `Encoder` | **None (100% Conformance)** |
| **JACK-F-005** | Header validation & magic number `0x4A41434B` | `ArchiveReader::validateHeader()` | **None (100% Conformance)** |
| **JACK-F-006** | Huffman tree reconstruction from frequencies | `HuffmanTree::reconstruct()` | **None (100% Conformance)** |
| **JACK-F-007** | MSB-first bitstream decoding | `BitReader` | **None (100% Conformance)** |
| **JACK-F-008** | Bit-for-bit lossless payload restoration | `Decoder::decode()` verified with `cmp` | **None (100% Conformance)** |
| **JACK-F-009** | Inspect archive header without decompressing | `ArchiveReader::readHeader()` | **None (100% Conformance)** |
| **JACK-F-010** | Calculate compression metrics | `StatisticsEngine::calculate()` | **None (100% Conformance)** |
| **JACK-F-011** | CLI table representation of statistics | `StatisticsEngine::printStatsTable()` | **None (100% Conformance)** |
| **JACK-F-012** | Handle corrupted archives during stat inspection | `StatisticsEngine` catches errors gracefully | **None (100% Conformance)** |
| **JACK-F-013** | Integrity check without modifying input | `IntegrityValidator::validateArchive()` | **None (100% Conformance)** |
| **JACK-F-014** | Recompute and match payload CRC-32 | `CRC32::calculate()` vs header checksum | **None (100% Conformance)** |
| **JACK-F-015** | Exit code 3 upon checksum mismatch | Verified in CLI & automated tests | **None (100% Conformance)** |
| **JACK-F-016** | Diagnostic report on archive tampering | Integrity report printed to terminal | **None (100% Conformance)** |

---

## 3. Value-Added Architectural Enhancements (Positive Additions)

While strictly adhering to the original specifications, the team incorporated the following usability and robustness enhancements:

1. **Interactive Terminal Menu Shell (`src/cli/menu.cpp`):**
   - *Original Scope:* Command-line argument parser only (`jackfruit <subcommand>`).
   - *Enhancement:* When launched without arguments (`./jackfruit`), the tool presents an interactive numeric menu (Options 1–5), allowing users unfamiliar with CLI flags to run Compression, Decompression, Statistics, and Validation interactively.
2. **Automated Continuous Fuzz Security Suite (`tests/unit/test_fuzz.cpp`):**
   - *Original Scope:* Manual edge-case tests.
   - *Enhancement:* Added 5 automated fuzz suites running in CI on every commit to test bit-flips, random binary noise, byte truncations at every offset, and integer overflow attempts.
3. **Automated Distributable Linux Release Packaging in CI:**
   - *Enhancement:* GitHub Actions automatically bundles the compiled binary, headers, and documentation into `jackfruit-v1.0-linux-x86_64.zip` on every push to `main`.

