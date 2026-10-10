# Document 3 — End-to-End Guide: Yuvraj

**Use case owned:** UC-03 — Inspect Compression Statistics (archive header serialization/deserialization, ratio calculation, statistics CLI)  
**Secondary role:** CI/CD & DevOps Lead (GitHub Actions pipeline, SonarCloud integration, release packaging)  
**Your branch:** `feature/yuvraj-statistics`  
**Authority:** Aligned with `SE_Mini_Project_Delivereables_Part-1.pdf` and `SE_Mini_Project_Delivereables_Part-2.pdf`.

This is your master guide for project execution — what to build, when, in what order, and on which branch.

---

## 1. Your Module & DevOps Scope

You own the structural backbone of the archive files and the team's automated build and delivery infrastructure:
- **Archive Serialization & Parsing (`ArchiveWriter` / `ArchiveReader`):**
  - Read and write the fixed 18-byte `.jack` header (Magic `0x4A41434B`, Version `0x01`, Flags, Original Size, Compressed Size, Checksum).
  - Serialize and deserialize the symbol dictionary / frequency table metadata.
  - Parse and extract total payload bit counts.
- **Statistics Engine:** Calculate compression ratio, percentage savings, and read archive metadata without needing full decompression.
- **Statistics CLI Screen:** Interactive menu view and CLI command (`jackfruit stat <file.jack>`) showing formatted statistics tables.
- **CI/CD Pipeline Architecture:**
  - Automated GitHub Actions workflow on PRs and commits to `main`.
  - SonarCloud static analysis configuration for continuous code quality.
  - Automated release artifact packaging (producing standalone distributable zip).

---

## 2. Dependencies & Critical Path

- **Critical Path in Sprint 1:** Both Saatwik (`Encoder`) and Tanmayi (`Decoder`) depend on your `ArchiveWriter` and `ArchiveReader` to package and unpackage `.jack` archives. You must deliver these components early in Sprint 1.
- **CI Pipeline:** The whole team relies on your GitHub Actions workflow to gate PRs with automated builds and GoogleTest runs.
- **Sprint 1 Team Milestone:** Support the round-trip integration by ensuring archives are formatted strictly per `docs/design/archive-format.md`, enabling the 1-minute working demo video by October 23.

---

## 3. Official Timeline & Deliverables Plan

### Phase 1: Engineering Documents (Part-1 — Completed Baseline)
- [x] Co-author binary archive specification (`docs/design/archive-format.md` & `.pdf`).
- [x] Contribute UC-03 requirements (`JACK-F-009..012`) to IEEE SRS (`docs/srs/srs-v0.1.pdf`).
- [x] Review C++ API design for `ArchiveWriter` and `ArchiveReader` in IEEE SAD (`docs/design/architecture-v0.1.pdf`).
- [x] Verify baseline GitHub Actions CI workflow runs CTest cleanly.

---

### Phase 2: Backlog & Story Point Setup (ETA: 12th Oct 2026)
- [ ] In the linked GitHub Project board, verify your assigned backlog items:
  - Archive Serialization & Parsing (`ArchiveWriter`/`ArchiveReader`) — **5 SP**
  - `JACK-F-009` (Archive metadata header inspection) — **2 SP**
  - `JACK-F-010` (Compression ratio & space savings calculation) — **2 SP**
  - `JACK-F-011` (Interactive statistics display table) — **3 SP**
  - `JACK-F-012` (Command-line `--stat` execution) — **2 SP**
- [ ] Participate in dry-run practice week (12th–16th Oct) testing CI PR gates.
- [ ] Confirm backlog lock on 19th Oct.

---

### Phase 2: Sprint 1 — Core Working Product (19th Oct – 23rd Oct 2026)
**Goal:** Deliver `ArchiveWriter` and `ArchiveReader`, ensure CI runs green on all teammate PRs, and support the 1-minute working demo video.

| Day / Task | Implementation Target | Artifact / Code Location |
|---|---|---|
| **Day 1–2 (19–20 Oct)** | Implement `ArchiveWriter` (18-byte header + metadata serialization) | `include/archive/archive_writer.hpp`, `src/archive/archive_writer.cpp` |
| **Day 2–3 (20–21 Oct)** | Implement `ArchiveReader` (header validation + metadata deserialization) | `include/archive/archive_reader.hpp`, `src/archive/archive_reader.cpp` |
| **Day 3 (21 Oct)** | Unit tests for archive round-trip serialization and error handling on invalid magic bytes | `tests/unit/test_archive_io.cpp` |
| **Day 4 (22 Oct)** | Ensure GitHub Actions CI triggers and reports build/test status on all teammate PRs | `.github/workflows/ci.yml` |
| **Day 4 (22 Oct)** | Raise PR from `feature/yuvraj-statistics` | PR reviewed and merged into `main` |
| **Day 5 (23 Oct)** | **Support Sprint 1 Demo Video:** Validate archive headers in the working round-trip demo (1-minute video uploaded to repo) | Video demo ready |

---

### Phase 2: Sprint 2 — Full Feature Product & Development Freeze (26th Oct – 30th Oct 2026)
**Goal:** Implement Statistics Engine and CLI command, wire SonarCloud and release packaging into CI, assist 2-minute demo video, and freeze development.

| Day / Task | Implementation Target | Artifact / Code Location |
|---|---|---|
| **Day 1–2 (26–27 Oct)** | Implement `StatisticsEngine` and ratio calculation logic | `include/stats/statistics_engine.hpp`, `src/stats/statistics_engine.cpp` |
| **Day 2–3 (27–28 Oct)** | Build interactive Statistics CLI screen and `jackfruit stat` command | `src/cli/stat_view.cpp` |
| **Day 3 (28 Oct)** | Configure SonarCloud analysis workflow in GitHub Actions | `.github/workflows/sonarcloud.yml` |
| **Day 4 (29 Oct)** | Add automated release packaging step to CI (generate distributable zip artifact) | Automated workflow artifact |
| **Day 4 (29 Oct)** | Unit tests for statistics calculations (`TC-STAT-01`) | `tests/unit/test_statistics.cpp` |
| **Day 5 (30 Oct)** | **Support Sprint 2 Demo Video:** Showcase `jackfruit stat` formatting in 2-minute video | Demo video completed |
| **Day 5 (30 Oct)** | **Development Freeze:** Lock CI pipeline, verify all checks green, freeze development | Ready for final evaluation |

---

## 4. CI/CD Lead Best Practices

1. **Keep CI Gating Strict:** Ensure every PR requires passing CTest runs before merging.
2. **Big-Endian Contract:** Double-check that multi-byte integers in headers are packed using Big-Endian order as agreed in the specification.
3. **No Breaking Changes:** Never alter header field sizes or positions without alerting the entire team.
