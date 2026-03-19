#include "board.h"
#include "move_gen.h"

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

// source: https://www.chessprogramming.org/Piece-Square_Tables

// Positional Evaluation Tables
// reward pawns for being in the center and penalize them for being on the edges
const int PawnPST[64] = {0,  0,  0,  0,  0,  0,   0,  0,  5,   10, 10, -20, -20,
                         10, 10, 5,  5,  -5, -10, 0,  0,  -10, -5, 5,  0,   0,
                         0,  20, 20, 0,  0,  0,   5,  5,  10,  25, 25, 10,  5,
                         5,  10, 10, 20, 30, 30,  20, 10, 10,  50, 50, 50,  50,
                         50, 50, 50, 50, 0,  0,   0,  0,  0,   0,  0,  0};

// reward knights for being in the center and penalize them for being on the
// edges
const int KnightPST[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50, -40, -20, 0,   0,   0,
    0,   -20, -40, -30, 0,   10,  15,  15,  10,  0,   -30, -30, 5,
    15,  20,  20,  15,  5,   -30, -30, 0,   15,  20,  20,  15,  0,
    -30, -30, 5,   10,  15,  15,  10,  5,   -30, -40, -20, 0,   5,
    5,   0,   -20, -40, -50, -40, -30, -30, -30, -30, -40, -50};

int evaluatePiece(uint64_t pieces, const int pst[64], bool isWhite) {
  int score = 0;
  while (pieces) {
    int sq = __builtin_ctzll(pieces);
    int pstSq = isWhite ? sq : (sq ^ 56); // Flip for black
    score += pst[pstSq];
    pieces &= pieces - 1;
  }
  return score;
}

int evaluate(Board &board) {
  int whiteScore = 0;
  int blackScore = 0;

  // Material
  whiteScore += __builtin_popcountll(board.whitePawns) * 100;
  whiteScore += __builtin_popcountll(board.whiteKnights) * 320;
  whiteScore += __builtin_popcountll(board.whiteBishops) * 330;
  whiteScore += __builtin_popcountll(board.whiteRooks) * 500;
  whiteScore += __builtin_popcountll(board.whiteQueen) * 900;

  blackScore += __builtin_popcountll(board.blackPawns) * 100;
  blackScore += __builtin_popcountll(board.blackKnights) * 320;
  blackScore += __builtin_popcountll(board.blackBishops) * 330;
  blackScore += __builtin_popcountll(board.blackRooks) * 500;
  blackScore += __builtin_popcountll(board.blackQueen) * 900;

  // Positional (PST)
  whiteScore += evaluatePiece(board.whitePawns, PawnPST, true);
  whiteScore += evaluatePiece(board.whiteKnights, KnightPST, true);

  blackScore += evaluatePiece(board.blackPawns, PawnPST, false);
  blackScore += evaluatePiece(board.blackKnights, KnightPST, false);

  return whiteScore - blackScore;
}

string toBinary(uint64_t x) { return bitset<64>(x).to_string(); }

void printBoard(string s) {
  for (int i = 0; i < 64; i += 8) {
    for (int j = 7; j >= 0; j--) {
      cout << s[i + j] << " ";
    }
    cout << endl;
  }
}

void unMakeMove(Board &board, Move &move, bool isWhite) {
  uint64_t from = 1ULL << (move.from - 1);
  uint64_t to = 1ULL << (move.to - 1);
  PieceType piece = move.piece;

  if (isWhite) {
    switch (piece) {
    case PAWN:
      board.whitePawns ^= to;
      board.whitePawns |= from;
      break;
    case KNIGHT:
      board.whiteKnights ^= to;
      board.whiteKnights |= from;
      break;
    case BISHOP:
      board.whiteBishops ^= to;
      board.whiteBishops |= from;
      break;
    case ROOK:
      board.whiteRooks ^= to;
      board.whiteRooks |= from;
      break;
    case QUEEN:
      board.whiteQueen ^= to;
      board.whiteQueen |= from;
      break;
    case KING:
      board.whiteKing ^= to;
      board.whiteKing |= from;
      break;
    default:
      break;
    }
  } else {
    switch (piece) {
    case PAWN:
      board.blackPawns ^= to;
      board.blackPawns |= from;
      break;
    case KNIGHT:
      board.blackKnights ^= to;
      board.blackKnights |= from;
      break;
    case BISHOP:
      board.blackBishops ^= to;
      board.blackBishops |= from;
      break;
    case ROOK:
      board.blackRooks ^= to;
      board.blackRooks |= from;
      break;
    case QUEEN:
      board.blackQueen ^= to;
      board.blackQueen |= from;
      break;
    case KING:
      board.blackKing ^= to;
      board.blackKing |= from;
      break;
    default:
      break;
    }
  }

  if (move.capturedPiece != NONE) {
    if (isWhite) {
      switch (move.capturedPiece) {
      case PAWN:
        board.blackPawns |= to;
        break;
      case KNIGHT:
        board.blackKnights |= to;
        break;
      case BISHOP:
        board.blackBishops |= to;
        break;
      case ROOK:
        board.blackRooks |= to;
        break;
      case QUEEN:
        board.blackQueen |= to;
        break;
      case KING:
        board.blackKing |= to;
        break;
      default:
        break;
      }
    } else {
      switch (move.capturedPiece) {
      case PAWN:
        board.whitePawns |= to;
        break;
      case KNIGHT:
        board.whiteKnights |= to;
        break;
      case BISHOP:
        board.whiteBishops |= to;
        break;
      case ROOK:
        board.whiteRooks |= to;
        break;
      case QUEEN:
        board.whiteQueen |= to;
        break;
      case KING:
        board.whiteKing |= to;
        break;
      default:
        break;
      }
    }
  }
  updateBoard(board);
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
        break;
      case KNIGHT:
        board.blackKnights ^= to;
        break;
      case BISHOP:
        board.blackBishops ^= to;
        break;
      case ROOK:
        board.blackRooks ^= to;
        break;
      case QUEEN:
        board.blackQueen ^= to;
        break;
      case KING:
        board.blackKing ^= to;
        break;
      default:
        break;
      }
    } else {
      switch (move.capturedPiece) {
      case PAWN:
        board.whitePawns ^= to;
        break;
      case KNIGHT:
        board.whiteKnights ^= to;
        break;
      case BISHOP:
        board.whiteBishops ^= to;
        break;
      case ROOK:
        board.whiteRooks ^= to;
        break;
      case QUEEN:
        board.whiteQueen ^= to;
        break;
      case KING:
        board.whiteKing ^= to;
        break;
      default:
        break;
      }
    }
  }
  updateBoard(board);
}

string moveToUCI(Move move) {
  int fromSq = move.from - 1;
  int toSq = move.to - 1;

  char fromFile = 'a' + (fromSq % 8);
  char fromRank = '1' + (fromSq / 8);
  char toFile = 'a' + (toSq % 8);
  char toRank = '1' + (toSq / 8);

  string res = "";
  res += fromFile;
  res += fromRank;
  res += toFile;
  res += toRank;
  return res;
}

string moveToSAN(Board &board, Move move, bool isWhite) {
  if (move.from == 0)
    return "none";

  string res = "";
  int fromSq = move.from - 1;
  int toSq = move.to - 1;
  char fromFile = 'a' + (fromSq % 8);
  char fromRank = '1' + (fromSq / 8);
  char toFile = 'a' + (toSq % 8);
  char toRank = '1' + (toSq / 8);

  if (move.piece == PAWN) {
    if (move.capturedPiece != NONE) {
      res += fromFile;
      res += 'x';
    }
    res += toFile;
    res += toRank;
  } else {
    // Piece prefix
    char pieceChar = ' ';
    switch (move.piece) {
    case KNIGHT:
      pieceChar = 'N';
      break;
    case BISHOP:
      pieceChar = 'B';
      break;
    case ROOK:
      pieceChar = 'R';
      break;
    case QUEEN:
      pieceChar = 'Q';
      break;
    case KING:
      pieceChar = 'K';
      break;
    default:
      break;
    }
    if (pieceChar != ' ')
      res += pieceChar;

    if (move.capturedPiece != NONE)
      res += 'x';
    res += toFile;
    res += toRank;
  }

  // Check (+)?
  Board tempBoard = board;
  makeMove(tempBoard, move, isWhite);
  if (isKingInCheck(tempBoard, !isWhite)) {
    res += '+';
  }

  return res;
}
