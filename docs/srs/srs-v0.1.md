# Software Requirements Specification (SRS)

## Jackfruit — Basic ZIP-Style File Compression Tool (Huffman Coding)

**Document Version:** 0.1.0  
**Date:** September 2026  
**Course:** UE24CS341A — Software Engineering  
**Authors:**
- **Saatwik** (Lead Author — Sections 1, 2, 5.1 UC-01)
- **Tanmayi** (Contributor — Section 5.2 UC-02)
- **Yuvraj** (Contributor — Section 5.3 UC-03)
- **Vibhav** (Contributor — Section 5.4 UC-04 & Section 6.3 Security)

---

## 1. Introduction

### 1.1 Purpose
The purpose of this document is to provide a complete and detailed specification of the requirements for the **Jackfruit File Compression Tool** (Release 1.0). It defines both functional and non-functional requirements to guide development, verification, and automated testing.

### 1.2 Document Conventions
- Requirements are tagged with unique alphanumeric identifiers:
  - `FR-COMP-xxx`: Functional Requirements for UC-01 (Compress File)
  - `FR-DECOMP-xxx`: Functional Requirements for UC-02 (Decompress File)
  - `FR-STAT-xxx`: Functional Requirements for UC-03 (Inspect Statistics)
  - `FR-VALID-xxx`: Functional Requirements for UC-04 (Validate Integrity)
  - `NFR-xxx`: Non-Functional Requirements
  - `SEC-xxx`: Security Requirements
- Priority levels are defined as: **Mandatory (M)**, **Desirable (D)**, or **Optional (O)**.

### 1.3 Intended Audience and Reading Suggestions
This document is intended for:
- Course Instructors and Evaluators (for validation against project grading rubrics).
- Student Developers (Saatwik, Tanmayi, Yuvraj, Vibhav) to implement modules against agreed contracts.
- Testing and Quality Leads to write unit, integration, and manual test suites.

### 1.4 Product Scope
Jackfruit is an interactive, menu-driven CLI utility designed in C++17. It utilizes canonical Huffman coding to compress arbitrary single files (text or binary) into a `.jack` archive container and restore them losslessly. It incorporates archive header metadata inspection, compression ratio analytics, and CRC-32 integrity validation.

### 1.5 References
- IEEE Std 830-1998: IEEE Recommended Practice for Software Requirements Specifications.
- D.A. Huffman, "A Method for the Construction of Minimum-Redundancy Codes", Proceedings of the IRE, 1952.
- Jackfruit Binary Archive Format Specification: `docs/design/archive-format.md`.
- Jackfruit System Architecture & Design v0.1: `docs/design/architecture-v0.1.md`.

---

## 2. Overall Description

### 2.1 Product Perspective
Jackfruit is a standalone, self-contained desktop CLI application. It does not require network connectivity, database servers, or external third-party compression libraries (e.g., zlib, libzip). It is compiled with modern standard C++17 and CMake.

### 2.2 Product Functions
The primary functions exposed through the interactive menu are:
1. **Compress File (UC-01):** Transform any input file into a `.jack` compressed archive.
2. **Decompress File (UC-02):** Reconstruct original files byte-for-byte from `.jack` archives.
3. **Inspect Statistics (UC-03):** Parse archive headers to display compression ratio, space savings, and file size metrics.
4. **Validate Integrity (UC-04):** Verify archive magic bytes, version compatibility, and CRC-32 checksums.

### 2.3 User Classes and Characteristics
- **General End Users:** Command-line users who require an intuitive, menu-driven compression tool without complex command-line flags.
- **Academic Evaluators:** Technical reviewers evaluating architectural modularity, test coverage ($\ge 80\%$), CI pipeline automation, and requirement traceability.

### 2.4 Operating Environment
- **Operating Systems:** Linux (Ubuntu 22.04 LTS / 24.04 LTS), macOS (Darwin ARM64 / x86_64).
- **Compilers:** Clang++ ($\ge 10.0$) or GCC/G++ ($\ge 9.0$) supporting ISO C++17.
- **Build System:** CMake ($\ge 3.14$).

### 2.5 Design and Implementation Constraints
- **Language Standard:** C++17 is mandatory.
- **Zero External Compression Dependencies:** All Huffman tree, bitstream I/O, and CRC algorithms must be written natively.
- **Interactive UI Requirement:** The application must present an interactive numbered menu; command-line flags alone do not satisfy project requirements.
- **Archive Format Invariant:** All binary multi-byte integers must be written in Big-Endian order to guarantee cross-architecture portability.

### 2.6 Assumptions and Dependencies
- The target system has sufficient write permissions in the directory specified for output files.
- The system has sufficient storage to house the output archive or decompressed file.

---

## 3. External Interface Requirements

### 3.1 User Interface
The tool executes in a standard terminal and provides a numbered menu on launch:
```text
==================================================
        JACKFRUIT HUFFMAN COMPRESSION TOOL        
==================================================
  1. Compress File
  2. Decompress File
  3. Inspect Compression Statistics
  4. Validate Archive Integrity
  5. Exit
--------------------------------------------------
Enter choice [1-5]: 
```
Each option interactively prompts the user for required paths, displays live progress during processing, and renders color-coded summary tables upon completion.

---

## 4. System Features Overview

| Feature ID | Feature Name | Primary Owner | Module Directory |
|---|---|---|---|
| **FEAT-01** | Lossless Huffman File Compression | Saatwik | `src/compression/` |
| **FEAT-02** | Exact Byte-for-Byte Decompression | Tanmayi | `src/decompression/` |
| **FEAT-03** | Archive Metadata & Analytics | Yuvraj | `src/archive/` |
| **FEAT-04** | CRC-32 Validation & Integrity Check | Vibhav | `src/integrity/` |
| **FEAT-05** | Shared Bit-Level Stream Processing | Saatwik / Tanmayi | `src/io/` |

---

## 5. Functional Requirements

### 5.1 Use Case 01: Compress File (Owner: Saatwik)

- **`FR-COMP-001` (File Input & Validation) [Priority: M]**  
  The system shall prompt the user for an input file path. If the file does not exist, is a directory, or cannot be opened for reading, the system shall display an error message and return to the main menu without crashing.

- **`FR-COMP-002` (Frequency Analysis) [Priority: M]**  
  The system shall perform a single pass over the input file to count the frequencies of all 256 possible byte values ($0$ to $255$) using an internal 64-bit frequency table.

- **`FR-COMP-003` (Huffman Tree Construction) [Priority: M]**  
  The system shall construct a minimal-redundancy Huffman binary tree using a priority queue (min-heap) based on the symbol frequencies. In the event of equal frequencies, ties shall be resolved deterministically by comparing the lowest symbol value.

- **`FR-COMP-004` (Canonical Prefix Code Generation) [Priority: M]**  
  The system shall traverse the Huffman tree to generate unique variable-length binary prefix codes for all symbols present in the file. No prefix code shall be a prefix of any other code.

- **`FR-COMP-005` (Empty and Single-Symbol Edge Cases) [Priority: M]**  
  - If the input file is 0 bytes, the system shall generate a valid 28-byte `.jack` archive marked with flag `IS_EMPTY = 1`.
  - If the input file contains only one distinct symbol repeated $N$ times, the system shall assign a trivial 1-bit code (`0`) to that symbol.

- **`FR-COMP-006` (Bitstream Encoding via BitWriter) [Priority: M]**  
  The system shall encode the file bytes into a continuous bitstream packed **MSB-first** into 8-bit bytes. Upon stream completion, remaining bits ($1 \text{ to } 7$) shall be flushed to disk with zero-padding.

- **`FR-COMP-007` (Archive Serialization Integration) [Priority: M]**  
  The compression module shall invoke `ArchiveWriter` to write the fixed header (`JACK` magic bytes, version 1, original size, CRC-32) and the active symbol frequency table before writing the compressed payload.

- **`FR-COMP-008` (Progress & Results Display) [Priority: M]**  
  During compression, the system shall display an interactive progress percentage. Upon completion, it shall display: original size, compressed size, compression ratio, space savings percentage, and elapsed execution time.

---

### 5.2 Use Case 02: Decompress File (Owner: Tanmayi)
*(To be detailed by Tanmayi in Sprint 1 PR via `feature/tanmayi-decompression`)*

- **`FR-DECOMP-001` (Archive Header Validation) [Priority: M]**  
  The system shall verify magic bytes and version before attempting decompression.
- **`FR-DECOMP-002` (Tree Reconstruction) [Priority: M]**  
  The system shall reconstruct the exact decoding tree from the archive's symbol frequency table.
- **`FR-DECOMP-003` (Bitstream Decoding via BitReader) [Priority: M]**  
  The system shall read bits MSB-first and terminate exactly after `total_bits` are read.
- **`FR-DECOMP-004` (Byte-for-Byte Restoration) [Priority: M]**  
  Decompressed output shall match the original file byte-for-byte.
- **`FR-DECOMP-005` (CLI Decompression Feedback) [Priority: M]**  
  Prompt for input archive and destination path; report success/failure status.

---

### 5.3 Use Case 03: Inspect Compression Statistics (Owner: Yuvraj)
*(To be detailed by Yuvraj in Sprint 1 PR via `feature/yuvraj-statistics`)*

- **`FR-STAT-001` (Fast Header Inspection) [Priority: M]**  
  The system shall extract archive metadata without decompressing the payload.
- **`FR-STAT-002` (Metric Calculations) [Priority: M]**  
  Calculate original size, compressed size, compression ratio ($\text{orig} / \text{comp}$), and space saved ($\%$).
- **`FR-STAT-003` (Formatted CLI Output Table) [Priority: M]**  
  Render a clean, aligned tabular view of archive metrics in the terminal.

---

### 5.4 Use Case 04: Validate Archive Integrity (Owner: Vibhav)
*(To be detailed by Vibhav in Sprint 1 PR via `feature/vibhav-integrity`)*

- **`FR-VALID-001` (Magic & Version Integrity) [Priority: M]**  
  Validate that magic bytes equal `JACK` and version equals `1`.
- **`FR-VALID-002` (Checksum Verification) [Priority: M]**  
  Calculate the CRC-32 checksum of decompressed data and compare against the stored header checksum.
- **`FR-VALID-003` (Structural Size Check) [Priority: M]**  
  Ensure file size matches header field calculations; reject truncated archives.

---

## 6. Non-Functional Requirements

### 6.1 Performance Requirements
- **`NFR-PERF-001` (Throughput):** Compression throughput shall exceed 5 MB/sec on standard desktop hardware for typical text/binary inputs.
- **`NFR-PERF-002` (Memory Footprint):** Memory consumption during compression and decompression shall not exceed 64 MB regardless of file size (streaming processing).

### 6.2 Reliability & Fault Tolerance
- **`NFR-REL-001` (No Undefined Behavior):** Malformed, truncated, or corrupted archive files shall never result in segmentation faults, memory corruption, or infinite loops.
- **`NFR-REL-002` (Clean Aborts):** In case of read/write errors, partial temporary files shall be cleanly removed or reported.

### 6.3 Security Requirements (Owner: Vibhav)
*(To be expanded by Vibhav in Sprint 1 PR)*
- **`SEC-001` (Path Traversal Prevention):** Input and output file paths must be sanitized to prevent directory traversal attacks (e.g., preventing unauthorized overwrite via `../../`).
- **`SEC-002` (Buffer Overflow Prevention):** Header fields such as `symbol_count` must be strictly bounded ($0 \le N \le 256$) before allocating memory.
- **`SEC-003` (Safe Integer Arithmetic):** Bit and byte offset calculations shall be checked against 64-bit integer overflow.

### 6.4 Maintainability & Quality
- **`NFR-QUAL-001` (Unit Test Coverage):** Automated unit test coverage shall achieve $\ge 80\%$ line coverage on core algorithms.
- **`NFR-QUAL-002` (Static Analysis):** The project shall pass SonarCloud quality gates with zero unresolved critical security or code-smell issues.
