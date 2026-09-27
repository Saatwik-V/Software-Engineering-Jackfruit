# Document 2 — End-to-End Guide: Tanmayi

**Use case owned:** UC-02 — Decompress File (bit reader → decoder → reconstruction)
**Secondary role:** Testing & Coverage Lead (you own the team's unit-test discipline and coverage reporting)
**Your branch:** `feature/tanmayi-decompression`

This is your ground truth for the whole semester — what to build, when, in what order, and on which branch. Read it once fully, then follow it week by week. Ask Saatwik if anything here conflicts with what's actually in the repo (this doc is the plan; the repo is the truth once things start moving).

---

## 1. Your module, precisely

You own turning a valid archive back into the original file:
- `BitReader` — the mirror of Saatwik's `BitWriter`. **Get his bit-ordering convention in writing before you build this** (Sprint 1–2); if you guess and he guesses differently, nothing will decode correctly and you'll lose a sprint finding out why.
- Decoder — bitstream → original bytes, using the Huffman tree/codes reconstructed from Yuvraj's archive metadata.
- Reconstruction — writing the decoded bytes back out as the original file, verified byte-for-byte.
- The interactive "Decompress" menu screen (prompt for archive path + output path, progress, success/failure message).

## 2. Dependencies — who needs what from whom

- You need Saatwik's `BitWriter` convention documented before you start `BitReader` (Sprint 1–2).
- You need Yuvraj's `ArchiveReader` (parses header/metadata) to reconstruct the Huffman tree before you can decode anything real (Sprint 3–4).
- You need a real archive from Saatwik's compress command (his Sprint 3 milestone) to test your decoder end-to-end — this is why his Sprint 3 deadline matters to you directly.
- As Testing Lead, everyone depends on you to set up the coverage tooling (gcov/lcov) in CI during Sprint 2, before code volume grows.

## 3. Sprint-by-sprint plan

| Sprint (Week) | What you build | Branch/PR | Deliverable & Definition of Done |
|---|---|---|---|
| **1** | Read and sign off on the archive format doc (Saatwik/Yuvraj draft it — you review it from a "can I decode this" angle). Write your SRS contribution: use case UC-02 description and its functional requirements (`FR-DECOMP-00x`). Set up GoogleTest conventions for the team (test naming, fixture patterns) — a short `docs/design/testing-conventions.md`. | PR: docs only | Testing conventions doc merged; UC-02 requirements in SRS v0.1. |
| **2** | `BitReader` (exact reverse of `BitWriter`): unit tests against **manually crafted bitstreams you write yourself**, not against Saatwik's encoder yet — this decouples you from his schedule. Also: wire gcov/lcov coverage collection into the CI workflow (coordinate with Yuvraj since he owns the CI pipeline). | `feature/tanmayi-decompression` → PR #1 | `BitReader` round-trips correctly against hand-built test bitstreams; CI now reports a coverage percentage on every PR. |
| **3** | Decoder logic: reconstruct Huffman tree from metadata + decode bitstream to bytes, still against synthetic/mocked archive data if Yuvraj's `ArchiveReader` isn't ready yet. Start drafting integration tests against a real archive as soon as Saatwik's compress command lands. | PR #2 | Decoder passes unit tests on synthetic data. |
| **4** | Full integration: real `ArchiveReader` (Yuvraj) + your decoder + Saatwik's real archives → **compress → decompress round trip produces byte-identical output**. Wire up the interactive "Decompress" CLI screen. This is your team milestone sprint. | PR #3 | Round trip verified on text, binary, empty, and large files. Flag this working end-to-end to the whole team — Yuvraj and Vibhav's later sprints assume this works. |
| **5** | Write 10 manual test cases for UC-02 (valid archive, empty original, one-byte original, all-256-value original, truncated header, truncated payload, invalid magic/version, wrong output path, checksum mismatch, malformed metadata). Start building the integration/system test suite structure in `tests/integration/` and `tests/system/` for the whole project — this is your Testing Lead responsibility, not just your own module. | PR into `docs/validation/` + `tests/` | 10 test cases documented; integration/system test folders scaffolded for all four modules. |
| **6** | Quality hardening as Testing Lead: get the team to a coverage target (≥80% line / ≥70% branch on core modules — adjust after your first baseline, don't guess a number nobody measured). Chase down any module below target. Fix your own SonarCloud findings. | Review + fix PRs | Coverage report published in CI for every PR; target met or documented reason given for gaps. |
| **7** | Help finalize integration/system tests across all four modules ahead of manual execution. Performance/robustness pass on your decoder against malformed inputs (coordinate with Vibhav, since malformed-input handling overlaps with his security work). | PR: fixes | Decoder never crashes on malformed input — always fails safely with a clear error. |
| **8** | Execute your 10 manual UC-02 test cases (fill Actual Result + Pass/Fail). Compile the final coverage report and automated-test summary for the report. Update RTM for your requirements. Rehearse the "testing & coverage" part of the demo. | Final PR + report doc | All FR-DECOMP requirements traced to code, tests, and manual results; coverage report finalized. |

## 4. What you personally must write in the shared documents

- **SRS:** UC-02 description and its functional requirements section.
- **Design doc:** Class + sequence diagram for decompression.
- **Testing conventions doc** (Sprint 1) and the **coverage report** (ongoing, finalized Sprint 8) — these are yours as Testing Lead, covering the whole codebase, not just your module.
- **Validation doc:** Your 10 UC-02 test cases.

## 5. PR checklist (use for every PR you open)

- [ ] Branched from latest `main`
- [ ] Unit tests added for new logic
- [ ] Builds clean, `ctest` passes locally, coverage didn't regress
- [ ] PR description references the GitHub issue number
- [ ] At least one teammate reviewed and approved
- [ ] CI green before merge
- [ ] SRS/design doc updated if this PR changes a requirement or interface

## 6. Pitfalls specific to your role

- Don't wait for Saatwik's real encoder before writing any tests — build your own synthetic bitstreams so you're never idle waiting on someone else's branch.
- As Testing Lead, don't let "unit tests" be the only kind of test that exists by Sprint 6 — integration and system tests are explicitly required by the course and are easy to forget until the last week.
- Don't let coverage numbers be vanity metrics — a module at 95% line coverage with only happy-path tests is worse evidence than 75% with real edge cases. Say so if you see it.
- Fill "Actual Result" and "Test Result" in manual test sheets only after you've actually run the test — never backfill these to look complete.
