#include <cassert>
#include <cstdint>
#include <iostream>

using namespace std;

struct Board {
  uint64_t whitePawns = 0x000000000000FF00ULL;
  uint64_t whiteKnights = 0x0000000000000042ULL;
  uint64_t whiteBishops = 0x0000000000000024ULL;
  uint64_t whiteRooks = 0x0000000000000081ULL;
  uint64_t whiteQueen = 0x0000000000000008ULL;
  uint64_t whiteKing = 0x0000000000000010ULL;

  uint64_t blackPawns = 0x00FF000000000000ULL;
  uint64_t blackKnights = 0x4200000000000000ULL;
  uint64_t blackBishops = 0x2400000000000000ULL;
  uint64_t blackRooks = 0x8100000000000000ULL;
  uint64_t blackQueen = 0x0800000000000000ULL;
  uint64_t blackKing = 0x1000000000000000ULL;

  uint64_t aFile = 0x0101010101010101; // ....0000 0001 0000 0001
  uint64_t bFile = 0x0202020202020202;

  uint64_t hfile = 0x8080808080808080; // ......1000 0000 1000 0000 1000 0000
  uint64_t gfile = 0x4040404040404040; // 0100 0000 0100 0000

  /*Occupancy..*/
  uint64_t whiteBoard = whitePawns | whiteKnights | whiteBishops | whiteKing |
                        whiteQueen | whiteRooks;
  uint64_t blackBoard = blackPawns | blackKnights | blackBishops | blackQueen |
                        blackRooks | blackKing;

  uint64_t allPieces = whiteBoard | blackBoard;
};

void boardUpdate(Board &board) {

  board.whiteBoard = board.whitePawns | board.whiteKnights |
                     board.whiteBishops | board.whiteQueen | board.whiteRooks |
                     board.whiteKing;
  board.blackBoard = board.blackPawns | board.blackKnights |
                     board.blackBishops | board.blackRooks | board.blackQueen |
                     board.blackKing;

  board.allPieces = board.whiteBoard | board.blackBoard;
}

bool isOccupied(uint64_t sq, Board &board) { return sq & board.allPieces; }

uint64_t getKnightMoves(uint64_t POS, Board &board, bool isWhite) {

  uint64_t abFile = board.aFile | board.bFile;
  uint64_t ghFile = board.gfile | board.hfile;

  uint64_t ownPieces = isWhite ? board.whiteBoard : board.blackBoard;
  uint64_t attacks = 00LL;

  attacks = (POS << 17 & ~board.aFile) | (POS << 15 & ~board.hfile) |
            (POS << 10 & ~abFile) | (POS << 6 & ~ghFile) |
            (POS >> 17 & ~board.hfile) | (POS >> 15 & ~board.aFile) |
            (POS >> 10 & ~ghFile) | (POS >> 6 & ~abFile);

  attacks &= ~ownPieces;

  return attacks;
};

string toBinary(uint64_t x) { return bitset<64>(x).to_string(); }

void printBoard(string s) {
  for (int i = 0; i < 64; i += 8) {
    for (int j = 7; j >= 0; j--) {
      cout << s[i + j] << " ";
    }
    cout << endl;
  }
}

uint64_t getRookMoves(uint64_t pos, Board &board, bool isWhite) {

  int sq = __builtin_ctzll(pos);

  int rank = sq / 8;
  int file = sq % 8;
  uint64_t attack = 00LL;
  uint64_t sameColorPieces = isWhite ? board.whiteBoard : board.blackBoard;

  /*North Direction*/
  for (int rr = rank + 1; rr < 8; rr++) {
    uint64_t s = 1LL << (rr * 8 + file);
    attack |= s;
    if (isOccupied(s, board)) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }

  /*South Direction*/
  for (int rr = rank - 1; rr >= 0; rr--) {
    uint64_t s = 1LL << (rr * 8 + file);
    attack |= s;
    if (isOccupied(s, board)) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }

  /*West Direction*/
  for (int ff = file - 1; ff >= 0; ff--) {
    uint64_t s = 1LL << (rank * 8 + ff);
    attack |= s;
    if (isOccupied(s, board)) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }

  /*East Direction*/
  for (int ff = file + 1; ff < 8; ff++) {
    uint64_t s = 1LL << (rank * 8 + ff);
    attack |= s;
    if (isOccupied(s, board)) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }

  return attack;
}

uint64_t getPawnMoves(uint64_t pos, Board &board, bool isWhite) {
  uint64_t moves = 00LL;
  uint64_t singlePush, doublePush;

  if (isWhite) {
    // Single Push
    singlePush = pos << 8;
    if (!(singlePush & board.allPieces)) {
      moves |= singlePush;

      // Double Push (Only from Rank 2)
      // 0x000000000000FF00 is Rank 2
      if ((pos & 0x000000000000FF00ULL)) {
        doublePush = singlePush << 8;
        if (!(doublePush & board.allPieces))
          moves |= doublePush;
      }
    }
    // Capture moves
    moves |= ((pos << 7 & ~board.hfile) | (pos << 9 & ~board.aFile)) &
             board.blackBoard;
  } else {
    singlePush = pos >> 8;
    if (!(singlePush & board.allPieces)) {
      moves |= singlePush;

      // Double Push (Only from Rank 7)
      // 0x00FF000000000000 is Rank 7
      if ((pos & 0x00FF000000000000ULL)) {
        doublePush = singlePush >> 8;
        if (!(doublePush & board.allPieces))
          moves |= doublePush;
      }
    }
    // Capture move
    moves |= ((pos >> 7 & ~board.aFile) | (pos >> 9 & ~board.hfile)) &
             board.whiteBoard;
  }
  return moves;
}

uint64_t getKingMoves(uint64_t pos, Board board, bool isWhite) {

  uint64_t ownPieces = isWhite ? board.whiteBoard : board.blackBoard;

  uint64_t attack = (pos << 8) | (pos >> 8) | (pos << 1 & ~board.aFile) |
                    (pos >> 1 & ~board.hfile) | (pos << 9 & ~board.aFile) |
                    (pos >> 9 & ~board.hfile) | (pos << 7 & ~board.hfile) |
                    (pos >> 7 & ~board.aFile);

  attack &= ~ownPieces;

  return attack;
}

uint64_t getBishopMoves(uint64_t pos, Board &board, bool isWhite) {

  int sq = __builtin_ctzll(pos);

  int rank = sq / 8;
  int file = sq % 8;

  uint64_t attack = 00LL;

  uint64_t sameColorPieces = isWhite ? board.whiteBoard : board.blackBoard;

  /*NE diagonal*/
  for (int i = 1; i <= min(7 - file, 7 - rank); i++) {
    uint64_t s = (pos << 9 * i);
    attack |= s;
    if (isOccupied(s, board)) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }

  /*NW diagonal*/
  for (int i = 1; i <= min(file, 7 - rank); i++) {
    uint64_t s = (pos << 7 * i);
    attack |= s;
    if (isOccupied(s, board)) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }

  /*SE diagonal*/
  for (int i = 1; i <= min(7 - file, rank); i++) {
    uint64_t s = (pos >> 7 * i);
    attack |= s;
    if (isOccupied(s, board)) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }

  /*SW diagonal*/
  for (int i = 1; i <= min(file, rank); i++) {
    uint64_t s = (pos >> 9 * i);
    attack |= s;
    if (isOccupied(s, board)) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }

  return attack;
}

uint64_t getQueenMove(uint64_t pos, Board &board, bool isWhite) {

  uint64_t attack = 00LL;

  uint64_t sameColorPieces = isWhite ? board.whiteBoard : board.blackBoard;

  attack |= getRookMoves(pos, board, isWhite);
  attack |= getBishopMoves(pos, board, isWhite);

  return attack;
}

enum PieceType { NONE = 0, PAWN = 1, KNIGHT, BISHOP, ROOK, QUEEN, KING };

struct Move {
  int from;
  int to;
  PieceType piece;
  PieceType capturedPiece;
};

vector<Move> generateAllMoves(Board &board, bool isWhite) {
  vector<Move> moves;

  uint64_t ownBoard = isWhite ? board.whiteBoard : board.blackBoard;
  uint64_t oppBoard = isWhite ? board.blackBoard : board.whiteBoard;

  uint64_t pawns = isWhite ? board.whitePawns : board.blackPawns;
  uint64_t knights = isWhite ? board.whiteKnights : board.blackKnights;
  uint64_t bishops = isWhite ? board.whiteBishops : board.blackBishops;
  uint64_t rooks = isWhite ? board.whiteRooks : board.blackRooks;
  uint64_t queen = isWhite ? board.whiteQueen : board.blackQueen;
  uint64_t king = isWhite ? board.whiteKing : board.blackKing;

  if (pawns) {
    uint64_t init = 0x8000000000000000ULL;
    for (int i = 0; i < 64; i++) {
      if (init & pawns) {
        uint64_t attacks = getPawnMoves(init, board, isWhite);
        Move m;
        m.from = __builtin_ctzll(init) + 1;
        m.piece = PAWN;
        m.capturedPiece = NONE;
        while (attacks) {
          uint64_t to = attacks & -attacks; // gets the least significant bit
          m.to = __builtin_ctzll(to) + 1;
          moves.push_back(m);
          attacks ^= to;
        }
      }
      init = init >> 1;
    }
  }

  if (bishops) {
    uint64_t init = 0x8000000000000000ULL;
    for (int i = 0; i < 64; i++) {
      if (init & bishops) {
        uint64_t attacks = getBishopMoves(init, board, isWhite);
        Move m;
        m.from = __builtin_ctzll(init) + 1;
        m.piece = BISHOP;
        m.capturedPiece = NONE;
        while (attacks) {
          uint64_t to = attacks & -attacks; // gets the least significant bit
          m.to = __builtin_ctzll(to) + 1;
          moves.push_back(m);
          attacks ^= to;
        }
      }
      init = init >> 1;
    }
  }

  if (knights) {
    uint64_t init = 0x8000000000000000ULL;
    for (int i = 0; i < 64; i++) {
      if (init & knights) {
        uint64_t attacks = getKnightMoves(init, board, isWhite);
        Move m;
        m.from = __builtin_ctzll(init) + 1;
        m.piece = KNIGHT;
        m.capturedPiece = NONE;
        while (attacks) {
          uint64_t to = attacks & -attacks; // gets the least significant bit
          m.to = __builtin_ctzll(to) + 1;
          moves.push_back(m);
          attacks ^= to;
        }
      }
      init = init >> 1;
    }
  }

  if (rooks) {
    uint64_t init = 0x8000000000000000ULL;
    for (int i = 0; i < 64; i++) {
      if (init & rooks) {
        uint64_t attacks = getRookMoves(init, board, isWhite);
        Move m;
        m.from = __builtin_ctzll(init) + 1;
        m.piece = ROOK;
        m.capturedPiece = NONE;
        while (attacks) {
          uint64_t to = attacks & -attacks; // gets the least significant bit
          m.to = __builtin_ctzll(to) + 1;
          moves.push_back(m);
          attacks ^= to;
        }
      }
      init = init >> 1;
    }
  }

  if (queen) {
    uint64_t init = 0x8000000000000000ULL;
    for (int i = 0; i < 64; i++) {
      if (init & queen) {
        uint64_t attacks = getQueenMove(init, board, isWhite);
        Move m;
        m.from = __builtin_ctzll(init) + 1;
        m.piece = QUEEN;
        m.capturedPiece = NONE;
        while (attacks) {
          uint64_t to = attacks & -attacks; // gets the least significant bit
          m.to = __builtin_ctzll(to) + 1;
          moves.push_back(m);
          attacks ^= to;
        }
      }
      init = init >> 1;
    }
  }

  if (king) {
    uint64_t init = 0x8000000000000000ULL;
    for (int i = 0; i < 64; i++) {
      if (init & king) {
        uint64_t attacks = getKingMoves(init, board, isWhite);
        Move m;
        m.from = __builtin_ctzll(init) + 1;
        m.piece = KING;
        m.capturedPiece = NONE;
        while (attacks) {
          uint64_t to = attacks & -attacks; // gets the least significant bit
          m.to = __builtin_ctzll(to) + 1;
          moves.push_back(m);
          attacks ^= to;
        }
      }
      init = init >> 1;
    }
  }

  return moves;
}

void updateBoard(Board &board) {
  board.whiteBoard = board.whitePawns | board.whiteKnights |
                     board.whiteBishops | board.whiteKing | board.whiteQueen |
                     board.whiteRooks;
  board.blackBoard = board.blackPawns | board.blackKnights |
                     board.blackBishops | board.blackQueen | board.blackRooks |
                     board.blackKing;

  board.allPieces = board.whiteBoard | board.blackBoard;
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

int main() {

  /*Initialize the board*/
  Board board;
  // uint64_t p = 0x0400;
  // printBoard(toBinary(p));
  // cout << endl << endl;
  // uint64_t attacks = getPawnMoves(p, board, true);
  // printBoard(toBinary(attacks));
  printBoard(toBinary(board.whiteBoard));
  vector<Move> moves = generateAllMoves(board, true);
  makeMove(board, moves[0], true);
  cout << endl;
  printBoard(toBinary(board.whiteBoard));
  // for (auto it : moves) {
  //   cout << it.from << " " << it.to << " " << it.piece << " "
  //        << it.capturedPiece << endl;
  // }

  return 0;
}
