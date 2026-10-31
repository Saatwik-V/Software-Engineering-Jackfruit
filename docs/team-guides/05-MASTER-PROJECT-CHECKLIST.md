# Document 5 — Master Project Checklist

**Project:** Jackfruit — C++17 Huffman File Compression Tool with Interactive CLI  
**Team:** Saatwik (Lead / Architecture / UC-01), Tanmayi (Testing / UC-02), Yuvraj (CI/CD / UC-03), Vibhav (Security / UC-04)  
**Course:** UE24CS341A — Software Engineering  
**Authority:** Aligned with `SE_Mini_Project_Delivereables_Part-1.pdf` and `SE_Mini_Project_Delivereables_Part-2.pdf`.

This is the single source of truth for where the whole team stands at any point. Update it at each milestone and sprint review.

---

## 0. Document Suite Structure

| Doc | For | Purpose |
|---|---|---|
| `00-SETUP-GUIDE-Saatwik.md` | Saatwik only | Day-0 repo, CMake, GTest, CI skeleton, branch setup |
| `01-MEMBER-GUIDE-Saatwik.md` | Saatwik | Lead responsibilities, UC-01 Compress, integration, demo |
| `02-MEMBER-GUIDE-Tanmayi.md` | Tanmayi | Testing lead, UC-02 Decompress, GTest, coverage (gcov/lcov) |
| `03-MEMBER-GUIDE-Yuvraj.md` | Yuvraj | CI/CD lead, Archive format parsing/writing, UC-03 Statistics, SonarCloud |
| `04-MEMBER-GUIDE-Vibhav.md` | Vibhav | Security lead, UC-04 Integrity/CRC-32, ASan/UBSan, malformed input testing |
| `05-MASTER-PROJECT-CHECKLIST.md` | Whole Team | This file — Phase milestones, Sprint checklists, deliverable tracking |

---

## 1. Official Course Evaluation Model & Milestones

The project is governed by a 2-Part evaluation structure:

| Phase | Milestone / Deliverable | Deadline / Window | Core Requirements & Artifacts |
|---|---|---|---|
| **Part-1** | **Engineering Design Documents** | **28th Sept 2026** (Baseline) | • IEEE SRS (`docs/srs/srs-v0.1.pdf`)<br>• IEEE SAD / Architecture (`docs/design/architecture-v0.1.pdf`)<br>• IEEE Test Plan with 7–10 concrete test cases (`docs/validation/test-plan-v0.1.pdf`) |
| **Part-2 (Setup)** | **GitHub Backlog & Story Points** | **12th Oct 2026** | • GitHub Project board linked to repo<br>• SRS FRs/NFRs converted to GitHub issues<br>• Story Points (Fibonacci) assigned to all issues<br>• Issues assigned to team members<br>• 12–16 Oct: Dry-run practice week; 19 Oct: Backlog lock |
| **Part-2 (Sprint 1)** | **Core Working Product** | **19th Oct – 23rd Oct 2026** | • Working core product (Compress + Decompress round-trip)<br>• Automated CI on PRs/commits via GitHub Actions<br>• Peer-reviewed PRs merged into `main`<br>• **1-minute video demo of working product** uploaded to repo |
| **Part-2 (Sprint 2)** | **Full Product & Hardening** | **26th Oct – 30th Oct 2026** | • Complete feature set (Statistics, Integrity check, interactive CLI)<br>• 80% test coverage, SonarCloud analysis, ASan/UBSan clean<br>• **2-minute video demo of complete product** uploaded to repo<br>• Documentation finalized & **Development Freeze (30th Oct)** |

---

## 2. Team Ownership & Feature Allocation

| Member | Use Case Owned | GitHub Feature Branch | Secondary Lead Role | Key Modules Owned |
|---|---|---|---|---|
| **Saatwik** | **UC-01: Compress File** | `feature/saatwik-compression` | Project Lead, Architecture, Integrator | `FrequencyTable`, `HuffmanTree`, `BitWriter`, `Encoder`, CLI framework |
| **Tanmayi** | **UC-02: Decompress File** | `feature/tanmayi-decompression` | Testing & Coverage Lead | `BitReader`, `Decoder`, byte reconstruction, GoogleTest suite, coverage |
| **Yuvraj** | **UC-03: Inspect Statistics** | `feature/yuvraj-statistics` | CI/CD & DevOps Lead | `ArchiveWriter`, `ArchiveReader`, `StatisticsEngine`, GitHub Actions, SonarCloud |
| **Vibhav** | **UC-04: Validate Integrity** | `feature/vibhav-integrity` | Security & Robustness Lead | `CRC32`, structural integrity validator, ASan/UBSan sanitizers, fuzz testing |

**Universal Rules:**
1. No direct commits to `main`.
2. Every change must go through a feature branch (`feature/<name>-<module>`), require a Pull Request, pass GitHub Actions CI, and receive at least 1 peer approval before merge.

---

## 3. Phase & Sprint Execution Checklists

### Phase 1: Documentation Deliverables (Part-1 — ETA: 28th Sept 2026)
- [x] **Repository Skeleton & Build System:** C++17 CMake configuration, GoogleTest integration, GitHub Actions CI workflow (Saatwik)
- [x] **Binary Archive Specification:** `.jack` format specified, MSB-first packing agreed and signed off (`docs/design/archive-format.md`, `archive-format.pdf`)
- [x] **Software Requirements Specification (SRS):** IEEE format, 16 FRs (`JACK-F-001..016`), 5 NFRs (`JACK-NF-001..005`), 5 Security Reqs (`JACK-SR-001..005`), 2 UML Use-Case diagrams, RTM (`docs/srs/srs-v0.1.pdf`) (Saatwik leads, team contributed)
- [x] **Software Architecture & Design (SAD):** IEEE format, Layered architecture, Component diagram, STRIDE threat model, 2 UML Sequence diagrams, C++ API contracts (`docs/design/architecture-v0.1.pdf`) (Saatwik)
- [x] **Testing Conventions:** GoogleTest naming and directory hierarchy (`docs/design/testing-conventions.md`) (Tanmayi)
- [x] **Software Test Plan (STP):** IEEE 829 format, 10 baseline test cases (`TC-COMP-01..03`, `TC-DECOMP-01..03`, `TC-STAT-01`, `TC-VALID-01`, `TC-PERF-01`, `TC-SEC-01`), 2-sprint schedule, compiled to `docs/validation/test-plan-v0.1.pdf` (Tanmayi + Team)

---

### Phase 2: Backlog & Story Point Setup (ETA: 12th Oct 2026)
- [x] **GitHub Project Linked:** Project board `Jackfruit Huffman Compressor` (Project 3) created and linked to `Software-Engineering-Jackfruit` repository.
- [x] **Custom Fields & Views Configured:**
  - `Story Points` field enabled using Fibonacci sequence (`1, 2, 3, 5, 8`).
  - `Sprint` field configured (`Sprint 1`, `Sprint 2`, `Backlog`).
  - Sprint views configured: `Sprint 1(19-23 Oct)`, `Sprint 2 (26–30 Oct)`.
- [x] **Saatwik Backlog Issues Created:** Issues #18, #19, #20, #21 assigned on project board with story points and Sprint 1 mapping.
- [x] **Backlog Setup Guides Generated for Teammates:**
  - `docs/meeting-notes/backlog-guide-tanmayi.md` (6 issues, 24 SP)
  - `docs/meeting-notes/backlog-guide-yuvraj.md` (6 issues, 22 SP)
  - `docs/meeting-notes/backlog-guide-vibhav.md` (6 issues, 20 SP)
- [x] **Teammates Populate Backlog Items on Project Board:**
  - **Saatwik (UC-01 & Architecture):** Issues `#18`, `#19`, `#20`, `#21` (Sprint 1: 16 SP) + Sprint 2 CLI (5 SP) = 21 SP.
  - **Tanmayi (UC-02 & Testing):** Issues `#29`, `#30`, `#31`, `#32` (Sprint 1: 16 SP) + Sprint 2 Testing (8 SP) = 24 SP.
  - **Yuvraj (UC-03 & CI/CD):** Issues `#23`, `#24`, `#25` (Sprint 1: 11 SP) + Sprint 2 Stats/Release (11 SP) = 22 SP.
  - **Vibhav (UC-04 & Security):** Issues `#35`, `#36` (Sprint 1: 5 SP) + Sprint 2 Validation/Hardening (15 SP) = 20 SP.
  *(Total Project Backlog: 23 issues, 87 Story Points; Sprint 1: 13 issues, 48 Story Points)*
- [x] **Dry-Run Week (12th – 16th Oct):** Team tested branch models, issue templates, and CI gates.
- [x] **Backlog Final Lock (19th Oct):** Sprint 1 Backlog formally locked with 13 issues ready for execution.

---

### Phase 2: Sprint 1 — Core Working Product (19th Oct – 23rd Oct 2026)
**Sprint Goal:** Produce a working end-to-end product (compress a file, decompress it, prove 100% byte-identical restoration) with automated CI and record a 1-minute video demo.

#### Saatwik Tasks (UC-01 & Architecture):
- [x] **Issue #21:** Implement file input path reading, validation & permissions check (`JACK-F-001`).
- [x] **Issue #18:** Implement `FrequencyTable` (`src/compression/frequency_table.cpp`) & `HuffmanTree` builder (`src/compression/huffman_tree.cpp`) (`JACK-F-002`, `JACK-F-003`).
- [x] **Issue #19:** Implement `BitWriter` (`src/io/bit_writer.cpp`) following MSB-first packing contract (`JACK-F-003`).
- [x] **Issue #20:** Implement `Encoder` module (`src/compression/encoder.cpp`) & integrate with Yuvraj's `ArchiveWriter` (`JACK-F-004`).
- [x] Raise PR from `feat/saatwik-compressor` with passing automated unit tests [Merged in PR #46].

#### Tanmayi Tasks (UC-02 & Testing):
- [x] **Issue #29:** Implement archive header parsing & validation (`JACK-F-005`, `JACK-SR-001`) [Merged in PR #42].
- [x] **Issue #30:** Implement Huffman tree reconstruction from archive metadata (`JACK-F-006`) [Merged in PR #48].
- [x] **Issue #31:** Implement `BitReader` (`src/io/bit_reader.cpp`) handling bit-boundary padding (`JACK-F-007`) [Merged in PR #42].
- [x] **Issue #32:** Implement `Decoder` module (`src/decompression/decoder.cpp`) for byte-identical file restoration (`JACK-F-008`) [Merged in PR #48].
- [x] Pair with Saatwik to verify **byte-identical round-trip restoration** (`cmp file output`).
- [x] Raise PR for remaining decompression tasks [Merged in PR #48].

#### Yuvraj Tasks (Archive IO & CI/CD):
- [x] **Issue #23:** Implement `ArchiveWriter` (`src/archive/archive_writer.cpp`) for 18-byte header & metadata serialization (`JACK-F-004`) [Merged in PR #43].
- [x] **Issue #24:** Implement `ArchiveReader` (`src/archive/archive_reader.cpp`) for header validation & deserialization (`JACK-F-005`) [Merged in PR #43].
- [x] **Issue #25:** Ensure GitHub Actions CI automates build and CTest execution on every PR [Merged in PR #43].
- [x] Raise PR from `feature/yuvraj-statistics` [Merged in PR #43].

#### Vibhav Tasks (Security Core):
- [x] **Issue #35:** Implement standalone `CRC32` calculation (`src/security/crc32.cpp`) with standard IEEE 802.3 polynomial (`0xEDB88320`) [Merged in PR #45].
- [x] **Issue #36:** Integrate CRC-32 calculation into compression pipeline and verify header checksums [Merged in PR #47].
- [x] Add unit tests verifying known CRC-32 test vectors (`tests/unit/test_crc32.cpp`) [Merged in PR #45 & #47].
- [x] Raise PR from `feature/vibhav-crc-verify` [Merged in PR #47].

#### Sprint 1 Milestone Deliverables:
- [x] All 4 Sprint 1 PRs reviewed and merged into `main`.
- [x] CLI command `jackfruit compress input.txt -o out.jack` and `jackfruit decompress out.jack -o restored.txt` working end-to-end.
- [x] `diff input.txt restored.txt` returns zero differences.
- [x] **Record 1-Minute Video Demo:** Demonstrate build, CLI compression, CLI decompression, and `cmp` verification. Upload video to `docs/media/sprint-1-demo.mp4` (or linked per course guidelines).

---

### Phase 2: Sprint 2 — Full Feature Product & Development Freeze (26th Oct – 30th Oct 2026)
**Sprint Goal:** Complete all 4 use cases, interactive CLI menu, coverage enforcement (≥80%), SonarCloud analysis, security hardening, final 2-minute video demo, and freeze development.

#### Saatwik Tasks (CLI & Performance):
- [x] Build interactive CLI terminal navigation menu (`src/cli/menu.cpp`).
- [x] Measure and record performance benchmark against 5MB dataset (`JACK-NF-001`).
- [x] Integrate all 4 module flows into unified `jackfruit` executable (`src/main.cpp`).
- [x] Final architecture and RTM reconciliation (`docs/validation/performance-baseline.md`).

#### Tanmayi Tasks (Coverage & Test Execution):
- [x] Wire `gcov`/`lcov` coverage reporting into GitHub Actions CI pipeline.
- [x] Expand integration and system test suites (`tests/integration/`, `tests/system/`).
- [x] Ensure codebase achieves **≥ 80% line coverage** and **≥ 70% branch coverage** (Achieved: 87.1% Line, 100% Function).
- [x] Execute 10 manual test cases (`TC-COMP`, `TC-DECOMP`, `TC-STAT`, `TC-VALID`, `TC-PERF`, `TC-SEC`) and document actual results (`docs/validation/test-results.md`).

#### Yuvraj Tasks (UC-03 Statistics & Packaging):
- [x] Implement `StatisticsEngine` (`src/stats/statistics_engine.cpp`) for original size, compressed size, and ratio calculations [Merged in PR #56].
- [x] Wire `jackfruit stat <archive.jack>` command and display screen [Merged in PR #56].
- [x] Configure SonarCloud analysis workflow in GitHub Actions (`.github/workflows/sonarcloud.yml`) [Merged in PR #56].
- [x] Create automated release packaging step in CI (distributable zip artifact: `jackfruit-v1.0-linux-x86_64.zip`) [Merged in PR #56].

#### Vibhav Tasks (UC-04 Integrity & Security Hardening):
- [x] Implement `IntegrityValidator` (`src/security/integrity_validator.cpp`) for header checks and payload CRC-32 verification [Merged in PR #52].
- [x] Wire `jackfruit validate <archive.jack>` command (returns exit code 0 on valid, 3 on corruption) [Merged in PR #52].
- [x] Set up AddressSanitizer (ASan) and UndefinedBehaviorSanitizer (UBSan) in CI pipeline [Merged in PR #57].
- [x] Execute malformed archive fuzz test suite and document findings in `docs/validation/security-review.md` [Merged in PR #57].

#### Sprint 2 Milestone Deliverables:
- [x] All 4 use cases functioning seamlessly from the interactive CLI menu.
- [x] SonarCloud quality gate passing with zero critical vulnerabilities (A Rating, Quality Gate PASSED).
- [x] CI pipeline completely green with automated test coverage report (Run ID 41228069996).
- [x] **Record 2-Minute Video Demo:** Comprehensive walk-through of Compression, Statistics, Integrity Validation, and Decompression with error handling. Upload video to `docs/media/sprint-2-demo.mp4` [Pushed in commit 5b890bf].
- [x] Document any deviations from SRS/SAD in `docs/deviations.md`.
- [x] **Development Freeze (30th Oct 2026):** Codebase locked, ready for final presentation and evaluation.

---

## 4. Final Deliverable & Presentation Readiness

- [x] **GitHub Repository Evidence:** Balanced commit graph across all 4 contributors, merged PRs with review threads, green CI runs.
- [x] **Documentation Suite:**
  - `docs/srs/srs-v0.1.pdf` (SRS)
  - `docs/design/architecture-v0.1.pdf` (SAD)
  - `docs/validation/test-plan-v0.1.pdf` (Test Plan)
  - `docs/validation/test-results.md` (Executed test records)
  - `docs/validation/security-review.md` (Security & sanitizer findings)
  - `docs/deviations.md` (Deviations report)
- [x] **Video Demos:** Sprint 1 (1 min: `docs/media/sprint-1-demo.mp4`) and Sprint 2 (2 min: `docs/media/sprint-2-demo.mp4`) present and accessible in the repository.
- [x] **Demo Preparation:** All 4 members prepared to explain their primary module, secondary lead area, and neighbor integration live.
