# Document 5 — Master Project Checklist

**Project:** Jackfruit Mini-Project — Basic ZIP-style File Compression Tool (Huffman Coding), interactive CLI
**Team:** Saatwik (lead), Tanmayi, Yuvraj, Vibhav
**Course:** UE24CS341A — Software Engineering

This is the single source of truth for where the whole team stands at any point. It doesn't replace the 5 other documents — it's the checkpoint view across all of them. Whoever is unsure "are we on track" reads this file first. Update it at the end of every sprint review.

---

## 0. How the 6 documents fit together

| Doc | For | Purpose |
|---|---|---|
| `00-SETUP-GUIDE-Saatwik.md` | Saatwik only | Day-0 repo/CI/board bootstrap — do this before Sprint 1 |
| `01-MEMBER-GUIDE-Saatwik.md` | Saatwik | His sprint-by-sprint contributor work (UC-01 Compress) |
| `02-MEMBER-GUIDE-Tanmayi.md` | Tanmayi | Her sprint-by-sprint contributor work (UC-02 Decompress) |
| `03-MEMBER-GUIDE-Yuvraj.md` | Yuvraj | His sprint-by-sprint contributor work (UC-03 Statistics) |
| `04-MEMBER-GUIDE-Vibhav.md` | Vibhav | His sprint-by-sprint contributor work (UC-04 Validate) |
| `05-MASTER-PROJECT-CHECKLIST.md` | Everyone | This file — cross-team checkpoints and grading-stakes tracker |

## 1. Marks weightage — know what's actually at stake

| Stage | Deliverable | Marks |
|---|---|---|
| Requirements | SW Requirements Specification | 12 |
| Requirements | Validation Spec | 8 |
| Design | High-level Architecture document | 7 |
| Design | SW Design (UML, in Arch doc) | 7 |
| Design | Validation Spec update | 6 |
| Implementation | Iterative design & implementation | 15 |
| Implementation | Unit test coverage | 5 |
| Implementation | Validation Spec update | 5 |
| System Validation | Validation report + maintenance plan | 15 |
| Final Demo | Working demo + 15-min presentation | 20 |
| **Total** | | **100** |

Implementation + testing together are worth 30, but Requirements + Design together are also worth 33 — don't treat the paperwork phases as filler before "the real work." They're graded almost as heavily as the code.

## 2. Team ownership at a glance

| Member | Use case | Branch | Secondary role |
|---|---|---|---|
| Saatwik | UC-01 Compress | `feature/saatwik-compression` | Lead, architecture, integrator |
| Tanmayi | UC-02 Decompress | `feature/tanmayi-decompression` | Testing & coverage lead |
| Yuvraj | UC-03 Statistics | `feature/yuvraj-statistics` | CI/CD lead |
| Vibhav | UC-04 Validate | `feature/vibhav-integrity` | Security & SonarCloud lead |

Rule for all branches: no direct pushes to `main`; PR + 1 review + green CI required to merge.

## 3. Sprint-by-sprint master checkpoint

Mark each row done only when **all** listed items are true — a sprint isn't "done" if one person's part is still open.

### Sprint 1 — Foundations
- [ ] Repo created, protected `main`, all 4 branches created (Saatwik)
- [ ] CI skeleton green on `main` (Saatwik)
- [ ] GitHub Project board created with Sprint 1–2 issues (Saatwik)
- [ ] Archive binary format agreed and documented in `docs/design/archive-format.md` (Saatwik + Yuvraj, reviewed by Tanmayi + Vibhav)
- [ ] SRS v0.1 mapped to `SRS_Template for SE.docx`: 16 FRs (`JACK-F-001..016`), 5 NFRs (`JACK-NF-001..005`), 5 Security Reqs (`JACK-SR-001..005`), 2 UML Use-Case diagrams, and RTM table
- [ ] SAD v0.1 mapped to `SAD_Template.docx`: Layered architecture, Component UML, STRIDE threat model, 2 UML Sequence diagrams, and C++ API interfaces (Saatwik)
- [ ] Testing conventions & initial Software Test Plan (STP) mapped to `Test_Plan_Template for SE.docx` (Tanmayi)

### Sprint 2 — Core algorithm & foundations
- [ ] Frequency table, Huffman tree, code generation, `BitWriter` — unit tested (Saatwik)
- [ ] `ArchiveWriter`/`ArchiveReader` serialize/parse — unit tested (Yuvraj) **← critical path, blocks 2 people**
- [ ] `BitReader` against hand-built bitstreams — unit tested (Tanmayi)
- [ ] Coverage reporting (gcov/lcov) wired into CI (Tanmayi + Yuvraj)
- [ ] Checksum/CRC module — unit tested (Vibhav)

### Sprint 3 — Compression vertical slice
- [ ] Encoder + `ArchiveWriter` integrated, interactive Compress screen working (Saatwik, supported by Yuvraj)
- [ ] `compress input.txt output.hzip` works end-to-end from the CLI menu
- [ ] GitHub Actions coverage step added (Yuvraj + Tanmayi)

### Sprint 4 — Decompression vertical slice
- [ ] Decoder + `ArchiveReader` integrated, interactive Decompress screen working (Tanmayi, supported by Yuvraj)
- [ ] **Compress → decompress round trip byte-identical** on text/binary/empty/large files — team milestone
- [ ] Statistics computation started against real archives (Yuvraj)

### Sprint 5 — Statistics + integrity, test cases written
- [ ] Interactive Statistics screen finished (Yuvraj)
- [ ] Checksum + structural validation integrated, interactive Validate screen working (Vibhav)
- [ ] SonarCloud wired into CI (Yuvraj)
- [ ] 10 manual test cases written per use case, all 4 owners (40 total, not yet executed)
- [ ] Integration/system test folders scaffolded (Tanmayi)

### Sprint 6 — Quality hardening
- [ ] Coverage target met or gap documented (Tanmayi)
- [ ] SonarCloud critical issues resolved, per module owner
- [ ] Sanitizers (ASan/UBSan) run against full pipeline
- [ ] Security review doc started (Vibhav)
- [ ] Release-zip packaging step added to CI (Yuvraj)

### Sprint 7 — Security, performance, release
- [ ] Performance baseline measured and recorded, not guessed (Saatwik)
- [ ] Fuzz/malformed-input batch run against full pipeline, no crashes (Vibhav + Tanmayi)
- [ ] Release packaging finalized, reproducible from a clean clone (Yuvraj)
- [ ] Security review doc finalized (Vibhav)

### Sprint 8 — System validation & finalization
- [ ] All 40 manual test cases executed, Actual Result + Pass/Fail filled (all 4 owners, for their own use case)
- [ ] Requirement Traceability Matrix complete end-to-end (all requirements → design → code → tests)
- [ ] Maintenance plan drafted (Saatwik, reviewed by all)
- [ ] Final report assembled (Saatwik leads, sections from all 4)
- [ ] Demo rehearsed — everyone can explain their own module **and** at least one neighboring module

## 4. Final report assembly checklist (Section order)

- [ ] Cover page
- [ ] Table of contents
- [ ] Proposal / Synopsis
- [ ] Software Requirements Specification
- [ ] Agile Project Plan
- [ ] Architecture and Design Diagrams
- [ ] Implementation overview and module ownership
- [ ] Test Plan and Test Cases (all 40)
- [ ] Coverage, CI/CD, SonarCloud and security evidence
- [ ] Bug/defect summary
- [ ] System Validation Report
- [ ] Maintenance Plan
- [ ] Screenshots / demo evidence
- [ ] Requirement Traceability Matrix
- [ ] Individual contribution summary
- [ ] References / glossary / appendices

## 5. 15-minute demo timing plan

| Time | Content | Lead speaker |
|---|---|---|
| 0:00–1:00 | Problem, motivation, scope | Saatwik |
| 1:00–3:00 | Use cases + architecture | Saatwik |
| 3:00–7:00 | Live demo: compress → statistics → validate → decompress | One member per screen |
| 7:00–9:00 | Huffman algorithm + archive format + key decisions | Saatwik + Yuvraj |
| 9:00–11:00 | Testing, coverage, bugs, security validation | Tanmayi + Vibhav |
| 11:00–13:00 | GitHub board, PRs, GitHub Actions, SonarCloud | Yuvraj |
| 13:00–14:00 | Differentiation, novelty, limitations | Whole team |
| 14:00–15:00 | Contribution summary + SDLC artefacts + conclusion | Saatwik |

## 6. Evidence checklist a grader will look for

- [ ] GitHub Project board with backlog/sprint history (shows real Agile process)
- [ ] Issues with clear acceptance criteria
- [ ] Git commit graph showing balanced individual contribution
- [ ] Branches + PRs + review comments (shows real collaboration, not one person's code)
- [ ] GitHub Actions run history
- [ ] GoogleTest results + coverage report
- [ ] SonarCloud report
- [ ] Sanitizer/security evidence
- [ ] UML diagrams (use case, architecture, class, sequence)
- [ ] Requirement Traceability Matrix
- [ ] Manual test sheet with genuine Actual Result / Pass-Fail entries
- [ ] Bug tickets with regression tests
- [ ] Working interactive tool + sample files at demo time

## 7. Critical mistakes to avoid (team-wide)

- Don't build features first and create board issues afterwards — issues come before code.
- Don't let one person dominate the commit graph.
- Don't count UI, testing, or documentation alone as anyone's "full functionality" — each person's UI screen sits on top of their real backend module.
- Don't merge a PR without review and a green CI check.
- Don't write only happy-path tests.
- Don't claim full ZIP/DEFLATE compatibility — this is a Huffman-based custom archive tool.
- Don't leave the Requirement Traceability Matrix until the final week.
- Don't fabricate manual test results — fill Actual Result only after running the test.
- Don't accept LLM-generated code without understanding it well enough to explain it live — the course explicitly grades this.
- Don't change the archive binary format informally once others are building against it.

## 8. Definition of project success

- All four use cases work end-to-end from the interactive CLI.
- A compressed file decompresses to byte-identical content.
- Invalid/corrupt inputs fail safely, never crash.
- All 40 manual test cases executed and documented.
- Automated unit/integration/system tests pass in CI.
- Line and branch coverage reported.
- GitHub Actions CI/CD demonstrably working end-to-end.
- SonarCloud and security evidence available.
- Every requirement traces to architecture → design → code → tests.
- Every member has meaningful, visible commits and can explain their module and a neighbor's.
- Final report contains every required SDLC artefact.
