<div align="center">

# 🌿 JACKFRUIT
### High-Performance, Lossless Huffman File Compression Tool
**Modern C++17 • Memory-Safe • Dual-Interface (Interactive Shell & UNIX CLI) • Built-in CRC-32 Integrity Engine**

[![CI Pipeline](https://github.com/Saatwik-V/Software-Engineering-Jackfruit/actions/workflows/ci.yml/badge.svg)](https://github.com/Saatwik-V/Software-Engineering-Jackfruit/actions/workflows/ci.yml)
[![SonarCloud Quality Gate](https://sonarcloud.io/api/project_badges/measure?project=Saatwik-V_Software-Engineering-Jackfruit&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=Saatwik-V_Software-Engineering-Jackfruit)
[![SonarCloud Coverage](https://sonarcloud.io/api/project_badges/measure?project=Saatwik-V_Software-Engineering-Jackfruit&metric=coverage)](https://sonarcloud.io/summary/new_code?id=Saatwik-V_Software-Engineering-Jackfruit)
[![SonarCloud Security](https://sonarcloud.io/api/project_badges/measure?project=Saatwik-V_Software-Engineering-Jackfruit&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=Saatwik-V_Software-Engineering-Jackfruit)
[![SonarCloud Reliability](https://sonarcloud.io/api/project_badges/measure?project=Saatwik-V_Software-Engineering-Jackfruit&metric=reliability_rating)](https://sonarcloud.io/summary/new_code?id=Saatwik-V_Software-Engineering-Jackfruit)
[![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg?logo=c%2B%2B)](https://en.cppreference.com/w/cpp/17)
[![Tests](https://img.shields.io/badge/GoogleTest-131%20Passed%20(100%25)-brightgreen.svg)](tests/)
[![Sanitizers](https://img.shields.io/badge/Sanitizers-ASan%20%7C%20UBSan%20Clean-success.svg)](.github/workflows/ci.yml)
[![Release](https://img.shields.io/badge/Release-v0.2.0--sprint--2-purple.svg)](https://github.com/Saatwik-V/Software-Engineering-Jackfruit/releases/tag/v0.2.0-sprint-2)

<p align="center">
  <a href="#-quick-start">Quick Start</a> •
  <a href="#-features--capabilities">Features</a> •
  <a href="#-interactive-cli--terminal-usage">CLI Usage</a> •
  <a href="#-binary-archive-format-specification-jack">Archive Format</a> •
  <a href="#-performance--quality-metrics">Benchmarks</a> •
  <a href="#-video-demonstrations">Video Demos</a> •
  <a href="#-documentation-suite">Documentation</a> •
  <a href="#-team-ownership--roles">Team</a>
</p>

</div>

---

## 📌 Executive Summary

**Jackfruit** is a robust, production-grade file compression utility engineered in **Modern C++17**. It implements the **canonical Huffman coding algorithm** to provide bit-for-bit lossless data compression wrapped in a custom, hardened binary container format (`.jack`). 

Jackfruit delivers a **dual-mode interface**: an intuitive, interactive terminal shell for guided workflows and a scriptable UNIX-style CLI for automation pipelines. Engineered following strict Software Engineering standards (IEEE SRS, SAD, STP), Jackfruit guarantees resilience against bit flips and malformed inputs through hardware-accelerated **IEEE 802.3 CRC-32 integrity validation**, full **AddressSanitizer (ASan)** / **UndefinedBehaviorSanitizer (UBSan)** memory safety, and an automated continuous CI/CD matrix achieving **87.1% test coverage** and a **SonarCloud Grade A Quality Gate**.

---

## 🚀 Features & Capabilities

- **100% Bit-for-Bit Lossless Reconstruction:** Reconstructs uncompressed byte streams with 0-byte divergence verified by binary comparison (`cmp` / `fc.exe /b`).
- **Canonical Prefix Coding:** Builds optimal prefix trees with deterministic symbol tie-breaking, ensuring identical tree regeneration on any architecture.
- **MSB-First Bitstream Engine:** Custom `BitWriter` and `BitReader` stream bit-level variable-length codes with sub-byte padding management.
- **Compact Custom Archive Container (`.jack`):** 18-byte big-endian fixed header with sparse symbol frequency serialization for zero bloat on small payloads.
- **In-Place Header Observability (UC-03):** Inspects compression metrics, ratio, bits-per-symbol, and checksums directly from archive headers without decompressing.
- **Hardware-Grade Data Integrity & Tamper Detection (UC-04):** Computes and verifies 32-bit CRC polynomials (`0xEDB88320`) to flag corrupted or truncated bitstreams with diagnostic exit codes.
- **Dual User Experience:**
  - **Interactive Terminal Shell:** Numbered, human-friendly TUI with colorized diagnostic reports.
  - **Scriptable POSIX CLI:** Direct subcommands (`compress`, `decompress`, `stat`, `validate`) designed for shell scripting and CI automation.
- **Military-Grade Robustness:** Hardened with an automated 5-suite continuous malformed fuzzing engine protecting against integer overflows, buffer truncations, and malicious frequency maps.

---

## 👥 Team Ownership & Roles

Jackfruit was engineered by a team of 4 software engineers under **Course UE24CS341A (Software Engineering)**:

| Member | Primary Use Case Owned | Secondary Leadership Role | Key Modules Owned |
|---|---|---|---|
| **Saatwik** | **UC-01: Compress File** | **Project Lead & Architecture** | `FrequencyTable`, `HuffmanTree`, `BitWriter`, `Encoder`, Interactive Menu Shell |
| **Tanmayi** | **UC-02: Decompress File** | **Testing & Quality Lead** | `BitReader`, `Decoder`, GoogleTest Suite, Automated Coverage (`gcov`/`lcov`) |
| **Yuvraj** | **UC-03: Inspect Statistics** | **DevOps & CI/CD Lead** | `ArchiveWriter`, `ArchiveReader`, `StatisticsEngine`, SonarCloud, Linux Release Packaging |
| **Vibhav** | **UC-04: Validate Integrity** | **Security & Robustness Lead** | `CRC32`, `IntegrityValidator`, ASan/UBSan Sanitizers, Continuous Malformed Fuzz Suite |

---

## 🎬 Video Demonstrations

Both official video milestones are tracked and directly viewable in the repository:

| Deliverable | Duration | Video File Link | Highlights Demonstrated |
|---|:---:|---|---|
| **Sprint 2: Full Product & Hardening** | **2:00 Min** | 🎥 [`docs/media/sprint-2-demo.mp4`](docs/media/sprint-2-demo.mp4) | Full interactive CLI menu (Options 1–5), 1.63x compression, header statistics, CRC-32 validation, 1-byte tamper injection, `diff -s` bit-identity proof, fuzz test suite, clean Git status. |
| **Sprint 1: Core Working Product** | **1:00 Min** | 🎥 [`docs/media/sprint-1-demo.mp4`](docs/media/sprint-1-demo.mp4) | Core round-trip compression and decompression, CMake build, CTest runner, and `cmp` binary equality verification. |

---

## ⚡ Quick Start

### Prerequisites
- **Compiler:** Modern C++17 compiler (`clang++` $\ge 5.0$, `g++` $\ge 7.0$, or MSVC $\ge 2019$)
- **Build System:** CMake $\ge 3.14$
- **Tools:** Git, Make / Ninja

### 1. Clone & Build
```bash
# Clone the repository
git clone https://github.com/Saatwik-V/Software-Engineering-Jackfruit.git
cd Software-Engineering-Jackfruit

# Configure & build in Release mode (parallelized)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

### 2. Run All Automated Tests (131 Tests)
```bash
# Run unit, integration, and fuzz test suites via CTest
ctest --test-dir build --output-on-failure
```

### 3. Run Memory Sanitizers (ASan & UBSan)
```bash
# Build with AddressSanitizer and UndefinedBehaviorSanitizer enabled
cmake -B build-sanitizers -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON
cmake --build build-sanitizers --parallel
ctest --test-dir build-sanitizers --output-on-failure
```

---

## 💻 Interactive CLI & Terminal Usage

Jackfruit can be operated interactively or via POSIX command-line arguments:

```
Jackfruit — Lossless Huffman File Compressor (v0.1.0)

Usage:
  jackfruit                             Launch interactive terminal menu
  jackfruit compress <in> [-o <out>]    Compress an input file into .jack archive
  jackfruit decompress <in> [-o <out>]  Decompress a .jack archive back to original
  jackfruit stat <archive.jack>         Show archive statistics without decompressing
  jackfruit validate <archive.jack>     Validate archive integrity and CRC-32 checksum
  jackfruit --help | -h                 Display help message
```

### 1. Interactive Terminal Menu Shell
Launch without flags to enter the menu:
```bash
./build/jackfruit
```
```text
============================================================
         JACKFRUIT — Huffman File Compressor (v0.1.0)       
               Interactive Terminal Menu Shell              
============================================================
  [1] Compress File
  [2] Decompress Archive (.jack)
  [3] View Archive Statistics
  [4] Validate Archive Integrity
  [5] Exit
------------------------------------------------------------
Select an option (1-5): 
```

### 2. Scriptable CLI Subcommands

#### Compress a File (UC-01)
```bash
./build/jackfruit compress document.txt -o document.jack
```
```text
[*] Compressing 'document.txt' -> 'document.jack'...
[+] Compression successful!
    Original size:    3015 bytes
    Archive size:     1848 bytes
    Checksum (CRC32): 0xAF2E7776
    Compression ratio: 1.63x
    Space saved:      38.71%
    Elapsed time:     0.40 ms
```

#### Inspect Statistics Without Decompressing (UC-03)
```bash
./build/jackfruit stat document.jack
```
```text
====================================================
  Archive Statistics
  File: document.jack
====================================================
  Original Size                 2 KB  (3015 B)
  Compressed Size               1 KB  (1848 B)
  Compression Ratio             1.63x
  Space Saved                   38.7%
  Unique Symbols                24
  Total Encoded Bits            12825
  Avg Bits / Symbol             4.254
  CRC-32 Checksum               0xAF2E7776
====================================================
```

#### Validate Integrity & Detect Tampering (UC-04)
```bash
# Valid authentic archive
./build/jackfruit validate document.jack
```
```text
============================================================
            Archive Integrity Verification Report           
============================================================
[+] Integrity Status:   PASS (VALID)
    Exit Code:          0
    Payload Verified:   3015 bytes
    Stored CRC-32:      0xAF2E7776
    Computed CRC-32:    0xAF2E7776
    Checksum Match:     YES (Identical)
------------------------------------------------------------
[+] Archive integrity verified successfully (PASS).
============================================================
```

#### Decompress an Archive (UC-02)
```bash
./build/jackfruit decompress document.jack -o restored.txt
diff -s document.txt restored.txt
# Output: Files document.txt and restored.txt are identical
```

---

## 📦 Binary Archive Format Specification (`.jack`)

Jackfruit packages compressed data inside an efficient, endianness-safe binary format:

```text
+-----------------------------------------------------------------------------------+
|                            JACKFRUIT ARCHIVE CONTAINER                            |
+-------------------+------------------+-------------------+------------------------+
| Bytes 0x00 - 0x03 | Byte 0x04        | Byte 0x05         | Bytes 0x06 - 0x09      |
| Magic: "JACK"     | Version (0x01)   | Reserved (0x00)   | Original Size (uint32) |
+-------------------+------------------+-------------------+------------------------+
| Bytes 0x0A - 0x0D | Bytes 0x0E - 0x11| Bytes 0x12 - 0x13 | Bytes 0x14+ ...        |
| Total Encoded Bits| CRC-32 Checksum  | Symbol Count (N)  | Frequency Dictionary   |
+-------------------+------------------+-------------------+------------------------+
| PAYLOAD BITSTREAM (MSB-First Packed Huffman Variable-Length Codes)                 |
+-----------------------------------------------------------------------------------+
```

- **Fixed Header (18 Bytes):** Always starts with magic bytes `0x4A41434B` (`JACK`), 1-byte version (`0x01`), 32-bit big-endian original payload length, 32-bit big-endian encoded bit count, and 32-bit CRC-32 checksum.
- **Sparse Frequency Dictionary:** Stores only characters that appear in the input ($N \le 256$ tuples of `[uint8 symbol, uint32 frequency]`), preventing wasteful table padding.
- **Payload Bitstream:** Streamed bit-by-bit MSB-first. Extra padding bits in the final byte are safely ignored using the declared `Total Encoded Bits` boundary.

---

## 📊 Performance & Quality Metrics

Verified on a 5.0 MB benchmark dataset across Linux and macOS architectures:

| Metric | Measured Baseline | Target Criteria | Conformance |
|---|---|---|:---:|
| **Compression Throughput** | **132 ms** (for 5.0 MB payload) | $< 2000\text{ ms}$ (`JACK-NF-001`) | **15x Faster** |
| **Peak Memory Footprint (RSS)** | **~8.4 MB** | $< 50\text{ MB}$ (`JACK-NF-002`) | **6x Better** |
| **Restoration Fidelity** | **100% Bit-for-Bit Identity** | 0 Differences (`diff -s` / `cmp`) | **Lossless** |
| **Automated Test Coverage** | **87.1% Line / 100% Function** | $\ge 80\%$ Line Coverage | **Exceeded** |
| **Total Automated Tests** | **131 Tests (Unit + Integration + Fuzz)** | All Passing | **100% Pass** |
| **SonarCloud Quality Gate** | **Grade A (Reliability, Security, Maintainability)** | Zero Vulnerabilities | **Passed (`OK`)** |
| **Sanitizers (ASan & UBSan)** | **0 Leaks, 0 Boundary Errors, 0 UB** | Zero Sanitizer Errors | **100% Clean** |

---

## 📂 Project Architecture

```
Software-Engineering-Jackfruit/
├── include/                     # Public modular headers
│   ├── archive/                 # ArchiveWriter, ArchiveReader, Header
│   ├── cli/                     # Menu shell & command dispatcher
│   ├── compression/             # FrequencyTable, HuffmanTree, BitWriter, Encoder, Compressor
│   ├── decompression/           # Decoder, BitReader
│   ├── io/                      # FileValidator, BitStreams
│   ├── security/                # CRC32, IntegrityValidator
│   └── stats/                   # StatisticsEngine
├── src/                         # Implementation files (.cpp)
│   ├── main.cpp                 # Application entrypoint
│   └── ...                      # Module implementations
├── tests/                       # Comprehensive GoogleTest suite (131 tests)
│   ├── unit/                    # Isolated component unit tests & test_fuzz.cpp
│   ├── integration/             # Cross-module roundtrip tests (test_roundtrip.cpp)
│   └── system/                  # End-to-end CLI & error tests (test_cli_e2e.cpp)
├── docs/                        # Formal IEEE Documentation & Engineering Reports
│   ├── srs/                     # IEEE SRS (srs-v0.1.pdf)
│   ├── design/                  # IEEE SAD Architecture (architecture-v0.1.pdf) & Archive Spec
│   ├── validation/              # IEEE Test Plan (test-plan-v0.1.pdf), Test Results, Security Review
│   ├── team-guides/             # Master Project Checklist & Member Guides
│   ├── meeting-notes/           # Agile Backlog & Video Recording Scripts
│   ├── media/                   # Official Video Demonstrations (sprint-1-demo.mp4, sprint-2-demo.mp4)
│   └── deviations.md            # Requirement Traceability & Deviations Report
├── .github/workflows/           # Automated CI/CD (ci.yml, sonarcloud.yml)
├── CMakeLists.txt               # Build configuration with sanitizer flags
├── sonar-project.properties     # SonarCloud configuration
└── README.md                    # Project documentation
```

---

## 📚 Documentation Suite

All formal engineering artifacts are versioned directly in the repository:

| Document | Format | Description |
|---|:---:|---|
| **Software Requirements Specification (SRS)** | [PDF](docs/srs/srs-v0.1.pdf) • [MD](docs/srs/srs-v0.1.md) | IEEE format, 16 FRs, 5 NFRs, 5 Security Reqs, UML Diagrams, RTM |
| **Software Architecture Document (SAD)** | [PDF](docs/design/architecture-v0.1.pdf) • [MD](docs/design/architecture-v0.1.md) | Component architecture, STRIDE threat model, UML sequence diagrams |
| **Binary Archive Specification** | [PDF](docs/design/archive-format.pdf) • [MD](docs/design/archive-format.md) | Bitstream specification & cross-module binary contracts |
| **Software Test Plan (STP)** | [PDF](docs/validation/test-plan-v0.1.pdf) • [MD](docs/validation/test-plan-v0.1.md) | IEEE 829 format, 10 baseline test cases, entry/exit criteria |
| **Test Execution Records** | [MD](docs/validation/test-results.md) | Executed test logs & verification matrix across 131 tests |
| **Security & Sanitizer Review** | [MD](docs/validation/security-review.md) | ASan/UBSan report & malformed archive fuzz analysis |
| **Performance Benchmark** | [MD](docs/validation/performance-baseline.md) | 5MB dataset runtime benchmarks and memory RSS audit |
| **Deviations Report** | [MD](docs/deviations.md) | RTM audit proving 100% specification conformance |
| **Master Project Checklist** | [MD](docs/team-guides/05-MASTER-PROJECT-CHECKLIST.md) | Complete sprint tracking & milestone sign-offs |

---

## 🔒 License & Academic Integrity

This project was developed for **UE24CS341A: Software Engineering** at PES University. It is distributed for educational and academic review under the MIT License.
