# Jackfruit System Architecture & Software Design (v0.1)

**Document Version:** 0.1.0 (Draft)  
**Status:** Sprint 1 Milestone  
**Lead Author:** Saatwik (Overall Lead, Architecture & UC-01 Owner)  
**Module Owners:** Tanmayi (UC-02), Yuvraj (UC-03), Vibhav (UC-04)

---

## 1. Architectural Vision & Quality Goals

The **Jackfruit Compression Tool** is an interactive, menu-driven CLI application written in modern C++17. Its primary architectural objective is clean modular separation of concerns using a **Layered Architecture Pattern**:
- **High Cohesion & Low Coupling:** Algorithm logic (Huffman coding) is strictly decoupled from file persistence (archive format serialization) and user interface (interactive prompts).
- **Testability:** Core algorithms operate on streams/in-memory buffers, allowing unit testing without file system dependencies.
- **Fail-Safe Integrity:** All archive parsing and bitstream decoding validate data bounds and check checksums before restoring files.

---

## 2. Layered Architecture Model

The system is decomposed into 4 vertical layers:

```mermaid
graph TD
    subgraph Layer 1: Presentation Layer
        CLI["CLI Interactive Shell (src/cli/)<br>• Menu Display & Input Prompts<br>• Real-time Progress Bar<br>• Colored Status Output"]
    end

    subgraph Layer 2: Application Facade Layer
        App["Jackfruit Application Controller<br>• Command Routing & Dispatch<br>• Session Management"]
    end

    subgraph Layer 3: Core Domain Modules
        Comp["Compression Module (Saatwik - UC-01)<br>src/compression/<br>• Frequency Analysis<br>• Huffman Tree Builder<br>• Canonical Code Generator<br>• Bitstream Encoder"]
        Decomp["Decompression Module (Tanmayi - UC-02)<br>src/decompression/<br>• Tree Reconstruction<br>• Bitstream Decoder<br>• Byte Reconstruction"]
        Arch["Archive & Statistics Module (Yuvraj - UC-03)<br>src/archive/<br>• Header Serializer / Deserializer<br>• Metadata Extraction<br>• Compression Metrics Calculator"]
        Integ["Integrity & Security Module (Vibhav - UC-04)<br>src/integrity/<br>• CRC-32 Checksum Calculator<br>• Structural Format Validator<br>• Boundary & Malformed Input Checker"]
    end

    subgraph Layer 4: Shared Infrastructure & I/O
        IO["Shared Bit & Stream I/O (src/io/)<br>• BitWriter (MSB-first accumulator)<br>• BitReader (MSB-first bit extraction)<br>• Binary File Helpers"]
    end

    CLI --> App
    App --> Comp
    App --> Decomp
    App --> Arch
    App --> Integ
    Comp --> Arch
    Comp --> IO
    Decomp --> Arch
    Decomp --> IO
    Arch --> Integ
    Integ --> IO
```

---

## 3. System Use-Case Diagram

The system supports four distinct primary use cases initiated by the user via the interactive menu:

```mermaid
flowchart LR
    User((User))

    subgraph Jackfruit System Boundary
        UC1(["UC-01: Compress File<br>(Owner: Saatwik)"])
        UC2(["UC-02: Decompress File<br>(Owner: Tanmayi)"])
        UC3(["UC-03: Inspect Statistics<br>(Owner: Yuvraj)"])
        UC4(["UC-04: Validate Archive Integrity<br>(Owner: Vibhav)"])
        
        SubCRC(["Calculate CRC-32"])
        SubTree(["Build / Parse Tree"])
        SubHeader(["Read / Write Header"])
    end

    User --> UC1
    User --> UC2
    User --> UC3
    User --> UC4

    UC1 -.->|includes| SubCRC
    UC1 -.->|includes| SubTree
    UC1 -.->|includes| SubHeader

    UC2 -.->|includes| SubHeader
    UC2 -.->|includes| SubTree
    UC2 -.->|includes| SubCRC

    UC3 -.->|includes| SubHeader
    UC4 -.->|includes| SubHeader
    UC4 -.->|includes| SubCRC
```

---

## 4. Component & Module Responsibilities

| Module | Directory | Primary Owner | Public Interface Header | Primary Responsibilities |
|---|---|---|---|---|
| **CLI Shell** | `src/cli/` | Saatwik | `include/cli/menu.hpp` | Runs main loop, prints banner, collects user paths, displays progress indicators. |
| **Compression** | `src/compression/` | Saatwik | `include/compression/compressor.hpp` | Computes byte frequencies, builds Huffman tree, assigns prefix codes, encodes bitstream. |
| **Decompression**| `src/decompression/`| Tanmayi | `include/decompression/decompressor.hpp`| Rebuilds decoding tree from header metadata, consumes bitstream, reconstructs original bytes. |
| **Archive** | `src/archive/` | Yuvraj | `include/archive/archive.hpp` | Serializes and parses the `.jack` file header, computes compression ratio and space savings. |
| **Integrity** | `src/integrity/` | Vibhav | `include/integrity/validator.hpp` | Calculates IEEE 802.3 CRC-32 checksums, validates header magic and version bounds. |
| **Shared I/O** | `src/io/` | Shared (Saatwik/Tanmayi) | `include/io/bit_stream.hpp` | Provides `BitWriter` (MSB-first packing) and `BitReader` (MSB-first decoding). |

---

## 5. Detailed Design: UC-01 Compression Module (Saatwik)

### 5.1 Class Diagram for Compression Subsystem

```mermaid
classDiagram
    class FrequencyTable {
        -uint64_t counts[256]
        -uint64_t totalBytes
        +FrequencyTable()
        +void countBytes(istream& input)
        +uint64_t getFrequency(uint8_t symbol) const
        +uint16_t getUniqueSymbolCount() const
        +vector~pair~uint8_t,uint64_t~~ getActiveSymbols() const
    }

    class HuffmanNode {
        +int symbol
        +uint64_t frequency
        +HuffmanNode* left
        +HuffmanNode* right
        +bool isLeaf() const
    }

    class HuffmanTree {
        -HuffmanNode* root
        -map~uint8_t, string~ codebook
        +HuffmanTree()
        +~HuffmanTree()
        +void buildFromFrequencies(const FrequencyTable& table)
        +const map~uint8_t, string~& getCodebook() const
        -void generateCodes(HuffmanNode* node, string currentCode)
        -void deleteTree(HuffmanNode* node)
    }

    class BitWriter {
        -ostream& out
        -uint8_t buffer
        -uint8_t bitCount
        -uint64_t totalBitsWritten
        +BitWriter(ostream& output)
        +~BitWriter()
        +void writeBit(uint8_t bit)
        +void writeBits(const string& bitString)
        +void flush()
        +uint64_t getTotalBitsWritten() const
    }

    class Compressor {
        +Compressor()
        +CompressionResult compress(const string& inputPath, const string& outputPath)
    }

    Compressor --> FrequencyTable : uses
    Compressor --> HuffmanTree : builds
    HuffmanTree *-- HuffmanNode : contains
    Compressor --> BitWriter : writes encoded data
```

---

### 5.2 Sequence Diagram: UC-01 End-to-End Compression Flow

```mermaid
sequenceDiagram
    autonumber
    actor User
    participant CLI as CLI Menu (src/cli)
    participant Comp as Compressor (src/compression)
    participant Freq as FrequencyTable
    participant Tree as HuffmanTree
    participant Arch as ArchiveWriter (src/archive)
    participant CRC as ChecksumValidator (src/integrity)
    participant BW as BitWriter (src/io)

    User->>CLI: Select 1. Compress File
    CLI->>User: Prompt for input file path
    User->>CLI: "sample.txt"
    CLI->>User: Prompt for output archive path (default: sample.txt.jack)
    User->>CLI: "sample.jack"

    CLI->>Comp: compress("sample.txt", "sample.jack")
    Comp->>CRC: computeCRC32(inputStream)
    CRC-->>Comp: crc32_value

    Comp->>Freq: countBytes(inputStream)
    Freq-->>Comp: symbol frequency table

    Comp->>Tree: buildFromFrequencies(Freq)
    Tree-->>Comp: Huffman codebook (symbol -> bitstring)

    Comp->>Arch: writeFixedHeader(magic, version, originalSize, crc32)
    Comp->>Arch: writeMetadata(symbol_count, active_frequencies)

    Comp->>BW: open stream at payload offset
    loop For each byte in original file
        Comp->>BW: writeBits(codebook[byte])
    end
    BW->>BW: flush() with zero-padding
    BW-->>Comp: total_bits_written

    Comp->>Arch: writePayloadBitHeader(total_bits_written)
    Comp-->>CLI: CompressionResult(success, originalSize, compressedSize, timeTaken)
    CLI->>User: Display success message with compression ratio & saved %
```

---

## 6. Public Interface Contracts (`include/`)

### 6.1 `include/compression/compressor.hpp` (Saatwik)
```cpp
#pragma once
#include <string>
#include <cstdint>

struct CompressionResult {
    bool success;
    std::string errorMessage;
    uint64_t originalBytes;
    uint64_t compressedBytes;
    double compressionRatio;
    double elapsedMilliseconds;
};

class Compressor {
public:
    CompressionResult compressFile(const std::string& inputFilePath,
                                   const std::string& outputArchivePath);
};
```

### 6.2 `include/io/bit_stream.hpp` (Saatwik & Tanmayi)
```cpp
#pragma once
#include <iostream>
#include <cstdint>
#include <string>

class BitWriter {
public:
    explicit BitWriter(std::ostream& outStream);
    ~BitWriter();
    void writeBit(uint8_t bit);
    void writeBits(const std::string& bitSequence);
    void flush();
    uint64_t getTotalBitsWritten() const;
private:
    std::ostream& out_;
    uint8_t buffer_{0};
    uint8_t bitCount_{0};
    uint64_t totalBits_{0};
};

class BitReader {
public:
    explicit BitReader(std::istream& inStream, uint64_t totalBits);
    bool readBit(uint8_t& bit);
    bool hasMoreBits() const;
private:
    std::istream& in_;
    uint64_t totalBits_;
    uint64_t bitsRead_{0};
    uint8_t buffer_{0};
    uint8_t bitIndex_{8}; // force read on first bit
};
```

---

## 7. Architectural Invariants
1. **Deterministic Code Generation:** For identical frequency inputs, the tree builder must break frequency ties deterministically (by lowest ASCII symbol value) to ensure identical prefix codes.
2. **Zero-Byte File Invariant:** Empty files are supported without errors; they produce a valid 28-byte `.jack` archive containing an empty symbol table.
3. **No Unchecked Buffer Access:** `BitReader` strictly stops at `total_bits`, preventing any out-of-bounds reads into trailing padding bits.
