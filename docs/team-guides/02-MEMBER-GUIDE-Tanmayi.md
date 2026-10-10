# Document 2 — End-to-End Guide: Tanmayi

**Use case owned:** UC-02 — Decompress File (bit reader → decoder → byte reconstruction)  
**Secondary role:** Testing & Coverage Lead (GoogleTest architecture, test plan, CI coverage monitoring)  
**Your branch:** `feature/tanmayi-decompression`  
**Authority:** Aligned with `SE_Mini_Project_Delivereables_Part-1.pdf` and `SE_Mini_Project_Delivereables_Part-2.pdf`.

This is your master guide for project execution — what to build, when, in what order, and on which branch.

---

## 1. Your Module & Testing Scope

You own turning a valid compressed archive back into an exact byte-identical replica of the original file:
- **BitReader Module:** The mirror of Saatwik's `BitWriter`. Sequentially extracts bits from bytes following the agreed MSB-first packing convention up to `total_bits` boundary without consuming padding bits.
- **Huffman Decoding Engine:** Traverses the reconstructed prefix tree for each input bit until a leaf node is encountered, emitting the decoded 8-bit symbol.
- **File Reconstruction:** Emits decoded byte stream to target output file; verifies byte-exact integrity.
- **Decompress CLI Screen:** Prompts for input archive and output path, provides status feedback, and reports restoration success.
- **Testing & Quality Leadership:**
  - Maintain the IEEE Software Test Plan (`docs/validation/test-plan-v0.1.md` / `test-plan-v0.1.pdf`).
  - Enforce GoogleTest conventions across the team (`docs/design/testing-conventions.md`).
  - Wire automated code coverage (gcov/lcov) into CI and ensure the codebase hits **≥ 80% line coverage** and **≥ 70% branch coverage**.

---

## 2. Dependencies & Critical Path

- **Bit Packing Agreement:** You follow the MSB-first bit order specified in `docs/design/archive-format.md` (Signed Off).
- **Metadata Deserialization:** You receive the reconstructed tree and total payload bit count from Yuvraj's `ArchiveReader` (`src/archive/archive_reader.cpp`).
- **Synthetic Testing First:** You do NOT need to wait for Saatwik's encoder to test `BitReader` and `Decoder`; build unit tests against hand-crafted synthetic bit sequences first.
- **Sprint 1 Team Milestone:** Partner with Saatwik to prove **100% byte-identical round-trip restoration** by October 23 for the required 1-minute video demo.

---

## 3. Official Timeline & Deliverables Plan

### Phase 1: Engineering Documents (Part-1 — In Progress / Final Sign-off)
- [x] Review and sign off on binary archive specification (`docs/design/archive-format.md`).
- [x] Author GoogleTest conventions (`docs/design/testing-conventions.md`).
- [x] Contribute UC-02 requirements (`JACK-F-005..008`) to IEEE SRS (`docs/srs/srs-v0.1.pdf`).
- [ ] **Finalize Software Test Plan:** Incorporate the 10 concrete test cases and updated 2-Sprint schedule from `docs/meeting-notes/review-feedback-tanmayi-testplan.md` into `docs/validation/test-plan-v0.1.md`.
- [ ] Compile `docs/validation/test-plan-v0.1.pdf` via `python3 scripts/generate_docs_pdf.py` and submit PR.

---

### Phase 2: Backlog & Story Point Setup (ETA: 12th Oct 2026)
- [ ] In the linked GitHub Project board, verify your assigned UC-02 backlog items:
  - `JACK-F-005` (Archive header reading & magic validation) — **3 SP**
  - `JACK-F-006` (Huffman tree reconstruction from archive) — **5 SP**
  - `JACK-F-007` (BitReader boundary bit parsing) — **3 SP**
  - `JACK-F-008` (Byte-identical file reconstruction) — **5 SP**
  - `JACK-SR-001` / `JACK-SR-003` (Malformed archive & truncation safety) — **3 SP**
- [ ] Participate in dry-run practice week (12th–16th Oct) testing branch workflows.
- [ ] Confirm backlog lock on 19th Oct.

---

### Phase 2: Sprint 1 — Core Working Product (19th Oct – 23rd Oct 2026)
**Goal:** Implement `BitReader` and `Decoder`, achieve round-trip decompression against Saatwik's compressor, and co-deliver the 1-minute working demo video.

| Day / Task | Implementation Target | Artifact / Code Location |
|---|---|---|
| **Day 1–2 (19–20 Oct)** | Implement `BitReader` class | `include/io/bit_reader.hpp`, `src/io/bit_reader.cpp` |
| **Day 2–3 (20–21 Oct)** | Implement `Decoder` module | `include/decompression/decoder.hpp`, `src/decompression/decoder.cpp` |
| **Day 3 (21 Oct)** | Unit test `BitReader` & `Decoder` against synthetic vectors | `tests/unit/test_bit_reader.cpp`, `tests/unit/test_decoder.cpp` |
| **Day 4 (22 Oct)** | Raise PR from `feature/tanmayi-decompression` | PR with clean tests and CI pass; review from Saatwik |
| **Day 5 (23 Oct)** | **Round-Trip Verification:** Test `compress` -> `decompress` on text and binary files (`cmp sample.txt restored.txt`) | Zero differences confirmed |
| **Day 5 (23 Oct)** | **Assist Sprint 1 Demo Video:** Validate CLI commands shown in the 1-minute working demo video | Upload to repo |

---

### Phase 2: Sprint 2 — Full Feature Product & Development Freeze (26th Oct – 30th Oct 2026)
**Goal:** Implement coverage reporting, expand integration/system test suites, execute manual test cases, verify quality targets (≥80% coverage), assist 2-minute demo video, and freeze development.

| Day / Task | Implementation Target | Artifact / Code Location |
|---|---|---|
| **Day 1–2 (26–27 Oct)** | Wire `gcov`/`lcov` coverage reporting into GitHub Actions CI | `.github/workflows/ci.yml`, HTML coverage reports |
| **Day 2–3 (27–28 Oct)** | Build end-to-end integration and system test suites | `tests/integration/test_roundtrip.cpp`, `tests/system/test_cli_e2e.cpp` |
| **Day 3–4 (28–29 Oct)** | Execute 10 baseline test cases (`TC-COMP`, `TC-DECOMP`, `TC-STAT`, `TC-VALID`, `TC-PERF`, `TC-SEC`) | Record results in `docs/validation/test-results.md` |
| **Day 4 (29 Oct)** | Ensure team meets quality gate (≥80% line coverage, ≥70% branch coverage) | Chase down and add tests for uncovered branches |
| **Day 5 (30 Oct)** | **Assist Sprint 2 Demo Video:** Verify all test cases and decompress flows in 2-minute video | Demo video completed |
| **Day 5 (30 Oct)** | **Development Freeze:** Finalize testing documentation, lock branch | Ready for final evaluation |

---

## 4. Testing Lead Best Practices

1. **Test Edge Cases First:** Test `BitReader` against streams with 1, 7, 8, 9, and 13 bits to ensure trailing bit padding is never misread as valid data.
2. **Deterministic Byte Equality:** Decompression is successful **only** when `diff` or `cmp` returns exit code 0.
3. **No Unexecuted Test Records:** Fill in "Actual Result" and "Pass/Fail" only after actually executing the test case against the compiled binary.
