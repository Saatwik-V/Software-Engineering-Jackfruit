# Document 4 — End-to-End Guide: Vibhav

**Use case owned:** UC-04 — Validate Archive Integrity (CRC-32 checksum, structural validation, corruption detection)  
**Secondary role:** Security & Robustness Lead (STRIDE threat model, memory safety sanitizers, malformed input fuzz testing)  
**Your branch:** `feature/vibhav-integrity`  
**Authority:** Aligned with `SE_Mini_Project_Delivereables_Part-1.pdf` and `SE_Mini_Project_Delivereables_Part-2.pdf`.

This is your master guide for project execution — what to build, when, in what order, and on which branch.

---

## 1. Your Module & Security Scope

You own making sure the tool never trusts an external or corrupted file blindly:
- **CRC-32 Engine:** Implement standard IEEE 802.3 CRC-32 calculation (`0xEDB88320` polynomial) to compute checksums over uncompressed/payload data and compare against stored archive headers.
- **Structural Integrity Validator:** Validate header fields (magic bytes `0x4A41434B`, version `0x01`, reasonable sizes, declared bit counts vs file length) before any memory allocation or decoding occurs.
- **Corrupted Input Protection:** Ensure safe, graceful rejection of truncated, malformed, or tampered archives without crashes, undefined behavior, or memory leaks.
- **Integrity CLI Screen:** Interactive menu view and CLI command (`jackfruit validate <file.jack>`) returning exit code 0 for valid archives and exit code 3 for corrupt ones.
- **Security Leadership:**
  - Enforce AddressSanitizer (ASan) and UndefinedBehaviorSanitizer (UBSan) in local testing and CI.
  - Execute malformed input fuzzing tests.
  - Author the security review report (`docs/validation/security-review.md`).

---

## 2. Dependencies & Critical Path

- **Independent Start in Sprint 1:** Your standalone `CRC32` module can be developed and unit tested immediately against standard test vectors with zero external dependencies.
- **Foundation for Archive Format:** Your CRC-32 checksum specification is embedded in the 18-byte header built by Yuvraj's `ArchiveWriter`.
- **Validation on Top of Reader:** In Sprint 2, your `IntegrityValidator` uses Yuvraj's `ArchiveReader` to inspect headers and compute checksum comparisons.
- **Sprint 1 Team Milestone:** Deliver the tested CRC-32 module so the compression pipeline can write real checksums into archives for the Sprint 1 demo video.

---

## 3. Official Timeline & Deliverables Plan

### Phase 1: Engineering Documents (Part-1 — Completed Baseline)
- [x] Contribute CRC-32 checksum specifications to `docs/design/archive-format.md`.
- [x] Contribute UC-04 requirements (`JACK-F-013..016`) and Security Objectives/Requirements (`JACK-SR-001..005`) to IEEE SRS (`docs/srs/srs-v0.1.pdf`).
- [x] Author STRIDE Threat Model (Section 3.9) in IEEE SAD (`docs/design/architecture-v0.1.pdf`).
- [x] Review security validation section in Tanmayi's Software Test Plan.

---

### Phase 2: Backlog & Story Point Setup (ETA: 12th Oct 2026)
- [ ] In the linked GitHub Project board, verify your assigned backlog items:
  - CRC32 Checksum Engine (`src/security/crc32.cpp`) — **3 SP**
  - `JACK-F-013` (Structural archive validation) — **2 SP**
  - `JACK-F-014` (Checksum recalculation & mismatch detection) — **3 SP**
  - `JACK-F-015` (Interactive validate display screen) — **2 SP**
  - `JACK-F-016` (Command-line `--validate` execution) — **3 SP**
  - `JACK-SR-001` to `005` (Security & sanitizer hardening) — **5 SP**
- [ ] Participate in dry-run practice week (12th–16th Oct) testing PR creation and code review.
- [ ] Confirm backlog lock on 19th Oct.

---

### Phase 2: Sprint 1 — Core Working Product (19th Oct – 23rd Oct 2026)
**Goal:** Deliver standalone CRC-32 calculation engine, integrate with Saatwik's compressor, and support the 1-minute working demo video.

| Day / Task | Implementation Target | Artifact / Code Location |
|---|---|---|
| **Day 1–2 (19–20 Oct)** | Implement `CRC32` module using IEEE 802.3 standard | `include/security/crc32.hpp`, `src/security/crc32.cpp` |
| **Day 2–3 (20–21 Oct)** | Unit tests against known CRC-32 test vectors (empty string, `"123456789"`, binary buffers) | `tests/unit/test_crc32.cpp` |
| **Day 3–4 (21–22 Oct)** | Support Saatwik in integrating CRC calculation into the compression workflow | Checksum stored in `.jack` header |
| **Day 4 (22 Oct)** | Raise PR from `feature/vibhav-integrity` | PR reviewed and merged into `main` |
| **Day 5 (23 Oct)** | **Support Sprint 1 Demo Video:** Verify valid checksums are generated in the 1-minute working demo | Video demo ready |

---

### Phase 2: Sprint 2 — Full Feature Product & Development Freeze (26th Oct – 30th Oct 2026)
**Goal:** Implement `IntegrityValidator` and CLI command, configure memory sanitizers (ASan/UBSan), execute malformed input fuzz suite, author security review, and freeze development.

| Day / Task | Implementation Target | Artifact / Code Location |
|---|---|---|
| **Day 1–2 (26–27 Oct)** | Implement `IntegrityValidator` (structural checks + CRC comparison) | `include/security/integrity_validator.hpp`, `src/security/integrity_validator.cpp` |
| **Day 2–3 (27–28 Oct)** | Build interactive Validate screen and `jackfruit validate` command (exit codes 0 and 3) | `src/cli/validate_view.cpp` |
| **Day 3–4 (28–29 Oct)** | Configure AddressSanitizer and UndefinedBehaviorSanitizer test runs; run malformed input fuzz tests (`TC-VALID-01`, `TC-SEC-01`) | Zero memory leaks / buffer overflows |
| **Day 4 (29 Oct)** | Author `docs/validation/security-review.md` documenting threat mitigations and sanitizer evidence | Security review report |
| **Day 5 (30 Oct)** | **Support Sprint 2 Demo Video:** Showcase corruption detection and graceful error handling in 2-minute video | Demo video completed |
| **Day 5 (30 Oct)** | **Development Freeze:** Lock security modules, finalize security documentation | Ready for final evaluation |

---

## 4. Security Lead Best Practices

1. **Never Crash on Bad Input:** An untrusted or malformed file must **always** yield a clean error message and return code, never `SIGSEGV` or `std::bad_alloc`.
2. **Bounds-Check Header Declared Lengths:** Check that payload sizes declared in headers do not exceed the actual file size on disk before allocating buffers.
3. **Verify with Sanitizers:** Always run tests with `-fsanitize=address,undefined` to guarantee memory safety.
