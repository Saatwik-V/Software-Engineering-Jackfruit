# GitHub Backlog Guide: Vibhav (Security & Integrity Lead)

**Target:** Vibhav (`feature/vibhav-integrity`)  
**Scope:** Part-2 GitHub Project Backlog Setup (ETA: 12th Oct 2026)  
**Project Board:** `Jackfruit Huffman Compressor` (`github.com/users/Saatwik-V/projects/3`)  
**Authority:** Aligned with `SE_Mini_Project_Delivereables_Part-2.pdf` and SRS v0.1.

---

## 1. Overview & Action Required

As part of Phase 2, each team member must enter their assigned User Stories and Functional Requirements into the GitHub Project board with **Story Points** (Fibonacci scale: `1, 2, 3, 5, 8`).

Please create the following **6 backlog issues** on GitHub and add them to our project board by **12th Oct 2026**.

---

## 2. Your Backlog Items & Story Points

| Issue Title | Target Sprint | Story Points | SRS Requirement Mapping | Primary Focus |
|---|---|:---:|---|---|
| `[SECURITY] Implement standalone IEEE 802.3 CRC-32 checksum engine` | **Sprint 1** (19–23 Oct) | **3 SP** | `JACK-F-015`, Security Spec | CRC-32 standard calculation |
| `[SECURITY] Support compressor pipeline with archive CRC-32 integration` | **Sprint 1** (19–23 Oct) | **2 SP** | Sprint 1 Milestone | Checksum in header |
| `[UC-04] Structural archive integrity & bounds validator` | **Sprint 2** (26–30 Oct) | **3 SP** | `JACK-F-013`, `JACK-F-014` | Fast-fail header inspection |
| `[UC-04] CRC-32 mismatch detection & interactive validate CLI` | **Sprint 2** (26–30 Oct) | **5 SP** | `JACK-F-015`, `JACK-F-016` | Payload CRC & exit codes |
| `[SECURITY] Memory sanitizers (ASan/UBSan) & malformed input fuzz suite` | **Sprint 2** (26–30 Oct) | **5 SP** | `JACK-SR-001..005` | Buffer overflow & fuzz testing |
| `[SECURITY] Author security review report & threat mitigations doc` | **Sprint 2** (26–30 Oct) | **2 SP** | STRIDE Mitigations | Security documentation |

**Total Allocation:** **20 Story Points** (5 SP in Sprint 1, 15 SP in Sprint 2).

---

## 3. Detailed Issue Copy-Paste Templates

### Issue 1: `[SECURITY] Implement standalone IEEE 802.3 CRC-32 checksum engine`
* **Assignee:** `@Vibhav`
* **Story Points:** `3`
* **Sprint:** `Sprint 1`
* **Labels:** `security`, `algorithm`, `UC-04`
* **Description:**
  ```markdown
  ### User Story
  As the integrity validation subsystem, I need a standalone IEEE 802.3 CRC-32 calculation engine (`0xEDB88320` polynomial) to compute checksums over byte streams and files.

  ### Requirements
  - Maps to `JACK-F-015` and Section 4.3 Component 5 in SAD.
  - Implement lookup-table-based CRC-32 calculation for performance.
  - Support incremental byte stream updates: `CRC32::update(currentCRC, buffer, length)`.

  ### Acceptance Criteria
  - [ ] Implemented in `include/security/crc32.hpp` and `src/security/crc32.cpp`.
  - [ ] Passes unit tests against standard vectors (empty string `0x00000000`, `"123456789"` $\implies$ `0xCBF43926`) in `tests/unit/test_crc32.cpp`.
  ```

---

### Issue 2: `[SECURITY] Support compressor pipeline with archive CRC-32 integration`
* **Assignee:** `@Vibhav`
* **Story Points:** `2`
* **Sprint:** `Sprint 1`
* **Labels:** `security`, `integration`, `milestone`
* **Description:**
  ```markdown
  ### Task
  Integrate CRC-32 calculation into Saatwik's compression pipeline so that every generated `.jack` archive stores the true uncompressed CRC-32 checksum in its header.

  ### Acceptance Criteria
  - [ ] Generated archive headers contain valid CRC-32 checksums.
  - [ ] Verified during the Sprint 1 working product round-trip demo.
  ```

---

### Issue 3: `[UC-04] Structural archive integrity & bounds validator`
* **Assignee:** `@Vibhav`
* **Story Points:** `3`
* **Sprint:** `Sprint 2`
* **Labels:** `security`, `UC-04`, `validation`
* **Description:**
  ```markdown
  ### User Story
  As an auditor, I want the tool to check the structural integrity of archive headers before allocating memory or decompressing payloads, safely rejecting malformed files.

  ### Requirements
  - Maps to `JACK-F-013`, `JACK-F-014`, and `JACK-SR-001..003`.
  - Validate magic bytes `0x4A41434B` (`JACK`) and format version `1`.
  - Validate symbol count bound $0 \le N \le 256$ and payload bit bounds.

  ### Acceptance Criteria
  - [ ] Non-JACK or corrupted files fail fast with exit code `2` (`ERROR_INVALID_HEADER`).
  - [ ] Unit tests pass in `tests/unit/test_integrity.cpp`.
  ```

---

### Issue 4: `[UC-04] CRC-32 mismatch detection & interactive validate CLI`
* **Assignee:** `@Vibhav`
* **Story Points:** `5`
* **Sprint:** `Sprint 2`
* **Labels:** `enhancement`, `ui`, `UC-04`
* **Description:**
  ```markdown
  ### User Story
  As a CLI user, I want to run `jackfruit validate <archive.jack>` or select option 4 from the menu to verify data integrity and detect if an archive has suffered bit-rot or payload corruption.

  ### Requirements
  - Maps to `JACK-F-015` and `JACK-F-016`.
  - Decode payload in memory, compute checksum, compare with header.
  - Exit code `0` on VALID, exit code `3` on CHECKSUM_MISMATCH.

  ### Acceptance Criteria
  - [ ] CLI displays color-coded validation report (`VALID` vs `CHECKSUM_MISMATCH`).
  - [ ] 1-bit corrupted payload detected reliably.
  ```

---

### Issue 5: `[SECURITY] Memory sanitizers (ASan/UBSan) & malformed input fuzz suite`
* **Assignee:** `@Vibhav`
* **Story Points:** `5`
* **Sprint:** `Sprint 2`
* **Labels:** `security`, `fuzzing`, `quality`
* **Description:**
  ```markdown
  ### Task
  As Security Lead, verify that the entire toolchain is memory-safe by running AddressSanitizer (ASan) and UndefinedBehaviorSanitizer (UBSan) against a suite of fuzzed and truncated files.

  ### Requirements
  - Maps to `JACK-NF-003` and `JACK-SR-001..005`.
  - Zero crashes, zero segmentation faults, zero buffer overflows on malformed inputs.

  ### Acceptance Criteria
  - [ ] ASan and UBSan run with 0 errors across fuzzed archives.
  ```

---

### Issue 6: `[SECURITY] Author security review report & threat mitigations doc`
* **Assignee:** `@Vibhav`
* **Story Points:** `2`
* **Sprint:** `Sprint 2`
* **Labels:** `documentation`, `security`
* **Description:**
  ```markdown
  ### Task
  Author `docs/validation/security-review.md` documenting the STRIDE threat mitigations, fuzzing outcomes, and sanitizer evidence for the final course evaluation.

  ### Acceptance Criteria
  - [ ] `docs/validation/security-review.md` created with threat coverage matrix.
  ```

---

## 4. Step-by-Step Instructions: How to Add Your Issues to GitHub Projects

Saatwik has set up the project board with custom fields (**Story Points**, **Sprint**) and tabs for each sprint. Follow these exact steps:

### Step 1: Open the Project Board
* Navigate to: **`https://github.com/users/Saatwik-V/projects/3`**
* You will see the board **`Jackfruit Huffman Compressor`** with three view tabs at the top:
  * **`View 1`**: Master board (displays all project items).
  * **`Sprint 1(19-23 Oct)`**: Filtered board displaying Sprint 1 deliverables.
  * **`Sprint 2 (26–30 Oct)`**: Filtered board displaying Sprint 2 deliverables.

### Step 2: Create Each Issue
1. Stay on **`View 1`** (or go to `Sprint 1(19-23 Oct)`).
2. In the **`Backlog`** column, click the **`+` (plus button)** at the top right of the column header (or click `+ Add item` at the bottom).
3. Paste the **Issue Title** (e.g. `[SECURITY] Implement standalone IEEE 802.3 CRC-32 checksum engine`).
4. A popup menu will appear directly above the typing bar:
   * Select **`Create new issue`** (press **Enter**).
5. In the creation modal that appears:
   * **Title:** Confirmed.
   * **Description:** Copy and paste the Markdown block (User Story, Requirements, Acceptance Criteria) from Section 3 above.
   * **Assignee:** Select `@Vibhav` (your GitHub account).
   * **Label:** Select `security`, `UC-04`.
   * Click the green **Create** button.

### Step 3: Assign Story Points and Sprint on the Card
1. As soon as you click Create, the card appears in the **`Backlog`** column.
2. **Click on the card** to open the side inspection panel:
   * Locate the **`Story Points`** field: Enter the exact number (e.g., `2`, `3`, or `5` as specified in the table).
   * Locate the **`Sprint`** field: Select **`Sprint 1`** or **`Sprint 2`**.
3. Close the side panel.

### Step 4: Verify Your Cards in the Sprint Views
* Click on the **`Sprint 1(19-23 Oct)`** tab: Your 2 Sprint 1 issues (`CRC-32 Engine` and `Compressor Integration`) should now be visible in the **Backlog** column with your avatar and Story Points badges!
* Click on the **`Sprint 2 (26–30 Oct)`** tab: Your 4 Sprint 2 issues (`Structural Validator`, `Validate CLI`, `ASan/Fuzzing`, `Security Review`) should be visible there!
* **Done!** Your backlog setup for Phase 2 is complete.
