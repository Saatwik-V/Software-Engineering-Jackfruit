# Sprint 2 Video Demo Guide (2 Minutes / 120 Seconds)

**Course:** UE24CS341A — Software Engineering Mini-Project (Part-2)  
**Deliverable:** 2-Minute Working Product Video Demo (`docs/media/sprint-2-demo.mp4`)  
**Target Deadline:** 30th October 2026 (Development Freeze)  
**Output Path:** `docs/media/sprint-2-demo.mp4`  

---

## 1. Video Specifications & Acceptance Criteria

Per **Course Mini-Project Deliverables Part-2 (Section 3.b)**:
- **Duration:** Strictly **2 minutes (110–120 seconds)**. Must not exceed 2:00.
- **Pacing Rule:** **NO LONG WAITS OR IDLE FREEZES!** Do not pause on static screens for 10–18 seconds. Keep typing/pasting brisk and continuous.
- **Feature Coverage:** Demonstrate **ALL 5 options of the Interactive Terminal Menu** (Compress, Decompress, Statistics, Validate, Exit), followed by byte-for-byte binary comparison (`fc.exe /b` or `cmp`), security tamper fault injection, and automated fuzz tests.
- **Resolution:** 1080p (1920×1080) or 720p (1280×720) Full HD widescreen.
- **Audio:** Clear spoken voiceover narration (ensure microphone is **UNMUTED** in your screen recording software).
- **Target File Size:** Under 25MB (save directly as `docs/media/sprint-2-demo.mp4`).

---

## 2. Pre-Recording Setup & Environment Preparation

Before pressing record, run this setup script in your terminal to ensure directories, encoding, and sample files are prepared cleanly.

### For Windows PowerShell (UCRT64 / MSYS2 / MinGW):
```powershell
# 1. Set terminal encoding to UTF-8 so menu box borders & dashes render cleanly
chcp 65001

# 2. Navigate to repository root and ensure clean main branch
git checkout main
git status

# 3. Create a clean demo sandbox directory
Remove-Item -Recurse -Force demo_sandbox -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path demo_sandbox | Out-Null
Set-Location demo_sandbox

# 4. Create an expressive, compressible sample text file (~3KB)
# (This produces a positive compression ratio of 1.63x and 38.7% space saved)
$content = "Software Engineering Jackfruit Lossless Huffman Compression Engine " * 45
Set-Content -Path demo_input.txt -Value $content -NoNewline

# 5. Clear the screen so your terminal starts fresh
Clear-Host
```

### For macOS / Linux (bash / zsh):
```bash
# 1. Navigate to repo root and clean main branch
git checkout main
git status

# 2. Create clean demo sandbox directory
rm -rf demo_sandbox && mkdir -p demo_sandbox
cd demo_sandbox

# 3. Create expressive, compressible sample text file (~3KB)
python3 -c "open('demo_input.txt', 'w').write('Software Engineering Jackfruit Lossless Huffman Compression Engine ' * 45)"

# 4. Clear the screen
clear
```

---

## 3. Second-by-Second Video Recording Script (0:00 – 2:00)

> [!IMPORTANT]
> **Pacing Instruction:** Move directly from command to command without idle pauses. The voiceover should narrate simultaneously as the terminal executes.

| Time Window | Segment Title | On-Screen Action (Without Pausing) | Spoken Voiceover Narration |
|---|---|---|---|
| **0:00 – 0:15** | **Introduction & Git Status** | Run: `git log -n 3 --oneline` | *"Welcome to the Sprint 2 demo of Jackfruit, a C++17 lossless compression tool developed by Saatwik, Tanmayi, Yuvraj, and Vibhav. In Sprint 2, we completed all four use cases, built an interactive terminal menu shell, and hardened the product with automated CI, coverage, and security sanitizers."* |
| **0:15 – 0:35** | **Interactive Menu: Option 1 (Compress)** | Launch `jackfruit`<br>Select `1`<br>Input: `demo_input.txt`<br>Output: `demo.jack` | *"We launch Jackfruit's interactive terminal menu. Choosing Option 1 runs Saatwik's compression engine: it builds the byte frequency table, generates the canonical Huffman tree, and writes the 18-byte `.jack` archive. Our 3KB input is compressed to 1.8KB in under one millisecond, saving ~39% space."* |
| **0:35 – 0:50** | **Interactive Menu: Option 3 (Statistics)** | In menu, select `3`<br>Input: `demo.jack` | *"Choosing Option 3 triggers Yuvraj's statistics engine. Without decompressing the file, it parses the binary header and displays a formatted table with compressed size, a 1.63x compression ratio, 24 unique symbols, average bits per symbol, and the stored CRC-32 checksum."* |
| **0:50 – 1:05** | **Interactive Menu: Option 4 (Validate)** | In menu, select `4`<br>Input: `demo.jack` | *"Choosing Option 4 runs Vibhav's integrity validator. It verifies magic bytes `JACK`, format versioning, recomputes the payload CRC-32, and confirms an identical checksum match, returning status PASS."* |
| **1:05 – 1:20** | **Interactive Menu: Option 2 (Decompress) & Option 5 (Exit)** | In menu, select `2`<br>Input: `demo.jack`<br>Output: `restored.txt`<br>Select `5` (Exit) | *"Choosing Option 2 runs Tanmayi's decompression engine, reconstructing the Huffman tree and decoding bits MSB-first to restore the payload. We then select Option 5 to cleanly exit the interactive menu."* |
| **1:20 – 1:30** | **Bit-for-Bit Lossless Verification** | Run: `fc.exe /b demo_input.txt restored.txt`<br>*(or `cmp demo_input.txt restored.txt`)* | *"To prove lossless reconstruction, we execute a binary file comparison. It reports zero differences encountered, proving 100% bit-for-bit lossless data restoration."* |
| **1:30 – 1:45** | **Security Fault Injection & Tamper Detection** | Run Python 1-byte tamper command.<br>Run: `jackfruit validate corrupt.jack`<br>Show exit code ($LASTEXITCODE / $?) | *"To demonstrate security and tamper detection, we inject a single-byte bit flip into the archive. Running the validator immediately detects the corruption, flags CHECKSUM_MISMATCH, and exits with a non-zero error code."* |
| **1:45 – 1:55** | **Automated Fuzz Tests** | Run: `jackfruit_tests --gtest_filter=FuzzSecurityTest*` | *"Under the hood, all 131 automated tests pass, including five continuous fuzz test suites that subject the decoder to random byte noise, bit flips, and boundary truncations with zero crashes."* |
| **1:55 – 2:00** | **Development Freeze Conclusion** | Run: `git status` | *"All 40 issues across Sprints 1 and 2 are closed, all four use cases are complete, and development is frozen for final evaluation. Thank you!"* |

---

## 4. Complete Copy-Paste Command Sheet

Keep this block ready to paste during recording:

### For Windows PowerShell:

```powershell
# 1. Recent commits (0:00 - 0:15)
git log -n 3 --oneline

# 2. Launch Interactive Menu (0:15 - 1:20)
# (Adjust path to ..\build-ucrt64-msys\jackfruit.exe or .\build\jackfruit.exe)
..\build-ucrt64-msys\jackfruit.exe
```

**Inside the Menu (Type smoothly, do not wait):**
1. Type `1` $\rightarrow$ Enter `demo_input.txt` $\rightarrow$ Enter `demo.jack` *(Option 1: Compress)*
2. Type `3` $\rightarrow$ Enter `demo.jack` *(Option 3: View Statistics)*
3. Type `4` $\rightarrow$ Enter `demo.jack` *(Option 4: Validate Integrity)*
4. Type `2` $\rightarrow$ Enter `demo.jack` $\rightarrow$ Enter `restored.txt` *(Option 2: Decompress)*
5. Type `5` *(Option 5: Exit Menu)*

```powershell
# 3. Verify bit-for-bit identity (1:20 - 1:30)
fc.exe /b demo_input.txt restored.txt

# 4. Inject 1-byte corruption & validate fault handling (1:30 - 1:45)
python -c "b=bytearray(open('demo.jack','rb').read()); b[-5]^=0xFF; open('corrupt.jack','wb').write(b)"
..\build-ucrt64-msys\jackfruit.exe validate corrupt.jack
echo "Exit code: $LASTEXITCODE"

# 5. Run automated malformed fuzz suite (1:45 - 1:55)
..\build-ucrt64-msys\jackfruit_tests.exe --gtest_filter=FuzzSecurityTest*

# 6. Show clean git working tree (1:55 - 2:00)
git status
```

---

### For macOS / Linux:

```bash
# 1. Recent commits (0:00 - 0:15)
git log -n 3 --oneline

# 2. Launch Interactive Menu (0:15 - 1:20)
../build/jackfruit
```

**Inside the Menu (Type smoothly, do not wait):**
1. Type `1` $\rightarrow$ Enter `demo_input.txt` $\rightarrow$ Enter `demo.jack` *(Option 1: Compress)*
2. Type `3` $\rightarrow$ Enter `demo.jack` *(Option 3: View Statistics)*
3. Type `4` $\rightarrow$ Enter `demo.jack` *(Option 4: Validate Integrity)*
4. Type `2` $\rightarrow$ Enter `demo.jack` $\rightarrow$ Enter `restored.txt` *(Option 2: Decompress)*
5. Type `5` *(Option 5: Exit Menu)*

```bash
# 3. Verify bit-for-bit identity (1:20 - 1:30)
diff -s demo_input.txt restored.txt

# 4. Inject 1-byte corruption & validate fault handling (1:30 - 1:45)
python3 -c "b=bytearray(open('demo.jack','rb').read()); b[-5]^=0xFF; open('corrupt.jack','wb').write(b)"
../build/jackfruit validate corrupt.jack
echo "Exit code: $?"

# 5. Run automated malformed fuzz suite (1:45 - 1:55)
./build/jackfruit_tests --gtest_filter=FuzzSecurityTest*

# 6. Show clean git working tree (1:55 - 2:00)
git status
```

---

## 5. Post-Recording Encoding & Saving

1. Ensure the recorded video includes your voiceover narration and is within 110–120 seconds.
2. Save or copy the MP4 file directly to:
   ```text
   docs/media/sprint-2-demo.mp4
   ```
3. Commit and push:
   ```bash
   git add docs/media/sprint-2-demo.mp4
   git commit -m "docs(media): Add 2-minute Sprint 2 working product video demo"
   git push origin main
   ```
