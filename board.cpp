#include <cassert>
#include <cstdint>
#include <iostream>

using namespace std;

struct Board {
  uint64_t blackPawns = 0x000000000000FF00ULL;
  uint64_t blackKnights = 0x0000000000000042ULL;
  uint64_t blackBishops = 0x0000000000000024ULL;
  uint64_t blackRooks = 0x0000000000000081ULL;
  uint64_t blackQueen = 0x0000000000000008ULL;
  uint64_t blackKing = 0x0000000000000010ULL;

  uint64_t whitePawns = 0x00FF000000000000ULL;
  uint64_t whiteKnights = 0x4200000000000000ULL;
  uint64_t whiteBishops = 0x2400000000000000ULL;
  uint64_t whiteRooks = 0x8100000000000000ULL;
  uint64_t whiteQueen = 0x0800000000000000ULL;
  uint64_t whiteKing = 0x1000000000000000ULL;

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
  for (int i = 63; i >= 0; i -= 8) {
    for (int j = 0; j <= 7; j++) {
      cout << s[i - j] << " ";
    }
    cout << endl;
  }
}

int getRank(uint64_t pos) {

  uint64_t init = 0xFF; // 000....1111 1111

  int rank = 1;

  while ((init & pos) == 0) {
    init = init << 8;
    rank++;
  }
  return rank;
}

int getFile(uint64_t pos) {

  uint64_t init = 0x0101010101010101; // 0000 0001 0000 0001

  int file = 1;

  while ((init & pos) == 0) {
    init = init << 1;
    file++;
  }
  return file;
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
  uint64_t s = 00LL;
  uint64_t attack = 00LL;
  uint64_t diag = 00LL;
  if (isWhite) {
    s = pos << 8;
    if (!(s & board.allPieces))
      attack |= s;

    diag = ((pos << 7 & ~board.hfile) | (pos << 9 & ~board.aFile)) &
           board.blackBoard;
    attack |= diag;
  } else {
    s = pos >> 8;
    if (!(s & board.allPieces))
      attack |= s;

    diag = ((pos >> 7 & ~board.aFile) | (pos >> 9 & ~board.hfile)) &
           board.whiteBoard;
    attack |= diag;
  }
  return attack;
}

uint64_t getKingMoves(uint64_t pos, Board board) {

  uint64_t attack = (pos << 8) | (pos >> 8) | (pos << 1 & ~board.aFile) |
                    (pos >> 1 & ~board.hfile) | (pos << 9 & ~board.aFile) |
                    (pos >> 9 & ~board.hfile) | (pos << 7 & ~board.hfile) |
                    (pos >> 7 & ~board.aFile);

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

struct Move {
  int from;
  int to;
  string piece;
  string capturedPiece;
};

vector<int> moveList(uint64_t moves) {

  uint64_t init = 0x0100000000000000ULL;
  vector<int> moveList;
  for (int i = 1; i < 64; i++) {
    if (init & moves) {
      int sq = 63 - __builtin_ctzll(init);
      moveList.push_back(sq);
    }
    init = init >> 1;
  }

  return moveList;
}

int main() {

  /*Initialize the board*/
  Board board;
  board.whiteBoard = 0, board.blackBoard = 0, board.allPieces = 0;

  uint64_t p = 0x8000000000; // ....0000 1000

  printBoard(toBinary(p));
  cout << endl;

  uint64_t attacks = getKnightMoves(p, board, true);

  string s = toBinary(attacks);

  printBoard(s);

  cout << endl << endl;

  vector<int> moves = moveList(attacks);

  for (auto it : moves)
    cout << it << " ";

  // cout<<getRank(p);

  return 0;
}
