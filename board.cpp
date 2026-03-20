#include "board.h"
#include "move_gen.h"
#include <sstream>

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

void makeMove(Board &board, Move &move, bool isWhite) {
  move.prevCastlingRights = board.castlingRights;
  move.prevEnPassantSq = board.enPassantSq;

  uint64_t from = 1ULL << (move.from - 1);
  uint64_t to = 1ULL << (move.to - 1);
  uint64_t fromTo = from | to;

  // Handle captures
  if (move.isEnPassant) {
    uint64_t capSq = isWhite ? (to >> 8) : (to << 8);
    if (isWhite)
      board.blackPawns ^= capSq;
    else
      board.whitePawns ^= capSq;
  } else if (move.capturedPiece != NONE) {
    if (isWhite) {
      if (to & board.blackPawns)
        board.blackPawns ^= to;
      else if (to & board.blackKnights)
        board.blackKnights ^= to;
      else if (to & board.blackBishops)
        board.blackBishops ^= to;
      else if (to & board.blackRooks)
        board.blackRooks ^= to;
      else if (to & board.blackQueen)
        board.blackQueen ^= to;
      else if (to & board.blackKing)
        board.blackKing ^= to;
    } else {
      if (to & board.whitePawns)
        board.whitePawns ^= to;
      else if (to & board.whiteKnights)
        board.whiteKnights ^= to;
      else if (to & board.whiteBishops)
        board.whiteBishops ^= to;
      else if (to & board.whiteRooks)
        board.whiteRooks ^= to;
      else if (to & board.whiteQueen)
        board.whiteQueen ^= to;
      else if (to & board.whiteKing)
        board.whiteKing ^= to;
    }
  }

  // Handle move
  PieceType p = move.piece;
  if (move.promotionPiece != NONE)
    p = move.promotionPiece;

  if (isWhite) {
    switch (move.piece) {
    case PAWN:
      board.whitePawns ^= from;
      if (move.promotionPiece == NONE)
        board.whitePawns |= to;
      break;
    case KNIGHT:
      board.whiteKnights ^= fromTo;
      break;
    case BISHOP:
      board.whiteBishops ^= fromTo;
      break;
    case ROOK:
      board.whiteRooks ^= fromTo;
      break;
    case QUEEN:
      board.whiteQueen ^= fromTo;
      break;
    case KING:
      board.whiteKing ^= fromTo;
      break;
    default:
      break;
    }
    if (move.promotionPiece != NONE) {
      switch (move.promotionPiece) {
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
      default:
        break;
      }
    }
  } else {
    switch (move.piece) {
    case PAWN:
      board.blackPawns ^= from;
      if (move.promotionPiece == NONE)
        board.blackPawns |= to;
      break;
    case KNIGHT:
      board.blackKnights ^= fromTo;
      break;
    case BISHOP:
      board.blackBishops ^= fromTo;
      break;
    case ROOK:
      board.blackRooks ^= fromTo;
      break;
    case QUEEN:
      board.blackQueen ^= fromTo;
      break;
    case KING:
      board.blackKing ^= fromTo;
      break;
    default:
      break;
    }
    if (move.promotionPiece != NONE) {
      switch (move.promotionPiece) {
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
      default:
        break;
      }
    }
  }

  // Handle castling
  // move.to == 3 means c1, move.to == 7 means g1, move.to == 59 means c8,
  // move.to == 63 means g8 this means that we are moving the king to c1, g1,
  // c8, or g8 and the rook is moving to the square next to the king
  if (move.isCastling) {
    if (isWhite) {
      if (move.to == 7) {
        board.whiteRooks ^= (1ULL << 7) | (1ULL << 5);
      } // g1
      else if (move.to == 3) {
        board.whiteRooks ^= (1ULL << 0) | (1ULL << 3);
      } // c1
    } else {
      if (move.to == 63) {
        board.blackRooks ^= (1ULL << 63) | (1ULL << 61);
      } // g8
      else if (move.to == 59) {
        board.blackRooks ^= (1ULL << 56) | (1ULL << 59);
      } // c8
    }
  }

  // Update enPassantSq
  board.enPassantSq = -1;
  if (move.piece == PAWN) {
    if (abs(move.from - move.to) == 16) {
      board.enPassantSq = isWhite ? (move.from + 7) : (move.from - 9);
    }
  }

  // Update castlingRights

  if (move.piece == KING) {
    if (isWhite)
      board.castlingRights &= ~3;
    else
      board.castlingRights &= ~12;
  }
  if (move.piece == ROOK) {
    if (isWhite) {
      if (move.from == 1)
        board.castlingRights &= ~2;
      else if (move.from == 8)
        board.castlingRights &= ~1;
    } else {
      if (move.from == 57)
        board.castlingRights &= ~8;
      else if (move.from == 64)
        board.castlingRights &= ~4;
    }
  }
  // Rook being captured
  if (move.capturedPiece == ROOK) {
    if (move.to == 8)
      board.castlingRights &= ~1;
    else if (move.to == 1)
      board.castlingRights &= ~2;
    else if (move.to == 64)
      board.castlingRights &= ~4;
    else if (move.to == 57)
      board.castlingRights &= ~8;
  }

  updateBoard(board);
}

void unMakeMove(Board &board, Move &move, bool isWhite) {
  uint64_t from = 1ULL << (move.from - 1);
  uint64_t to = 1ULL << (move.to - 1);
  uint64_t fromTo = from | to;

  // Move the pieces back
  if (isWhite) {
    if (move.promotionPiece != NONE) {
      board.whitePawns |= from;
      switch (move.promotionPiece) {
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
      default:
        break;
      }
    } else {
      switch (move.piece) {
      case PAWN:
        board.whitePawns ^= fromTo;
        break;
      case KNIGHT:
        board.whiteKnights ^= fromTo;
        break;
      case BISHOP:
        board.whiteBishops ^= fromTo;
        break;
      case ROOK:
        board.whiteRooks ^= fromTo;
        break;
      case QUEEN:
        board.whiteQueen ^= fromTo;
        break;
      case KING:
        board.whiteKing ^= fromTo;
        break;
      default:
        break;
      }
    }
  } else {
    if (move.promotionPiece != NONE) {
      board.blackPawns |= from;
      switch (move.promotionPiece) {
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
      default:
        break;
      }
    } else {
      switch (move.piece) {
      case PAWN:
        board.blackPawns ^= fromTo;
        break;
      case KNIGHT:
        board.blackKnights ^= fromTo;
        break;
      case BISHOP:
        board.blackBishops ^= fromTo;
        break;
      case ROOK:
        board.blackRooks ^= fromTo;
        break;
      case QUEEN:
        board.blackQueen ^= fromTo;
        break;
      case KING:
        board.blackKing ^= fromTo;
        break;
      default:
        break;
      }
    }
  }

  // Restore captured pieces
  if (move.isEnPassant) {
    uint64_t capSq = isWhite ? (to >> 8) : (to << 8);
    if (isWhite)
      board.blackPawns |= capSq;
    else
      board.whitePawns |= capSq;
  } else if (move.capturedPiece != NONE) {
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

  // Undo castling rook move
  if (move.isCastling) {
    if (isWhite) {
      if (move.to == 7) {
        board.whiteRooks ^= (1ULL << 7) | (1ULL << 5);
      } // g1
      else if (move.to == 3) {
        board.whiteRooks ^= (1ULL << 0) | (1ULL << 3);
      } // c1
    } else {
      if (move.to == 63) {
        board.blackRooks ^= (1ULL << 63) | (1ULL << 61);
      } // g8
      else if (move.to == 59) {
        board.blackRooks ^= (1ULL << 56) | (1ULL << 59);
      } // c8
    }
  }

  // Restore state
  board.castlingRights = move.prevCastlingRights;
  board.enPassantSq = move.prevEnPassantSq;

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

  if (move.promotionPiece != NONE) {
    switch (move.promotionPiece) {
    case KNIGHT:
      res += 'n';
      break;
    case BISHOP:
      res += 'b';
      break;
    case ROOK:
      res += 'r';
      break;
    case QUEEN:
      res += 'q';
      break;
    default:
      break;
    }
  }

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

void resetBoard(Board &board) {
  board.whitePawns = 0;
  board.whiteKnights = 0;
  board.whiteBishops = 0;
  board.whiteRooks = 0;
  board.whiteQueen = 0;
  board.whiteKing = 0;
  board.blackPawns = 0;
  board.blackKnights = 0;
  board.blackBishops = 0;
  board.blackRooks = 0;
  board.blackQueen = 0;
  board.blackKing = 0;
  board.whiteToMove = true;
  board.enPassantSq = -1;
  board.castlingRights = 0;
  updateBoard(board);
}

void loadFEN(Board &board, string fen) {
  resetBoard(board);
  stringstream ss(fen);
  string pieces, turn, castling, ep, halfmove, fullmove;
  ss >> pieces >> turn >> castling >> ep >> halfmove >> fullmove;

  int rank = 7, file = 0;
  for (char c : pieces) {
    if (c == '/') {
      rank--;
      file = 0;
    } else if (isdigit(c)) {
      file += c - '0';
    } else {
      int sq = rank * 8 + file;
      uint64_t bit = 1ULL << sq;
      switch (c) {
      case 'P':
        board.whitePawns |= bit;
        break;
      case 'N':
        board.whiteKnights |= bit;
        break;
      case 'B':
        board.whiteBishops |= bit;
        break;
      case 'R':
        board.whiteRooks |= bit;
        break;
      case 'Q':
        board.whiteQueen |= bit;
        break;
      case 'K':
        board.whiteKing |= bit;
        break;
      case 'p':
        board.blackPawns |= bit;
        break;
      case 'n':
        board.blackKnights |= bit;
        break;
      case 'b':
        board.blackBishops |= bit;
        break;
      case 'r':
        board.blackRooks |= bit;
        break;
      case 'q':
        board.blackQueen |= bit;
        break;
      case 'k':
        board.blackKing |= bit;
        break;
      }
      file++;
    }
  }
  board.whiteToMove = (turn == "w");

  // Castling rights
  board.castlingRights = 0;
  if (castling != "-") {
    for (char c : castling) {
      if (c == 'K')
        board.castlingRights |= 1;
      else if (c == 'Q')
        board.castlingRights |= 2;
      else if (c == 'k')
        board.castlingRights |= 4;
      else if (c == 'q')
        board.castlingRights |= 8;
    }
  }

  // En passant square
  if (ep != "-") {
    int f = ep[0] - 'a';
    int r = ep[1] - '1';
    board.enPassantSq = r * 8 + f;
  } else {
    board.enPassantSq = -1;
  }

  updateBoard(board);
}

Move parseMove(Board &board, string moveStr, bool isWhite) {
  vector<Move> legalMoves = generateAllMoves(board, isWhite);
  for (Move &m : legalMoves) {
    if (moveToUCI(m) == moveStr) {
      return m;
    }
  }
  return {0, 0, NONE, NONE};
}
