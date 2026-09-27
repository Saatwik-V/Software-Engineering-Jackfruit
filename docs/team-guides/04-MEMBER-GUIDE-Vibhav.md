# Document 4 — End-to-End Guide: Vibhav

**Use case owned:** UC-04 — Validate Archive Integrity (checksum/CRC, structural validation, corruption handling)
**Secondary role:** Security & SonarCloud Lead — you own the team's security-validation evidence
**Your branch:** `feature/vibhav-integrity`

This is your ground truth for the whole semester — what to build, when, in what order, and on which branch. Read it once fully, then follow it week by week.

---

## 1. Your module, precisely

You own making sure the tool never trusts a file blindly:
- Checksum/CRC calculation and verification.
- Structural validation of an archive: header well-formed, version supported, declared sizes plausible, payload boundaries consistent.
- Safe rejection of malformed/truncated/corrupted archives — with a clear error message, never a crash and never silent data corruption.
- The interactive "Validate" menu screen (prompt for archive path, clear valid/corrupt report).
- Team-wide security review: as Security Lead, you keep an eye on every module's handling of untrusted input (file paths, declared sizes, archive fields) — not just your own.

## 2. Dependencies — who needs what from whom

- You need Yuvraj's header/metadata parsing (`ArchiveReader`) as a foundation for structural validation — your Sprint 5 depends on his Sprint 2 and Sprint 4 work.
- You benefit from Saatwik's and Tanmayi's real archives (from Sprint 3–4) to test against real corruption scenarios, not just synthetic ones.
- Everyone benefits from your Sprint 2 checksum module early, since Yuvraj's archive format needs a checksum field defined from the start.

## 3. Sprint-by-sprint plan

| Sprint (Week) | What you build | Branch/PR | Deliverable & Definition of Done |
|---|---|---|---|
| **1** | Contribute the checksum/integrity field to the archive format discussion (Saatwik + Yuvraj) — you're the one who has to actually verify it, so make sure it's specified precisely. Write your SRS contribution: UC-04 description and functional/security requirements (`FR-VALID-00x`, `SEC-00x`). | PR: docs only | Security requirements section exists in SRS v0.1. |
| **2** | Checksum/CRC module: calculate + verify, with unit tests against known vectors, empty data, and a deliberately mismatched checksum. This is independent of everyone else — build and test it standalone now. | `feature/vibhav-integrity` → PR #1 | Checksum module fully unit-tested in isolation. |
| **3–4** | While others integrate their vertical slices, start structural-validation logic against Yuvraj's `ArchiveReader` as soon as it's usable: reject bad magic bytes, unsupported versions, and impossible size fields before allocating memory or reading data (this is the actual security-relevant part — prevent overflow/out-of-bounds behavior from untrusted archive fields). | PR #2 | Validator rejects a set of hand-crafted malformed archives safely (no crash, no undefined behavior). |
| **5** | Integrate checksum verification + structural validation into one "Validate Archive Integrity" flow. Wire up the interactive "Validate" CLI screen with a clear valid/corrupt report. Write 10 manual test cases for UC-04 (valid archive, bad magic, unsupported version, truncated header, impossible size, truncated payload, bad checksum, invalid tree metadata, trailing-garbage policy, repeated validation). | PR into `docs/validation/` | Validate command works end-to-end on both good and deliberately broken archives. |
| **6** | Security pass across the whole codebase (your lead responsibility, not just your module): check every place a file path, size field, or length comes from user/archive input and confirm it's validated before use. Run sanitizers (ASan/UBSan) against the full pipeline if not already covered. Address your own SonarCloud security hotspots first, then flag any you spot in other modules to their owners. | Review + fix PRs | A short `docs/validation/security-review.md` listing what was checked and what was found/fixed. |
| **7** | Fuzz/malformed-input pass: feed the compress/decompress/validate pipeline a batch of corrupted/truncated files and confirm nothing crashes or misbehaves. Coordinate with Tanmayi since this overlaps with her robustness testing. | PR: fixes | No crashes on the malformed-input batch; every failure path returns a clean error. |
| **8** | Execute your 10 manual UC-04 test cases (fill Actual Result + Pass/Fail). Finalize the security review doc for the report. Update RTM for your requirements. Rehearse the "security & robustness" part of the demo. | Final PR + report doc | All FR-VALID/SEC requirements traced to code, tests, and manual results; security evidence section ready. |

## 4. What you personally must write in the shared documents

- **SRS:** UC-04 description, functional requirements, and the **Security Requirements** subsection (Section 6.3 of the SRS template) — this section is easy to forget and you're the natural owner.
- **Design doc:** Class + sequence/activity diagram for archive validation and the error path.
- **Validation doc:** Your 10 UC-04 test cases, plus the security review write-up.

## 5. PR checklist (use for every PR you open)

- [ ] Branched from latest `main`
- [ ] Unit tests added for new logic
- [ ] Builds clean, `ctest` passes locally
- [ ] PR description references the GitHub issue number
- [ ] At least one teammate reviewed and approved
- [ ] CI green before merge
- [ ] SRS/design doc updated if this PR changes a requirement or interface

## 6. Pitfalls specific to your role

- Don't limit "security validation" to happy-path checksum matching — the actual grading interest is in **malformed-input handling**: truncated files, bad lengths, impossible values. Build a deliberately-broken test archive collection early and reuse it all semester.
- Don't wait until Sprint 6 to look at other people's input-handling — a quick note to a teammate in Sprint 3 ("this length field should be bounds-checked before use") is cheaper than finding it during the security pass.
- Don't treat "it didn't crash on my machine" as proof — use ASan/UBSan, they catch what manual testing misses.
- Don't skip writing the security requirements section of the SRS just because it feels like an afterthought category — it's an explicit graded requirement (`6.3 Security Requirements`) and it's yours.
