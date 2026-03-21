# Raven

Raven is a clean, efficient C++ chess engine built from scratch. It uses a **bitboard representation** for high-performance move generation and board state management, and supports the standard **Universal Chess Interface (UCI)** protocol.

## 🚀 Key Features

- **Bitboard Engine**: High-performance 64-bit representation for all pieces, enabling fast bitwise move generation.
- **Full Chess Rules**: Supports all legal moves, including:
  - **Castling** (Kingside and Queenside).
  - **En Passant** captures.
  - **Pawn Promotion** (to Queen, Rook, Bishop, or Knight).
- **Search & Optimization**:
  - **Alpha-Beta Pruning**: Efficiently narrows the search tree.
  - **Iterative Deepening**: Ensures a best move is always ready when time runs out.
  - **Time Management**: Standard UCI time control support (`wtime`, `btime`, `winc`, `binc`).
- **UCI Protocol**: Fully compatible with chess GUIs like Cute Chess, Arena, or Stockfish GUI.
- **Position Support**: Can load games from any state using **FEN** strings.

## 📁 Source Overview

- `board.h/cpp`: Core board representation, bitboard management, and move application logic.
- `move_gen.h/cpp`: Comprehensive move generator including check detection and legal move validation.
- `engine.h/cpp`: The search brain, implementing Minimax with Alpha-Beta and Iterative Deepening.
- `uci.h/cpp`: Communication layer for the UCI protocol.
- `main.cpp`: Entry point that launches the UCI loop.

## 🛠️ Getting Started

### Prerequisites
- A C++ compiler (GCC or Clang recommended).
- C++17 support.

### Compilation
To build Raven, run:

```bash
g++ -std=c++17 main.cpp board.cpp move_gen.cpp uci.cpp engine.cpp -o raven
```

### Usage
Once compiled, you can run the engine directly (`./raven`) and type UCI commands, or plug the executable into your favorite Chess GUI.

Example manual commands:
```text
uci
isready
position startpos moves e2e4
go depth 6
```

## 📈 Roadmap (Future Improvements)

- [ ] **Zobrist Hashing**: To support Transposition Tables.
- [ ] **Transposition Tables**: To avoid redundant move calculations.
- [ ] **Quiescence Search**: To improve evaluation stability by searching captures until quiet positions are reached.
- [ ] **Move Ordering**: Heuristics like MVV-LVA or Killer Heuristic to speed up Alpha-Beta pruning.
- [ ] **Advanced PST**: More refined Piece-Square Tables for better positional understanding.
