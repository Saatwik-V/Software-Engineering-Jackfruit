# Document 1 — End-to-End Guide: Saatwik

**Use case owned:** UC-01 — Compress File (frequency table → Huffman tree → encoder → archive write)  
**Secondary role:** Overall Lead, Architecture Owner, Main-Branch Integrator, Code-Review Lead  
**Your branch:** `feature/saatwik-compression`  
**Authority:** Aligned with `SE_Mini_Project_Delivereables_Part-1.pdf` and `SE_Mini_Project_Delivereables_Part-2.pdf`.

This is your master guide for project execution — what to build, when, in what order, and on which branch.

---

## 1. Your Module & Architectural Scope

You own everything needed to analyze and transform raw input bytes into an optimal compressed bitstream:
- **Frequency Table Construction:** Count 8-bit symbol occurrences (`0x00`–`0xFF`) across the input stream.
- **Huffman Tree Builder & Code Generator:** Build min-heap priority queue, construct optimal binary prefix tree, and generate variable-length bitcodes.
- **BitWriter Utility:** Bit-level accumulator packing bits MSB-first into bytes, byte-boundary padding, and file flushing (shared utility; Tanmayi mirrors this with `BitReader`).
- **Core Encoder:** Map file bytes to variable-length bit sequences.
- **CLI Framework & Compress Screen:** Interactive prompt for input/output paths, progress indication, and compression ratio reporting.
- **Project Leadership & Integration:** Review all teammate PRs, enforce branch protection, oversee GitHub Backlog with Story Points, and coordinate the 1-minute (Sprint 1) and 2-minute (Sprint 2) video demos.

---

## 2. Dependencies & Critical Path

- **Independent Start:** Your core Huffman algorithm (`FrequencyTable`, `HuffmanTree`, `BitWriter`) has zero external dependencies and can be built immediately.
- **Collaboration with Yuvraj:** You call into Yuvraj's `ArchiveWriter` (`src/archive/archive_writer.cpp`) to prepend the 18-byte header and serialized tree metadata before writing the compressed bitstream.
- **Handshake with Tanmayi:** Tanmayi's `BitReader` must mirror your `BitWriter`'s MSB-first bit packing convention (formally documented in `docs/design/archive-format.md`).
- **Sprint 1 Team Milestone:** Your encoder + Yuvraj's archive writer + Tanmayi's decoder must achieve a working round-trip compression/decompression by October 23 for the Sprint 1 demo video.

---

## 3. Official Timeline & Deliverables Plan

### Phase 1: Engineering Documents (Part-1 — Completed Baseline)
- [x] Initial C++17 build system with CMake, FetchContent GoogleTest, and basic GitHub Actions CI.
- [x] Author IEEE Software Architecture & Design Document (`docs/design/architecture-v0.1.pdf`): Component diagram, Layered architecture, STRIDE threat model, 2 UML Sequence diagrams, and C++ API interfaces.
- [x] Co-author IEEE Software Requirements Specification (`docs/srs/srs-v0.1.pdf`): Introduction, Overall Description, UC-01 requirements (`JACK-F-001..004`), 5 NFRs (`JACK-NF-001..005`), 2 UML Use-Case diagrams, and RTM.
- [x] Co-author binary specification (`docs/design/archive-format.md` & `.pdf`).
- [x] Review and approve Tanmayi's testing conventions and initial Test Plan.

---

### Phase 2: Backlog & Story Point Setup (ETA: 12th Oct 2026)
- [ ] Link GitHub Project board to repository.
- [ ] Create backlog issues from SRS for all requirements (`JACK-F-001..016`, `JACK-NF-001..005`, `JACK-SR-001..005`).
- [ ] Add custom field **Story Points** (Fibonacci: 1, 2, 3, 5, 8).
- [ ] Assign your UC-01 backlog items:
  - `JACK-F-001` (Input file reading & validation) — **3 SP**
  - `JACK-F-002` (Frequency analysis table) — **5 SP**
  - `JACK-F-003` (Huffman tree & code generation) — **5 SP**
  - `JACK-F-004` (Bitstream encoding & archive generation) — **3 SP**
  - `JACK-NF-001` (Performance sub-second execution) — **3 SP**
- [ ] Set up Sprint-1 (19–23 Oct) and Sprint-2 (26–30 Oct) project views.
- [ ] Lead dry-run practice week (12th–16th Oct) and freeze backlog on 19th Oct.

---

### Phase 2: Sprint 1 — Core Working Product (19th Oct – 23rd Oct 2026)
**Goal:** Deliver a working end-to-end compression engine, integrate with Tanmayi & Yuvraj for round-trip verification, and record the 1-minute working demo video.

| Day / Task | Implementation Target | Artifact / Code Location |
|---|---|---|
| **Day 1–2 (19–20 Oct)** | Build `FrequencyTable` and `HuffmanTree` builder | `include/compression/frequency_table.hpp`, `src/compression/frequency_table.cpp`, `src/compression/huffman_tree.cpp` |
| **Day 2–3 (20–21 Oct)** | Build `BitWriter` with MSB-first packing | `include/io/bit_writer.hpp`, `src/io/bit_writer.cpp` |
| **Day 3 (21 Oct)** | Implement `Encoder` and connect with Yuvraj's `ArchiveWriter` | `include/compression/encoder.hpp`, `src/compression/encoder.cpp` |
| **Day 4 (22 Oct)** | Unit test coverage for all compression components | `tests/unit/test_frequency_table.cpp`, `tests/unit/test_huffman_tree.cpp`, `tests/unit/test_bit_writer.cpp` |
| **Day 4 (22 Oct)** | Raise PR from `feature/saatwik-compression` | PR with passing CI, get review from Tanmayi & Yuvraj |
| **Day 5 (23 Oct)** | **Integration & Round-Trip Milestone:** Pair with Tanmayi to verify `compress` → `decompress` produces byte-identical output | `cmp sample.txt restored.txt` returns 0 differences |
| **Day 5 (23 Oct)** | **Record 1-Minute Video Demo:** Record working product CLI demo, upload to repository (`docs/media/sprint-1-demo.mp4`) | Video demo requirement fulfilled |

---

### Phase 2: Sprint 2 — Full Feature Product & Development Freeze (26th Oct – 30th Oct 2026)
**Goal:** Complete interactive CLI menu, measure performance benchmarks, resolve code quality checks, record 2-minute demo video, and freeze development.

| Day / Task | Implementation Target | Artifact / Code Location |
|---|---|---|
| **Day 1–2 (26–27 Oct)** | Build interactive CLI terminal navigation menu | `include/cli/menu.hpp`, `src/cli/menu.cpp`, `src/main.cpp` |
| **Day 2–3 (27–28 Oct)** | Performance benchmarking on 5MB sample file (`JACK-NF-001`) | `docs/validation/performance-baseline.md` |
| **Day 3–4 (28–29 Oct)** | Code hardening, address SonarCloud issues, verify sanitizers (ASan/UBSan) | Zero critical bugs across compression modules |
| **Day 4 (29 Oct)** | Complete end-to-end integration and reconcile Requirement Traceability Matrix (RTM) | All `JACK-F-001..004` mapped to code and tests |
| **Day 5 (30 Oct)** | **Record 2-Minute Video Demo:** Full demonstration of CLI menu (Compress, Statistics, Validate, Decompress) + upload video | Video demo uploaded to repo |
| **Day 5 (30 Oct)** | **Development Freeze:** Tag release v1.0, lock `main` branch, finalize project report | Final submission ready |

---

## 4. Key Reviewer & Integrator Rules

1. **Enforce the PR Standard:** Every member must raise a PR from their own feature branch. No direct pushes to `main`.
2. **Review with Context:** As lead, verify that each PR contains tests and follows the approved header/API interfaces.
3. **Keep Commit History Balanced:** Ensure all four teammates make regular, meaningful commits to avoid unbalanced contributor graphs during course evaluation.
4. **Document Any Deviations:** If implementation alters any requirement from SRS or SAD, log it in `docs/deviations.md` for discussion during the final presentation.
