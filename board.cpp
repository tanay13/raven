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
};

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

int main() {

  /*Initialize the board*/
  Board board;

  uint64_t p = 0x08; // ....0000 1000


  printBoard(toBinary(p));

  cout<<endl;

  uint64_t attacks = getKnightMoves(p, board);

  string s = toBinary(attacks);

  printBoard(s);

  return 0;
}
