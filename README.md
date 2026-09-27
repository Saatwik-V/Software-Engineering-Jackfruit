# Jackfruit — Huffman File Compressor

[![CI](https://github.com/Saatwik-V/Software-Engineering-Jackfruit/actions/workflows/ci.yml/badge.svg)](https://github.com/Saatwik-V/Software-Engineering-Jackfruit/actions/workflows/ci.yml)

> Basic ZIP-style file compression tool using Huffman coding, with an interactive CLI.

**Course:** UE24CS341A — Software Engineering  
**Project:** Jackfruit Mini-Project  
**Language:** C++17  
**Build System:** CMake (v3.14+)  
**Testing Framework:** GoogleTest  
**CI/CD:** GitHub Actions  

---

## Team Ownership & Roles

| Member | Primary use case (their functionality) | Branch | Secondary role |
|---|---|---|---|
| **Saatwik** | UC-01 Compress File | `feature/saatwik-compression` | Overall lead, architecture, main-branch integrator |
| **Tanmayi** | UC-02 Decompress File | `feature/tanmayi-decompression` | Testing & coverage lead |
| **Yuvraj** | UC-03 Inspect Compression Statistics | `feature/yuvraj-statistics` | CI/CD (GitHub Actions) lead |
| **Vibhav** | UC-04 Validate Archive Integrity | `feature/vibhav-integrity` | Security & SonarCloud lead |

*Note: Each member builds their own interactive CLI screen (prompts, options, and progress/output formatting) directly inside their respective module.*

---

## Interactive UI Overview

Jackfruit features a menu-driven interactive Command-Line Interface (CLI):
- **1. Compress File** (UC-01) — Prompts for target file, calculates frequency tables, builds Huffman tree, encodes bitstream, and generates `.jack` archive.
- **2. Decompress File** (UC-02) — Reconstructs Huffman tree from archive header, decodes bitstream, and restores original file.
- **3. Compression Statistics** (UC-03) — Inspects compressed archives, displays file sizes, compression ratio, space saved, and bit efficiency tables.
- **4. Validate Archive Integrity** (UC-04) — Computes and compares checksum/CRC to ensure archive is not corrupted or tampered.
- **5. Exit** — Safely closes the CLI.

---

## Project Structure

```
jackfruit/
├── src/
│   ├── cli/            # Interactive menu + command parsing
│   ├── compression/    # Frequency table, Huffman tree, encoder (Saatwik)
│   ├── decompression/  # Decoder, reconstruction (Tanmayi)
│   ├── archive/        # Archive header, metadata, statistics (Yuvraj)
│   ├── integrity/      # Checksum/CRC, validation (Vibhav)
│   ├── io/             # Shared file + bit-level I/O
│   └── main.cpp        # Application entrypoint
├── include/            # Public header files for modules
├── tests/
│   ├── unit/           # Unit tests (GoogleTest)
│   ├── integration/    # Cross-module integration tests
│   └── system/         # End-to-end CLI & compression tests
├── docs/
│   ├── srs/            # Software Requirements Specification
│   ├── design/         # UML diagrams & architecture specs
│   ├── validation/     # Verification & validation plans
│   ├── team-guides/    # Team onboarding & sprint guides
│   └── meeting-notes/  # Agile meeting logs
├── scripts/            # Helper scripts & automation
├── .github/workflows/  # GitHub Actions CI definitions
├── CMakeLists.txt      # Root CMake configuration
├── README.md
└── .gitignore
```

---

## Build & Test Instructions

### Prerequisites
- C++17 compatible compiler (`clang++` >= 5.0 or `g++` >= 7.0)
- CMake (>= 3.14)
- Git (for fetching GoogleTest)

### Configure & Build
```bash
# 1. Configure CMake build directory
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 2. Build the executable and test targets
cmake --build build --config Release
```

### Run Unit Tests
```bash
# Run tests using CTest
ctest --test-dir build --output-on-failure -C Release

# Or run test binary directly
./build/jackfruit_tests
```

### Run the Application
```bash
./build/jackfruit
```

---

## Documentation Links

All project documentation is tracked in the repository:
- [Software Requirements Specification (SRS)](docs/srs/)
- [Architecture & UML Design](docs/design/)
- [Verification & Validation Plan](docs/validation/)
- [Team Member Guides & Setup Checklists](docs/team-guides/)
  - [00 - Setup Guide (Saatwik)](docs/team-guides/00-SETUP-GUIDE-Saatwik.md)
  - [01 - Member Guide: Compression (Saatwik)](docs/team-guides/01-MEMBER-GUIDE-Saatwik.md)
  - [02 - Member Guide: Decompression (Tanmayi)](docs/team-guides/02-MEMBER-GUIDE-Tanmayi.md)
  - [03 - Member Guide: Statistics & CI/CD (Yuvraj)](docs/team-guides/03-MEMBER-GUIDE-Yuvraj.md)
  - [04 - Member Guide: Integrity & SonarCloud (Vibhav)](docs/team-guides/04-MEMBER-GUIDE-Vibhav.md)
  - [05 - Master Project Checklist](docs/team-guides/05-MASTER-PROJECT-CHECKLIST.md)
- [Meeting Notes](docs/meeting-notes/)