#include "board.h"

using namespace std;

std::map<PieceType, int> PiecePoint = {{PAWN, 1}, {KNIGHT, 3}, {BISHOP, 3},
                                       {ROOK, 5}, {QUEEN, 9},  {KING, 10000}};

void boardUpdate(Board &board) {
  board.whiteBoard = board.whitePawns | board.whiteKnights |
                     board.whiteBishops | board.whiteQueen | board.whiteRooks |
                     board.whiteKing;
  board.blackBoard = board.blackPawns | board.blackKnights |
                     board.blackBishops | board.blackRooks | board.blackQueen |
                     board.blackKing;

  board.allPieces = board.whiteBoard | board.blackBoard;
}

void updateBoard(Board &board) { boardUpdate(board); }

bool isOccupied(uint64_t sq, Board &board) { return sq & board.allPieces; }

string toBinary(uint64_t x) { return bitset<64>(x).to_string(); }

void printBoard(string s) {
  for (int i = 0; i < 64; i += 8) {
    for (int j = 7; j >= 0; j--) {
      cout << s[i + j] << " ";
    }
    cout << endl;
  }
}

void makeMove(Board &board, Move &move, bool isWhite) {
  uint64_t from = 1ULL << (move.from - 1);
  uint64_t to = 1ULL << (move.to - 1);
  PieceType piece = move.piece;

  if (isWhite) {
    switch (piece) {
    case PAWN:
      board.whitePawns ^= from;
      board.whitePawns |= to;
      break;
    case KNIGHT:
      board.whiteKnights ^= from;
      board.whiteKnights |= to;
      break;
    case BISHOP:
      board.whiteBishops ^= from;
      board.whiteBishops |= to;
      break;
    case ROOK:
      board.whiteRooks ^= from;
      board.whiteRooks |= to;
      break;
    case QUEEN:
      board.whiteQueen ^= from;
      board.whiteQueen |= to;
      break;
    case KING:
      board.whiteKing ^= from;
      board.whiteKing |= to;
      break;
    default:
      break;
    }
  } else {
    switch (piece) {
    case PAWN:
      board.blackPawns ^= from;
      board.blackPawns |= to;
      break;
    case KNIGHT:
      board.blackKnights ^= from;
      board.blackKnights |= to;
      break;
    case BISHOP:
      board.blackBishops ^= from;
      board.blackBishops |= to;
      break;
    case ROOK:
      board.blackRooks ^= from;
      board.blackRooks |= to;
      break;
    case QUEEN:
      board.blackQueen ^= from;
      board.blackQueen |= to;
      break;
    case KING:
      board.blackKing ^= from;
      board.blackKing |= to;
      break;
    default:
      break;
    }
  }

  if (move.capturedPiece != NONE) {
    if (isWhite) {
      switch (move.capturedPiece) {
      case PAWN:
        board.blackPawns ^= to;
        board.whiteScore += PiecePoint[PAWN];
        break;
      case KNIGHT:
        board.blackKnights ^= to;
        board.whiteScore += PiecePoint[KNIGHT];
        break;
      case BISHOP:
        board.blackBishops ^= to;
        board.whiteScore += PiecePoint[BISHOP];
        break;
      case ROOK:
        board.blackRooks ^= to;
        board.whiteScore += PiecePoint[ROOK];
        break;
      case QUEEN:
        board.blackQueen ^= to;
        board.whiteScore += PiecePoint[QUEEN];
        break;
      case KING:
        board.blackKing ^= to;
        board.whiteScore += PiecePoint[KING];
        break;
      default:
        break;
      }
    } else {
      switch (move.capturedPiece) {
      case PAWN:
        board.whitePawns ^= to;
        board.blackScore += PiecePoint[PAWN];
        break;
      case KNIGHT:
        board.whiteKnights ^= to;
        board.blackScore += PiecePoint[KNIGHT];
        break;
      case BISHOP:
        board.whiteBishops ^= to;
        board.blackScore += PiecePoint[BISHOP];
        break;
      case ROOK:
        board.whiteRooks ^= to;
        board.blackScore += PiecePoint[ROOK];
        break;
      case QUEEN:
        board.whiteQueen ^= to;
        board.blackScore += PiecePoint[QUEEN];
        break;
      case KING:
        board.whiteKing ^= to;
        board.blackScore += PiecePoint[KING];
        break;
      default:
        break;
      }
    }
  }
  updateBoard(board);
}
