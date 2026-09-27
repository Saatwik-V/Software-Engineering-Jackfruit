# Document 1 — End-to-End Guide: Saatwik

**Use case owned:** UC-01 — Compress File (frequency table → Huffman tree → encoder → archive write)
**Secondary role:** Overall lead, architecture owner, main-branch integrator, code-review lead
**Your branch:** `feature/saatwik-compression`
**Do first:** `00-SETUP-GUIDE-Saatwik.md` (repo/CI/board must exist before Sprint 1 starts)

This is your ground truth for the whole semester — what to build, when, in what order, and on which branch. Read this once fully, then use it week by week.

---

## 1. Your module, precisely

You own everything needed to turn a raw input file into a valid archive:
- Frequency table construction from input bytes.
- Huffman tree construction and Huffman code generation.
- Encoder (bytes → Huffman bitstream), using the shared `BitWriter` (you build this shared utility since compression needs it first; decompression will reuse your `BitWriter`'s mirror, `BitReader`, which Tanmayi owns).
- The interactive "Compress" menu screen in the CLI (prompts for input/output paths, shows progress, reports original vs. compressed size on success).

You do **not** own the archive header/metadata format or its serialization — that's Yuvraj's (Archive layer). You call into his `ArchiveWriter` once your encoded bitstream is ready. Agree the exact header fields together in Sprint 1 before either of you builds against it (see below).

## 2. Dependencies — who needs what from whom

- You depend on: nothing to *start*. Your Huffman core (Sprint 2) is pure algorithm, no other module needed.
- You depend on Yuvraj's `ArchiveWriter` to finish the **end-to-end** compress command (Sprint 3).
- Tanmayi depends on your `BitWriter`'s bit-ordering convention to build `BitReader` correctly — write this convention down in the design doc the moment you decide it (Sprint 1–2), don't leave it implicit in code.
- Everyone depends on you (as architecture owner) to keep `src/compression/` and `include/` interfaces stable once others start building against them — don't change a public interface without flagging it to the team first.

## 3. Sprint-by-sprint plan

| Sprint (Week) | What you build | Branch/PR | Deliverable & Definition of Done |
|---|---|---|---|
| **1** | Draft system architecture (layers diagram), use-case diagram, and — jointly with the whole team — the archive binary format (magic bytes, version, original size, tree metadata, bit-count, checksum, payload). Write SRS v0.1 skeleton (you own Sections 1–2: Introduction, Overall Description). CMake/CI already done in Setup Guide. | Small PR: `docs/design/architecture-v0.1.md`, `docs/design/archive-format.md` | Format is written down and agreed by all 4 before Sprint 2 starts. |
| **2** | Frequency table + Huffman tree construction + code generation. `BitWriter` (bit-level packing, byte-boundary handling, flush). Unit tests for all of it (empty input, single symbol, all 256 byte values, repeated bytes). | `feature/saatwik-compression` → PR #1 | Huffman core passes unit tests **independently of any file I/O or archive format**. Reviewed by Tanmayi (she needs your `BitWriter` contract). |
| **3** | Encoder (bytes → bitstream using your codes), integrate with Yuvraj's `ArchiveWriter`, wire up the interactive "Compress" CLI screen (prompt → progress → result). | PR #2 | First real end-to-end: `compress input.txt output.hzip` works from the interactive menu and produces a real archive. This is the Sprint-3 team milestone — don't slip it, Tanmayi's Sprint 4 needs a real archive to decompress. |
| **4** | Support Tanmayi's round-trip testing (compress → decompress → byte-identical). Fix any encoder bugs surfaced. Start writing your part of the Design doc: class diagram + sequence diagram for UC-01. | PR #3 (fixes) | Round-trip test passes on varied files (text, binary, empty, large). |
| **5** | Write 10 manual test cases for UC-01 (per the course template: Test Case ID, Module, Description, Preconditions, Steps, Test Data, Expected/Actual/Result) covering normal, empty, one-byte, all-256-values, nonexistent input, unreadable input, output-path-collision, large-file cases. | PR into `docs/validation/` | 10 test cases documented (not yet executed — execution happens Sprint 8). |
| **6** | Quality hardening: address SonarCloud findings in your module, run sanitizers (ASan/UBSan) against the encoder, fix anything they flag. Review Tanmayi/Yuvraj/Vibhav PRs actively — you're the architecture reviewer. | Review-only + small fix PRs | Your module has zero unresolved critical SonarCloud issues. |
| **7** | Performance baseline: measure compression time/memory on a representative large file, record numbers (don't invent targets before measuring). Help finalize packaging (zip release build). | PR: `docs/validation/performance-baseline.md` | Numbers recorded, not guessed. |
| **8** | Execute your 10 manual test cases (fill Actual Result + Pass/Fail). Update the Requirement Traceability Matrix for your requirements. Assemble the final report skeleton (you own overall assembly since you're lead) and rehearse the "architecture + integration" part of the demo. | Final PR + report doc | All FR-COMP requirements traced to code, tests, and manual results. |

## 4. What you personally must write in the shared documents

- **SRS:** Sections 1 (Introduction) and 2 (Overall Description) — plus Section 5 subsection for UC-01's functional requirements (`FR-COMP-00x`).
- **Design doc:** System architecture diagram, use-case diagram, component/module diagram (whole-system, not just yours), plus your own class + sequence diagram for compression.
- **Validation doc:** Your 10 UC-01 test cases, plus you're responsible for compiling everyone else's test cases into one document by Sprint 5.
- **Maintenance plan (Sprint 8):** You draft the first version since you know the architecture best; others add their module's known limitations.

## 5. PR checklist (use for every PR you open)

- [ ] Branched from latest `main`
- [ ] Unit tests added for new logic
- [ ] Builds clean, `ctest` passes locally
- [ ] PR description references the GitHub issue number
- [ ] At least one teammate reviewed and approved
- [ ] CI green before merge
- [ ] SRS/design doc updated if this PR changes a requirement or interface

## 6. Pitfalls specific to your role

- Don't let "I'm the lead" become "I write most of the code." Weekly commit graphs are checked — everyone's contribution must be visible and roughly balanced.
- Don't quietly change the archive format after Yuvraj/Tanmayi have built against it. Any format change is a design change: update `docs/design/archive-format.md`, tell the team, update tests.
- Don't merge your own PRs without a review just because you're the integrator — the "no direct pushes, 1 review minimum" rule applies to you too.
- Don't let the compress vertical slice slip past Sprint 3 — three other people's sprints (4 and 5) are blocked on it.
