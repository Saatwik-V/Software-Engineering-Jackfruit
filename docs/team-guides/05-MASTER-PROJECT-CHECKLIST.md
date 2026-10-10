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
- [ ] **Software Test Plan (STP):** IEEE 829 format, 10 baseline test cases (`TC-COMP-01..03`, `TC-DECOMP-01..03`, `TC-STAT-01`, `TC-VALID-01`, `TC-PERF-01`, `TC-SEC-01`), 2-sprint schedule, compiled to `docs/validation/test-plan-v0.1.pdf` (Tanmayi — in progress via feedback guide)

---

### Phase 2: Backlog & Story Point Setup (ETA: 12th Oct 2026)
- [ ] **GitHub Project Linked:** Project board created and linked to `Software-Engineering-Jackfruit` repository.
- [ ] **Issue Creation from SRS:** All functional requirements (`JACK-F-001` through `016`), NFRs (`JACK-NF-001..005`), and Security requirements (`JACK-SR-001..005`) entered as GitHub issues with acceptance criteria.
- [ ] **Story Point Field Created:** Custom field **Story Points** enabled in GitHub Projects using Fibonacci sequence (1, 2, 3, 5, 8).
- [ ] **Story Points Assigned:**
  - Saatwik (UC-01): `JACK-F-001` (3 SP), `JACK-F-002` (5 SP), `JACK-F-003` (5 SP), `JACK-F-004` (3 SP).
  - Tanmayi (UC-02): `JACK-F-005` (3 SP), `JACK-F-006` (5 SP), `JACK-F-007` (3 SP), `JACK-F-008` (5 SP).
  - Yuvraj (UC-03 / Archive): Archive Serialization/Deserialization (5 SP), `JACK-F-009` (2 SP), `JACK-F-010` (2 SP), `JACK-F-011` (3 SP), `JACK-F-012` (2 SP).
  - Vibhav (UC-04 / Security): CRC32 Engine (3 SP), `JACK-F-013` (2 SP), `JACK-F-014` (3 SP), `JACK-F-015` (2 SP), `JACK-F-016` (3 SP).
- [ ] **Sprint Allocation:** Stories mapped into Sprint-1 (19–23 Oct) and Sprint-2 (26–30 Oct) milestone views.
- [ ] **Dry-Run Week (12th – 16th Oct):** Team tests PR creation, review flow, and branch merges.
- [ ] **Backlog Final Lock (19th Oct):** Final review and grooming of backlog before Sprint 1 kickoff.

---

### Phase 2: Sprint 1 — Core Working Product (19th Oct – 23rd Oct 2026)
**Sprint Goal:** Produce a working end-to-end product (compress a file, decompress it, prove 100% byte-identical restoration) with automated CI and record a 1-minute video demo.

#### Saatwik Tasks (UC-01 & Architecture):
- [ ] Implement `FrequencyTable` (`src/compression/frequency_table.cpp`) and unit tests.
- [ ] Implement `HuffmanTree` builder & canonical code generator (`src/compression/huffman_tree.cpp`).
- [ ] Implement `BitWriter` (`src/io/bit_writer.cpp`) following MSB-first packing contract.
- [ ] Implement `Encoder` module (`src/compression/encoder.cpp`).
- [ ] Raise PR from `feature/saatwik-compression` with automated unit tests.

#### Tanmayi Tasks (UC-02 & Testing):
- [ ] Implement `BitReader` (`src/io/bit_reader.cpp`) handling bit-boundary padding.
- [ ] Implement `Decoder` module (`src/decompression/decoder.cpp`) for Huffman tree decoding.
- [ ] Scaffold unit tests in `tests/unit/test_bit_reader.cpp` and `test_decoder.cpp`.
- [ ] Pair with Saatwik to verify **byte-identical round-trip restoration** (`cmp file output`).
- [ ] Raise PR from `feature/tanmayi-decompression`.

#### Yuvraj Tasks (Archive IO & CI/CD):
- [ ] Implement `ArchiveWriter` (`src/archive/archive_writer.cpp`) for 18-byte header & metadata serialization.
- [ ] Implement `ArchiveReader` (`src/archive/archive_reader.cpp`) for header validation & deserialization.
- [ ] Ensure GitHub Actions CI automates build and CTest execution on every PR.
- [ ] Raise PR from `feature/yuvraj-statistics`.

#### Vibhav Tasks (Security Core):
- [ ] Implement standalone `CRC32` calculation (`src/security/crc32.cpp`) with standard IEEE 802.3 polynomial (`0xEDB88320`).
- [ ] Add unit tests verifying known CRC-32 test vectors (`tests/unit/test_crc32.cpp`).
- [ ] Raise PR from `feature/vibhav-integrity`.

#### Sprint 1 Milestone Deliverables:
- [ ] All 4 Sprint 1 PRs reviewed and merged into `main`.
- [ ] CLI command `jackfruit compress input.txt -o out.jack` and `jackfruit decompress out.jack -o restored.txt` working end-to-end.
- [ ] `diff input.txt restored.txt` returns zero differences.
- [ ] **Record 1-Minute Video Demo:** Demonstrate build, CLI compression, CLI decompression, and `cmp` verification. Upload video to `docs/media/sprint-1-demo.mp4` (or linked per course guidelines).

---

### Phase 2: Sprint 2 — Full Feature Product & Development Freeze (26th Oct – 30th Oct 2026)
**Sprint Goal:** Complete all 4 use cases, interactive CLI menu, coverage enforcement (≥80%), SonarCloud analysis, security hardening, final 2-minute video demo, and freeze development.

#### Saatwik Tasks (CLI & Performance):
- [ ] Build interactive CLI terminal navigation menu (`src/cli/menu.cpp`).
- [ ] Measure and record performance benchmark against 5MB dataset (`JACK-NF-001`).
- [ ] Integrate all 4 module flows into unified `jackfruit` executable.
- [ ] Final architecture and RTM reconciliation.

#### Tanmayi Tasks (Coverage & Test Execution):
- [ ] Wire `gcov`/`lcov` coverage reporting into GitHub Actions CI pipeline.
- [ ] Expand integration and system test suites (`tests/integration/`, `tests/system/`).
- [ ] Ensure codebase achieves **≥ 80% line coverage** and **≥ 70% branch coverage**.
- [ ] Execute 10 manual test cases (`TC-COMP`, `TC-DECOMP`, `TC-STAT`, `TC-VALID`, `TC-PERF`, `TC-SEC`) and document actual results.

#### Yuvraj Tasks (UC-03 Statistics & Packaging):
- [ ] Implement `StatisticsEngine` (`src/stats/statistics_engine.cpp`) for original size, compressed size, and ratio calculations.
- [ ] Wire `jackfruit stat <archive.jack>` command and display screen.
- [ ] Configure SonarCloud analysis workflow in GitHub Actions.
- [ ] Create automated release packaging step in CI (distributable zip artifact).

#### Vibhav Tasks (UC-04 Integrity & Security Hardening):
- [ ] Implement `IntegrityValidator` (`src/security/integrity_validator.cpp`) for header checks and payload CRC-32 verification.
- [ ] Wire `jackfruit validate <archive.jack>` command (returns exit code 0 on valid, 3 on corruption).
- [ ] Set up AddressSanitizer (ASan) and UndefinedBehaviorSanitizer (UBSan) in CI pipeline.
- [ ] Execute malformed archive fuzz test suite and document findings in `docs/validation/security-review.md`.

#### Sprint 2 Milestone Deliverables:
- [ ] All 4 use cases functioning seamlessly from the interactive CLI menu.
- [ ] SonarCloud quality gate passing with zero critical vulnerabilities.
- [ ] CI pipeline completely green with automated test coverage report.
- [ ] **Record 2-Minute Video Demo:** Comprehensive walk-through of Compression, Statistics, Integrity Validation, and Decompression with error handling. Upload video to `docs/media/sprint-2-demo.mp4`.
- [ ] Document any deviations from SRS/SAD in `docs/deviations.md`.
- [ ] **Development Freeze (30th Oct 2026):** Codebase locked, ready for final presentation and evaluation.

---

## 4. Final Deliverable & Presentation Readiness

- [ ] **GitHub Repository Evidence:** Balanced commit graph across all 4 contributors, merged PRs with review threads, green CI runs.
- [ ] **Documentation Suite:**
  - `docs/srs/srs-v0.1.pdf` (SRS)
  - `docs/design/architecture-v0.1.pdf` (SAD)
  - `docs/validation/test-plan-v0.1.pdf` (Test Plan)
  - `docs/validation/test-results.md` (Executed test records)
  - `docs/validation/security-review.md` (Security & sanitizer findings)
- [ ] **Video Demos:** Sprint 1 (1 min) and Sprint 2 (2 min) videos present and accessible in the repository.
- [ ] **Demo Preparation:** All 4 members prepared to explain their primary module, secondary lead area, and neighbor integration live.
