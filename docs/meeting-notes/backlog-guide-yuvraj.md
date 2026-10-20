# GitHub Backlog Guide: Yuvraj (CI/CD & Archive Lead)

**Target:** Yuvraj (`feature/yuvraj-statistics`)  
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
| `[ARCHIVE] Implement ArchiveWriter for 18-byte header & metadata serialization` | **Sprint 1** (19–23 Oct) | **5 SP** | `JACK-F-004`, Architecture | Header serialization (CRITICAL PATH) |
| `[ARCHIVE] Implement ArchiveReader for header validation & metadata parsing` | **Sprint 1** (19–23 Oct) | **3 SP** | `JACK-F-005`, Architecture | Binary parsing & seek |
| `[CI/CD] Automate PR builds & CTest test runner via GitHub Actions` | **Sprint 1** (19–23 Oct) | **3 SP** | CI/CD Baseline | Continuous Integration gates |
| `[UC-03] Fast metadata inspection & ratio computation engine` | **Sprint 2** (26–30 Oct) | **3 SP** | `JACK-F-009`, `JACK-F-010` | Math & ratio analytics |
| `[UC-03] Formatted statistics terminal table & CLI command` | **Sprint 2** (26–30 Oct) | **3 SP** | `JACK-F-011`, `JACK-F-012` | Terminal UI display table |
| `[CI/CD] Wire SonarCloud static analysis & automated release packaging` | **Sprint 2** (26–30 Oct) | **5 SP** | Quality & Packaging | Quality Gate & Zip release |

**Total Allocation:** **22 Story Points** (11 SP in Sprint 1, 11 SP in Sprint 2).

---

## 3. Detailed Issue Copy-Paste Templates

### Issue 1: `[ARCHIVE] Implement ArchiveWriter for 18-byte header & metadata serialization`
* **Assignee:** `@Yuvraj`
* **Story Points:** `5`
* **Sprint:** `Sprint 1` (CRITICAL PATH — Blocks Saatwik's compression and Tanmayi's decompression)
* **Labels:** `enhancement`, `archive`, `critical-path`
* **Description:**
  ```markdown
  ### User Story
  As the compression system, I need an `ArchiveWriter` class that serializes the fixed 18-byte `.jack` header, symbol frequency dictionary, and bitstream payload into Big-Endian binary format.

  ### Requirements
  - Maps to `JACK-F-004` and `docs/design/archive-format.md`.
  - Fixed 18-byte header: Magic (`JACK`), Version (`0x01`), Flags (`0x00`), Original Size (`uint64_t`), Checksum CRC-32 (`uint32_t`), Symbol Count (`uint16_t`), Total Bits (`uint64_t`).
  - Serializes frequency entries: pairs of `(uint8_t symbol, uint64_t freq)`.

  ### Acceptance Criteria
  - [ ] Implemented in `include/archive/archive_writer.hpp` and `src/archive/archive_writer.cpp`.
  - [ ] Multi-byte integers written in Big-Endian network byte order.
  - [ ] Unit tests pass in `tests/unit/test_archive_io.cpp`.
  ```

---

### Issue 2: `[ARCHIVE] Implement ArchiveReader for header validation & metadata parsing`
* **Assignee:** `@Yuvraj`
* **Story Points:** `3`
* **Sprint:** `Sprint 1`
* **Labels:** `enhancement`, `archive`, `io`
* **Description:**
  ```markdown
  ### User Story
  As the decompression and statistics subsystems, I need an `ArchiveReader` class to parse the binary header and symbol table from disk without needing to read the entire payload.

  ### Requirements
  - Maps to `JACK-F-005` and `JACK-F-009`.
  - Reads 18-byte header, validates magic bytes, and extracts `symbol_count` and `total_bits`.

  ### Acceptance Criteria
  - [ ] Implemented in `include/archive/archive_reader.hpp` and `src/archive/archive_reader.cpp`.
  - [ ] Round-trip unit test (write then read) restores identical header struct.
  - [ ] Gracefully fails on non-jack files.
  ```

---

### Issue 3: `[CI/CD] Automate PR builds & CTest test runner via GitHub Actions`
* **Assignee:** `@Yuvraj`
* **Story Points:** `3`
* **Sprint:** `Sprint 1`
* **Labels:** `ci`, `devops`, `infrastructure`
* **Description:**
  ```markdown
  ### Task
  As CI/CD Lead, configure and maintain our GitHub Actions workflow (`.github/workflows/ci.yml`) to automatically build the project and execute all GoogleTests on every pull request and push to `main`.

  ### Requirements
  - Aligned with `SE_Mini_Project_Delivereables_Part-2.pdf` requirement 2.b.
  - Matrix builds across Linux (Ubuntu) with compiler warnings as errors (`-Wall -Wextra`).

  ### Acceptance Criteria
  - [ ] GitHub Actions workflow triggers on PRs and commits.
  - [ ] PRs require green CTest status before merge.
  ```

---

### Issue 4: `[UC-03] Fast metadata inspection & ratio computation engine`
* **Assignee:** `@Yuvraj`
* **Story Points:** `3`
* **Sprint:** `Sprint 2`
* **Labels:** `enhancement`, `UC-03`, `math`
* **Description:**
  ```markdown
  ### User Story
  As a user inspecting archive properties, I want the tool to calculate file metrics (uncompressed size, compressed size, ratio, space saved) in under 50 ms without decompressing the payload.

  ### Requirements
  - Maps to `JACK-F-009` and `JACK-F-010`.
  - Compute Compression Ratio ($S_{\text{orig}} / S_{\text{comp}}$) and Percentage Space Saved ($(1 - S_{\text{comp}} / S_{\text{orig}}) \times 100\%$).
  - Guard against division by zero on 0-byte original files (`N/A`).

  ### Acceptance Criteria
  - [ ] Implemented in `include/stats/statistics_engine.hpp` and `src/stats/statistics_engine.cpp`.
  - [ ] Inspection execution time $\le 50$ ms.
  - [ ] Unit tests pass in `tests/unit/test_statistics.cpp`.
  ```

---

### Issue 5: `[UC-03] Formatted statistics terminal table & CLI command`
* **Assignee:** `@Yuvraj`
* **Story Points:** `3`
* **Sprint:** `Sprint 2`
* **Labels:** `enhancement`, `ui`, `UC-03`
* **Description:**
  ```markdown
  ### User Story
  As a CLI user, I want to run `jackfruit stat <file.jack>` or select option 3 from the interactive menu to view a clean, aligned metrics summary table.

  ### Requirements
  - Maps to `JACK-F-011` and `JACK-F-012`.
  - Display original size, compressed size, space saved percentage, ratio, CRC-32 checksum, and inspection duration.

  ### Acceptance Criteria
  - [ ] `jackfruit stat <file.jack>` outputs clean ASCII formatted table.
  - [ ] Gracefully prints error message on missing or corrupted archives.
  ```

---

### Issue 6: `[CI/CD] Wire SonarCloud static analysis & automated release packaging`
* **Assignee:** `@Yuvraj`
* **Story Points:** `5`
* **Sprint:** `Sprint 2`
* **Labels:** `ci`, `sonarcloud`, `release`
* **Description:**
  ```markdown
  ### Task
  As CI/CD Lead, configure SonarCloud code quality scanner in GitHub Actions and add an automated release packaging job that zips the compiled binary into a distributable archive on merge to `main`.

  ### Acceptance Criteria
  - [ ] SonarCloud workflow running on PRs; Quality Gate passing.
  - [ ] GitHub Actions generates and uploads release `.zip` artifact.
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
3. Paste the **Issue Title** (e.g. `[ARCHIVE] Implement ArchiveWriter for 18-byte header & metadata serialization`).
4. A popup menu will appear directly above the typing bar:
   * Select **`Create new issue`** (press **Enter**).
5. In the creation modal that appears:
   * **Title:** Confirmed.
   * **Description:** Copy and paste the Markdown block (User Story, Requirements, Acceptance Criteria) from Section 3 above.
   * **Assignee:** Select `@Yuvraj` (your GitHub account).
   * **Label:** Select `archive`, `ci`, or `UC-03`.
   * Click the green **Create** button.

### Step 3: Assign Story Points and Sprint on the Card
1. As soon as you click Create, the card appears in the **`Backlog`** column.
2. **Click on the card** to open the side inspection panel:
   * Locate the **`Story Points`** field: Enter the exact number (e.g., `3` or `5` as specified in the table).
   * Locate the **`Sprint`** field: Select **`Sprint 1`** or **`Sprint 2`**.
3. Close the side panel.

### Step 4: Verify Your Cards in the Sprint Views
* Click on the **`Sprint 1(19-23 Oct)`** tab: Your 3 Sprint 1 issues (`ArchiveWriter`, `ArchiveReader`, `CI Actions`) should now be visible in the **Backlog** column with your avatar and Story Points badges!
* Click on the **`Sprint 2 (26–30 Oct)`** tab: Your 3 Sprint 2 issues (`Statistics Engine`, `Stats CLI`, `SonarCloud & Packaging`) should be visible there!
* **Done!** Your backlog setup for Phase 2 is complete.
