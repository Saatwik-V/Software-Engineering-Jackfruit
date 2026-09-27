# Document 0 — Repo & Project Setup Guide (Saatwik)

**Project:** Jackfruit Mini-Project — Basic ZIP-style File Compression Tool (Huffman Coding)
**Course:** UE24CS341A — Software Engineering
**Your role:** Overall Lead, Repository/Integration Owner, Architecture & Code-Review Lead, Owner of UC-01 (Compress)

This is your Day-0 checklist. Nobody else on the team can start real work until the items in Section 1–4 exist. Do these in order, in one sitting if possible, then move to your own `01-MEMBER-GUIDE-Saatwik.md`.

> **Note on tools:** The official course slide brief (not the older generic guideline doc) names **GitHub Actions** for CI/CD and **SonarCloud** for static analysis — use these, not Jenkins/SonarQube. Docker is explicitly marked **optional** ("Zip file ok" for deployment), so don't burn setup time on it until the core pipeline works. For the Agile board, use **GitHub Projects + Issues** (built into GitHub, zero extra sign-ups) instead of Jira — it's fully acceptable as "Jira or similar tool" and is far less setup overhead for a 4-person student team.

---

## 1. Repository creation

1. Create the GitHub repo (private is fine): e.g. `jackfruit-huffman-compressor`.
2. Add Tanmayi, Yuvraj, Vibhav as collaborators with **Write** access.
3. Set the default branch to `main`.
4. Go to **Settings → Branches → Branch protection rules** and protect `main`:
   - Require a pull request before merging.
   - Require at least **1 approving review**.
   - Require status checks to pass before merging (you'll tick the actual CI check once Section 5 exists).
   - Disable force-pushes and branch deletion on `main`.

This one step is what makes "everyone works on their own branch" actually enforced instead of a suggestion.

## 2. Folder structure

Create this skeleton and commit it directly to `main` (this is the only commit anyone makes straight to `main` without a PR — everything after this goes through PRs):

```
jackfruit-huffman-compressor/
├── src/
│   ├── cli/            # interactive menu + command parsing
│   ├── compression/    # frequency table, Huffman tree, encoder   (Saatwik)
│   ├── decompression/  # decoder, reconstruction                 (Tanmayi)
│   ├── archive/        # archive header, metadata, statistics    (Yuvraj)
│   ├── integrity/      # checksum/CRC, validation                (Vibhav)
│   └── io/             # shared file + bit-level I/O
├── include/             # public headers for the above modules
├── tests/
│   ├── unit/
│   ├── integration/
│   └── system/
├── docs/
│   ├── srs/
│   ├── design/            # UML exports + .drawio source files
│   ├── validation/
│   ├── team-guides/        # <- these 6 documents live here
│   └── meeting-notes/
├── scripts/
├── .github/
│   └── workflows/          # GitHub Actions YAML files
├── CMakeLists.txt
├── README.md
├── .gitignore
└── LICENSE   (optional)
```

## 3. README.md — minimum content

Your README is the first thing a grader opens. Include:
- Project name + one-line description ("Basic ZIP-style file compression tool using Huffman coding, with an interactive CLI").
- Team names + roles (pull directly from the ownership table below).
- Build instructions (CMake commands).
- How to run the interactive tool.
- Link to `docs/` (SRS, design, validation, team guides).
- Badge/link to the GitHub Actions workflow status once it exists.

## 4. Ownership table — put this in the README and in your GitHub Project

| Member | Primary use case (their functionality) | Branch | Secondary role |
|---|---|---|---|
| **Saatwik** | UC-01 Compress File | `feature/saatwik-compression` | Overall lead, architecture, main-branch integrator |
| **Tanmayi** | UC-02 Decompress File | `feature/tanmayi-decompression` | Testing & coverage lead |
| **Yuvraj** | UC-03 Inspect Compression Statistics | `feature/yuvraj-statistics` | CI/CD (GitHub Actions) lead |
| **Vibhav** | UC-04 Validate Archive Integrity | `feature/vibhav-integrity` | Security & SonarCloud lead |

Each person's interactive-UI screen (menu option, prompts, progress/output formatting) is built **by that person, inside their own module** — there is no separate "UI person." This matters because the course guideline explicitly says UI alone cannot count as anyone's full functionality; here it's layered onto real backend work instead.

## 5. Interactive UI — what "interactive" means here

Your instructor's requirement is satisfied with a **menu-driven interactive CLI** (not a full GUI, not required for a C/C++ course project):
- On launch, the tool shows a numbered menu: `1. Compress  2. Decompress  3. Statistics  4. Validate  5. Exit`.
- Each option prompts the user for input (file paths) interactively rather than requiring command-line flags only.
- Show live feedback: progress indicators during compression, a formatted results table for statistics, clear colored success/error messages.
- You (Saatwik) own `src/cli/` — the shell that shows the menu and routes to each person's module. Each teammate wires their own menu option's interaction into it via their PR.
- This is testable, demoable, and doesn't blow up scope. If time permits later, a small web dashboard (static HTML calling the binary) can be a stretch/novelty add-on — not a baseline requirement.

## 6. Branching model

```
main                              ← protected, PR + review + CI required
 ├─ feature/saatwik-compression
 ├─ feature/tanmayi-decompression
 ├─ feature/yuvraj-statistics
 └─ feature/vibhav-integrity
```

Rules for everyone (including you):
- Never push directly to `main`.
- Branch off the latest `main` before starting new work; pull/rebase regularly so your branch doesn't drift too far.
- Small, frequent PRs beat one giant PR at the end — merge in vertical slices (see each member's sprint table).
- Commit message convention: `feat(compression): build frequency table`, `test(decompression): add truncated-header case`, `fix(integrity): correct CRC edge case`.
- You review every PR for architecture/interface consistency; the module owner's teammate reviews for correctness. Two eyes minimum before merge.

## 7. CMake + first CI run (prove the pipeline works before anyone builds features)

1. Write a minimal `CMakeLists.txt` that compiles an empty `main.cpp` printing "Jackfruit compressor" and links GoogleTest for a placeholder test target.
2. Add `.github/workflows/ci.yml` that on every push/PR: checks out code, configures CMake, builds, runs `ctest`.
3. Push this to `main` directly (last direct push allowed), confirm the Actions tab shows a green check.
4. Now go back to Section 1 and turn on "require status checks" in branch protection, selecting this workflow.

Once this is green, tell the team to start pulling `main` and creating their branches — this is the "go" signal for Sprint 1.

## 8. GitHub Project board (your Jira substitute)

1. Create a GitHub Project (Board view) linked to the repo.
2. Columns: `Backlog → Ready → In Progress → In Review → Done`.
3. Create one Epic-style label per area: `compression`, `decompression`, `statistics`, `integrity`, `cli`, `devops`, `docs`.
4. Create the first batch of issues (one per module's first vertical slice — see each member's guide, Sprint 1–2 rows) before anyone writes code. This is what "requirements before code" evidence looks like to the grader.
5. Reference the issue number in branch names/commits/PR titles, e.g. `feat(compression): frequency table (#12)`.

## 9. Day-0 checklist (tick before ending your first session)

- [ ] Repo created, teammates invited with write access
- [ ] `main` branch protection enabled (PR + 1 review + status check)
- [ ] Folder structure committed
- [ ] README with team table and build instructions committed
- [ ] Minimal CMake + placeholder test compiles locally
- [ ] `.github/workflows/ci.yml` added and green on `main`
- [ ] GitHub Project board created with initial issues for Sprint 1
- [ ] All 4 feature branches created (empty, just branched off `main`)
- [ ] `docs/team-guides/` created and this file + the other 5 committed there
- [ ] Everyone has read their own member guide

Once every box above is checked, move to **`01-MEMBER-GUIDE-Saatwik.md`** — that's your own sprint-by-sprint instructions as a contributor, same format as the other three.
