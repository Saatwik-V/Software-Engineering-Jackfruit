# Document 3 — End-to-End Guide: Yuvraj

**Use case owned:** UC-03 — Inspect Compression Statistics (archive header format, archive read/write, reporting)
**Secondary role:** CI/CD Lead — you own and evolve the GitHub Actions pipeline after Saatwik's Day-0 skeleton
**Your branch:** `feature/yuvraj-statistics`

This is your ground truth for the whole semester — what to build, when, in what order, and on which branch. Read it once fully, then follow it week by week.

---

## 1. Your module, precisely

You own the archive format itself — this makes your work foundational, not just "statistics."
- The archive header/metadata layout (magic bytes, version, original size, tree/code metadata, bit-count, checksum placeholder, payload) — you and Saatwik agree this together in Sprint 1, then **you own the code that serializes and parses it** (`ArchiveWriter` / `ArchiveReader`).
- Statistics reporting: original size, compressed size, ratio, estimated savings, timing.
- The interactive "Statistics" menu screen (prompt for archive path, formatted results table).
- CI/CD pipeline evolution: build/test/coverage/SonarCloud all wired into GitHub Actions and kept green.

## 2. Dependencies — who needs what from whom

- Saatwik needs your `ArchiveWriter` to finish his compress vertical slice (his Sprint 3) — this is the single most time-critical dependency in the whole project. If your archive writer slips, three other people's sprints slip.
- Tanmayi needs your `ArchiveReader` for her decoder (her Sprint 4).
- Vibhav needs your header/metadata parsing to build structural validation on top of it (his Sprint 5).
- You need the team's agreed archive format from Sprint 1 before writing any serialization code — don't start coding the format before it's written down and everyone has seen it.

## 3. Sprint-by-sprint plan

| Sprint (Week) | What you build | Branch/PR | Deliverable & Definition of Done |
|---|---|---|---|
| **1** | Co-author the archive binary format with Saatwik (`docs/design/archive-format.md`) — this is joint, not solo. Write your SRS contribution: Section 4.3 for UC-03 description and functional requirements (`JACK-F-009` through `JACK-F-012`). Review Saatwik's CI skeleton from the Setup Guide and plan what you'll add (coverage, SonarCloud) in later sprints. | PR: docs only | Archive format signed off by all 4 members before Sprint 2; UC-03 requirements in SRS v0.1. |
| **2** | **`ArchiveWriter`/`ArchiveReader`**: serialize/parse the header and metadata, with round-trip unit tests (serialize then parse gives back the same values) and negative tests (invalid magic, unsupported version, impossible field values — these will matter a lot more to Vibhav later, but write the basic ones now). | `feature/yuvraj-statistics` → PR #1 | Archive header serializes/parses correctly in isolation, before any real Huffman data exists. **This is your critical-path deliverable — treat this sprint as non-negotiable.** |
| **3** | Support Saatwik integrating `ArchiveWriter` into his compress command (pair with him if needed). Start adding GitHub Actions coverage reporting (with Tanmayi) to the pipeline. | PR #2 (fixes/support) | Saatwik's compress milestone lands using your writer without major rework. |
| **4** | Support Tanmayi integrating `ArchiveReader` into her decoder. Begin the Statistics use case itself: read a valid archive and compute original size, compressed size, ratio. | PR #3 | Statistics can be computed from any real archive Saatwik/Tanmayi produce. |
| **5** | Finish the interactive "Statistics" CLI screen with a clean formatted output table. Write 10 manual test cases for UC-03 (valid archive, empty file, high redundancy, low redundancy, ratio calculation, size consistency, invalid archive, missing file, boundary sizes, output formatting). Expand GitHub Actions to run SonarCloud analysis on every PR. | PR into `docs/validation/` + `.github/workflows/` | Statistics screen demo-ready; SonarCloud dashboard live and linked in README. |
| **6** | Quality hardening: as CI/CD lead, make sure build+test+coverage+SonarCloud all run reliably on every PR, not just on `main`. Fix your own SonarCloud findings. Help package a release build (zip artifact per the course's "Docker optional, zip file ok" note) as a CI step. | Pipeline PRs | CI pipeline produces a downloadable release zip as an artifact on merge to `main`. |
| **7** | Support performance baseline measurement (Saatwik) by adding a simple timing harness if useful. Finalize the release packaging step. | PR: pipeline polish | One-command reproducible build + test + package works for a new clone of the repo. |
| **8** | Execute your 10 manual UC-03 test cases (fill Actual Result + Pass/Fail). Compile CI/CD and SonarCloud evidence (screenshots/links) for the final report. Update RTM for your requirements. Rehearse the "DevOps/CI-CD" part of the demo. | Final PR + report doc | All JACK-F-009..012 requirements traced to code, tests, and manual results; CI/CD evidence section ready. |

## 4. What you personally must write in the shared documents

- **SRS:** Section 4.3 (UC-03 description and functional requirements `JACK-F-009` through `JACK-F-012`), plus co-authoring the archive format specification.
- **Design doc (SAD):** Class diagram for statistics reporting, API design for `ArchiveWriter`/`ArchiveReader` (Section 4.3), plus the archive-format field table.
- **Validation doc:** Your 10 UC-03 test cases.
- **CI/CD evidence:** Pipeline history, SonarCloud report links/screenshots — this is the "evidence" a grader looks for that CI/CD is real, not decorative.

## 5. PR checklist (use for every PR you open)

- [ ] Branched from latest `main`
- [ ] Unit tests added for new logic
- [ ] Builds clean, `ctest` passes locally
- [ ] PR description references the GitHub issue number
- [ ] At least one teammate reviewed and approved
- [ ] CI green before merge
- [ ] SRS/design doc updated if this PR changes a requirement or interface

## 6. Pitfalls specific to your role

- Don't be the bottleneck — Saatwik and Tanmayi are both waiting on your Sprint 2 deliverable. If you're stuck, say so in the daily check-in immediately, not at the end of the week.
- Don't gold-plate the archive format before it's agreed — get the minimum viable set of fields signed off in Sprint 1, refine later only with a documented format-version bump.
- As CI/CD lead, don't let the pipeline become something only you understand — document the workflow file with comments so any teammate can read what each CI step does.
- Don't treat "statistics" as a trivial module just because it sounds simple — the archive parsing underneath it is genuinely load-bearing infrastructure for the whole team.
