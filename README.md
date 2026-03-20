# Raven

Raven is a clean, efficient C++ chess engine built from scratch. It uses **bitboard representation** for high-performance move generation and board state management.

## 🚀 Version 1 Features

- **Bitboard Engine**: All pieces (pawns, knights, bishops, rooks, queens, and kings) are represented by 64-bit integers (`uint64_t`), allowing for fast bitwise operations.
- **Move Generation**: Fully functional move generation for all piece types, including:
  - Knight, Bishop, Rook, Queen, and King move generation.
  - Pawn logic (single/double push and capture logic).
  - Sliding piece generation using optimized loops.
- **Legal Move Validation**: Detection of king check states.
- **Board Management**: Functions for applying moves to the board and tracking scores based on captures.
- **Evaluation Utilities**: Support for piece value mapping (PAWN=1, KNIGHT/BISHOP=3, ROOK=5, QUEEN=9).
- **Visualization Tools**: Utilities to print binary representations of bitboards to the console for easier debugging.

## 📁 Source Overview

- `board.h/cpp`: Core board representation, piece types, bitboard management, and move application.
- `move_gen.h/cpp`: The move generator implementation, including path calculation and check detection.
- `main.cpp`: Entry point for testing and running the engine.

## 🛠️ Getting Started

### Prerequisites
- A C++ compiler (GCC or Clang recommended).
- Support for C++11 or higher (C++17 recommended).

### Compilation
You can compile the project using a standard C++ compiler. To build the main test executable:

```bash
g++ -std=c++17 main.cpp board.cpp move_gen.cpp uci.cpp engine.cpp -o raven
```
*(Note: If you are using features that require C++17, use `-std=c++17`)*

## 📈 Next Steps
- Implement Alpha-Beta pruning for search optimization.
- Add support for Zobrist hashing.
- UCI (Universal Chess Interface) protocol support.
