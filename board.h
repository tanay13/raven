#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <string>
#include <vector>
#include <bitset>
#include <iostream>

using namespace std;

enum PieceType { NONE = 0, PAWN = 1, KNIGHT, BISHOP, ROOK, QUEEN, KING };

struct Move {
  int from;
  int to;
  PieceType piece;
  PieceType capturedPiece;
};

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

  uint64_t aFile = 0x0101010101010101; 
  uint64_t bFile = 0x0202020202020202;

  uint64_t hfile = 0x8080808080808080;
  uint64_t gfile = 0x4040404040404040;

  uint64_t whiteBoard = whitePawns | whiteKnights | whiteBishops | whiteKing |
                        whiteQueen | whiteRooks;
  uint64_t blackBoard = blackPawns | blackKnights | blackBishops | blackQueen |
                        blackRooks | blackKing;

  uint64_t allPieces = whiteBoard | blackBoard;
};

void boardUpdate(Board &board);
void updateBoard(Board &board);
bool isOccupied(uint64_t sq, Board &board);
string toBinary(uint64_t x);
void printBoard(string s);
void makeMove(Board &board, Move &move, bool isWhite);

#endif // BOARD_H
