# GitHub Backlog Guide: Tanmayi (Testing & Coverage Lead)

**Target:** Tanmayi (`feature/tanmayi-decompression`)  
**Scope:** Part-2 GitHub Project Backlog Setup (ETA: 12th Oct 2026)  
**Project Board:** `Jackfruit Huffman Compressor` (`github.com/users/Saatwik-V/projects/3`)  
**Authority:** Aligned with `SE_Mini_Project_Delivereables_Part-2.pdf` and SRS v0.1.

---

## 1. Overview & Action Required

As part of Phase 2, each team member is responsible for adding their assigned User Stories and Functional Requirements to our GitHub Project board with **Story Points** (using the Fibonacci scale: `1, 2, 3, 5, 8`).

Please create the following **6 backlog issues** on GitHub and add them to our project board by **12th Oct 2026**.

---

## 2. Your Backlog Items & Story Points

| Issue Title | Target Sprint | Story Points | SRS Requirement Mapping | Primary Focus |
|---|---|:---:|---|---|
| `[UC-02] Parse & validate fixed archive header` | **Sprint 1** (19–23 Oct) | **3 SP** | `JACK-F-005`, `JACK-SR-001` | Header parser & signature check |
| `[UC-02] Reconstruct Huffman decoding tree from metadata` | **Sprint 1** (19–23 Oct) | **5 SP** | `JACK-F-006` | Min-heap prefix tree builder |
| `[UC-02] BitReader sequential MSB-first bit extraction` | **Sprint 1** (19–23 Oct) | **3 SP** | `JACK-F-007` | Unaligned bit boundary parsing |
| `[UC-02] Round-trip byte-identical file restoration` | **Sprint 1** (19–23 Oct) | **5 SP** | `JACK-F-008` | Full decoder & byte reconstruction |
| `[TEST] Wire automated code coverage (gcov/lcov ≥80%) into CI` | **Sprint 2** (26–30 Oct) | **5 SP** | Testing Lead | CI coverage pipeline & report |
| `[TEST] Execute & record 10 baseline test cases` | **Sprint 2** (26–30 Oct) | **3 SP** | `TC-COMP/DECOMP/STAT/VALID` | IEEE 829 test records |

**Total Allocation:** **24 Story Points** (16 SP in Sprint 1, 8 SP in Sprint 2).

---

## 3. Detailed Issue Copy-Paste Templates

### Issue 1: `[UC-02] Parse & validate fixed archive header`
* **Assignee:** `@Tanmayi`
* **Story Points:** `3`
* **Sprint:** `Sprint 1`
* **Labels:** `enhancement`, `UC-02`, `decompression`
* **Description:**
  ```markdown
  ### User Story
  As a user decompressing an archive, I want the tool to read and validate the 18-byte fixed header of a `.jack` file so that non-jack archives or corrupted signatures are rejected immediately before reading data.

  ### Requirements
  - Maps to `JACK-F-005` and `JACK-SR-001`.
  - Validate magic bytes `0x4A 0x41 0x43 0x4B` (`JACK`).
  - Validate supported format version `0x01`.

  ### Acceptance Criteria
  - [ ] Correctly reads 18-byte header into `ArchiveHeader` struct.
  * [ ] Rejects files with invalid magic bytes with exit code `2` (`ERROR_INVALID_HEADER`).
  * [ ] Unit test passing in `tests/unit/test_archive_io.cpp`.
  ```

---

### Issue 2: `[UC-02] Reconstruct Huffman decoding tree from metadata`
* **Assignee:** `@Tanmayi`
* **Story Points:** `5`
* **Sprint:** `Sprint 1`
* **Labels:** `enhancement`, `algorithm`, `UC-02`
* **Description:**
  ```markdown
  ### User Story
  As a decompression engine, I need to rebuild the exact Huffman binary tree topology from the stored frequency metadata so that bit sequences can be decoded back into the original symbols.

  ### Requirements
  - Maps to `JACK-F-006`.
  - Reconstruct tree using min-heap priority queue identical to the compression algorithm.
  - Standardize tie-breaking on lowest byte value.

  ### Acceptance Criteria
  - [ ] Reconstructed tree topology perfectly mirrors the encoder's tree.
  - [ ] Single-character and all-256-symbol trees reconstructed cleanly.
  - [ ] Unit tests passing in `tests/unit/test_decoder.cpp`.
  ```

---

### Issue 3: `[UC-02] BitReader sequential MSB-first bit extraction`
* **Assignee:** `@Tanmayi`
* **Story Points:** `3`
* **Sprint:** `Sprint 1`
* **Labels:** `enhancement`, `io`, `UC-02`
* **Description:**
  ```markdown
  ### User Story
  As the bitstream consumer, I need a `BitReader` class that extracts bits MSB-first from the payload byte stream and halts strictly when `total_bits` is reached so that zero trailing padding bits are decoded.

  ### Requirements
  - Maps to `JACK-F-007` and `JACK-SR-003`.
  - Adhere to the MSB-first bit ordering signed off in `docs/design/archive-format.md`.
  - Terminate reading exactly when `bitsRead == totalBits`.

  ### Acceptance Criteria
  - [ ] Reads bits MSB-first matching `BitWriter` contract.
  - [ ] Boundary tests pass on unaligned bit counts (e.g. 1, 7, 9, 13 bits).
  - [ ] Unit tests passing in `tests/unit/test_bit_reader.cpp`.
  ```

---

### Issue 4: `[UC-02] Round-trip byte-identical file restoration`
* **Assignee:** `@Tanmayi`
* **Story Points:** `5`
* **Sprint:** `Sprint 1`
* **Labels:** `enhancement`, `milestone`, `UC-02`
* **Description:**
  ```markdown
  ### User Story
  As a user, I want to decompress a `.jack` archive into an output file that is byte-for-byte identical to the original input file.

  ### Requirements
  - Maps to `JACK-F-008` (Sprint 1 Team Milestone).
  - Stream decoded bytes directly to the target output file.
  - Partner with Saatwik to achieve working round-trip compression and decompression.

  ### Acceptance Criteria
  - [ ] CLI command `jackfruit decompress <input.jack> -o <restored>` works end-to-end.
  - [ ] Round-trip verification: `diff original restored` produces 0 differences on text, code, binary, and empty files.
  - [ ] Verified for inclusion in the required 1-minute working product demo video.
  ```

---

### Issue 5: `[TEST] Wire automated code coverage (gcov/lcov ≥80%) into CI`
* **Assignee:** `@Tanmayi`
* **Story Points:** `5`
* **Sprint:** `Sprint 2`
* **Labels:** `testing`, `ci`, `quality`
* **Description:**
  ```markdown
  ### Task
  As Testing Lead, wire gcov/lcov code coverage collection into our GitHub Actions CI workflow to monitor line and branch coverage across all modules.

  ### Requirements
  - Generate HTML and terminal coverage summaries on pull requests.
  - Enforce project coverage threshold: $\ge 80\%$ line coverage and $\ge 70\%$ branch coverage.

  ### Acceptance Criteria
  - [ ] CI step runs coverage analysis and uploads lcov report artifact.
  - [ ] All PRs display test coverage percentage.
  ```

---

### Issue 6: `[TEST] Execute & record 10 baseline test cases`
* **Assignee:** `@Tanmayi`
* **Story Points:** `3`
* **Sprint:** `Sprint 2`
* **Labels:** `testing`, `documentation`
* **Description:**
  ```markdown
  ### Task
  Execute the 10 baseline test cases defined in `docs/validation/test-plan-v0.1.md` (`TC-COMP-01..03`, `TC-DECOMP-01..03`, `TC-STAT-01`, `TC-VALID-01`, `TC-PERF-01`, `TC-SEC-01`) against the compiled binary and record actual results.

  ### Acceptance Criteria
  - [ ] All 10 test cases executed against release binary.
  - [ ] Pass/Fail results and actual outputs documented in `docs/validation/test-results.md`.
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
3. Paste the **Issue Title** (e.g. `[UC-02] Parse & validate fixed archive header`).
4. A popup menu will appear directly above the typing bar:
   * Select **`Create new issue`** (press **Enter**).
5. In the creation modal that appears:
   * **Title:** Confirmed.
   * **Description:** Copy and paste the Markdown block (User Story, Requirements, Acceptance Criteria) from Section 3 above.
   * **Assignee:** Select `@Tanmayi` (your GitHub account).
   * **Label:** Select `decompression`, `UC-02`.
   * Click the green **Create** button.

### Step 3: Assign Story Points and Sprint on the Card
1. As soon as you click Create, the card appears in the **`Backlog`** column.
2. **Click on the card** to open the side inspection panel:
   * Locate the **`Story Points`** field: Enter the exact number (e.g., `3` or `5` as specified in the table).
   * Locate the **`Sprint`** field: Select **`Sprint 1`** or **`Sprint 2`**.
3. Close the side panel.

### Step 4: Verify Your Cards in the Sprint Views
* Click on the **`Sprint 1(19-23 Oct)`** tab: Your 4 Sprint 1 issues (`JACK-F-005` to `008`) should now be visible in the **Backlog** column with your avatar and Story Points badges!
* Click on the **`Sprint 2 (26–30 Oct)`** tab: Your 2 Sprint 2 issues (`Coverage` & `Manual Tests`) should be visible there!
* **Done!** Your backlog setup for Phase 2 is complete.
