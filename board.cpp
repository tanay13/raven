#include <cstdint>
#include<cassert>
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

bool isOccupied(uint64_t sq, Board &board) {
  return sq & board.allPieces;
}

uint64_t getKnightMoves(uint64_t POS, Board board) {

  uint64_t abFile = board.aFile | board.bFile;
  uint64_t ghFile = board.gfile | board.hfile;

  uint64_t attacks = (POS << 17 & ~board.aFile) | (POS << 15 & ~board.hfile) |
                     (POS << 10 & ~abFile) | (POS << 6 & ~ghFile) |
                     (POS >> 17 & ~board.hfile) | (POS >> 15 & ~board.aFile) |
                     (POS >> 10 & ~ghFile) | (POS >> 6 & ~abFile);

  return attacks;
};

string toBinary(uint64_t x) { return bitset<64>(x).to_string(); }

void printBoard(string s) {
  for (int i = 0; i <= 63; i += 8) {
    for (int j = 7; j >= 0; j--) {
      cout << s[i + j] << " ";
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

uint64_t getRookMoves(uint64_t pos, Board board) {
  int rank = getRank(pos);
  int file = getFile(pos);

  uint64_t initRank = 0xFF;
  uint64_t initFile = 0x0101010101010101;

  uint64_t attack =
      (pos | (initRank << (rank - 1) * 8)) | (pos | (initFile << (file - 1)));
  return attack;
}

uint64_t getPawnMoves(uint64_t pos, Board board, bool isWhite) {
  if (isWhite) {
    return pos << 8;
  }
  return pos >> 8;
}

uint64_t getKingMoves(uint64_t pos, Board board) {

  uint64_t attack = (pos << 8) | (pos >> 8) | (pos << 1 & ~board.aFile) |
                    (pos >> 1 & ~board.hfile) | (pos << 9 & ~board.aFile) |
                    (pos >> 9 & ~board.hfile) | (pos << 7 & ~board.hfile) |
                    (pos >> 7 & ~board.aFile);

  return attack;
}

uint64_t getBishopMoves(uint64_t pos, Board board) {

  int rank = getRank(pos);
  int file = getFile(pos);

  uint64_t attack = pos;

  /*NE diagonal*/
  for (int i = 1; i <= min(8 - file, 8 - rank); i++) {
    attack |= (pos << 9 * i);
  }

  /*NW diagonal*/
  for (int i = 1; i <= min(file - 1, 8 - rank); i++) {
    attack |= (pos << 7 * i);
  }

  /*SE diagonal*/
  for (int i = 1; i <= min(8 - file, rank - 1); i++) {
    attack |= (pos >> 7 * i);
  }

  /*SW diagonal*/
  for (int i = 1; i <= min(file - 1, rank - 1); i++) {
    attack |= (pos >> 9 * i);
  }

  return attack;
}

uint64_t getQueenMove(uint64_t pos, Board board) {

  int rank = getRank(pos);
  int file = getFile(pos);

  uint64_t initRank = 0xFF;
  uint64_t initFile = 0x0101010101010101;

  uint64_t attack = pos;

  attack |=
      (pos | (initRank << (rank - 1) * 8)) | (pos | (initFile << (file - 1)));

  /*NE diagonal*/
  for (int i = 1; i <= min(8 - file, 8 - rank); i++) {
    attack |= (pos << 9 * i);
  }

  /*NW diagonal*/
  for (int i = 1; i <= min(file - 1, 8 - rank); i++) {
    attack |= (pos << 7 * i);
  }

  /*SE diagonal*/
  for (int i = 1; i <= min(8 - file, rank - 1); i++) {
    attack |= (pos >> 7 * i);
  }

  /*SW diagonal*/
  for (int i = 1; i <= min(file - 1, rank - 1); i++) {
    attack |= (pos >> 9 * i);
  }

  return attack;
}

int main() {

  /*Initialize the board*/
  Board board;

  // uint64_t p = board.whiteKnights; // ....0000 1000
  //
  // printBoard(toBinary(p));
  //
  // cout << endl;

  assert(board.whiteBoard == 0x000000000000FFFFULL);
  assert(board.blackBoard == 0xFFFF000000000000ULL);
  cout << endl;
  // uint64_t attacks = getKnightMoves(p, board);

  // uint64_t attacks = getRookMoves(p, board);

  // uint64_t attacks = getPawnMoves(p, board, false);

  // uint64_t attacks = getBishopMoves(p, board);

  // uint64_t attacks = getQueenMove(p, board);

  // uint64_t attacks  = getKingMoves(p, board);

  // string s = toBinary(attacks);

  // printBoard(s);

  cout << endl << endl;

  // cout<<getRank(p);

  return 0;
}
