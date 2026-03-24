#ifndef BOARD_H
#define BOARD_H

#include <bitset>
#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <vector>

enum PieceType { NONE = 0, PAWN = 1, KNIGHT, BISHOP, ROOK, QUEEN, KING };

extern std::map<PieceType, int> PiecePoint;

struct Move {
  int from;
  int to;
  PieceType piece;
  PieceType capturedPiece;
  PieceType promotionPiece = NONE;
  bool isCastling = false;
  bool isEnPassant = false;
  int prevEnPassantSq = -1;
  uint8_t prevCastlingRights = 0;
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
  bool whiteToMove = true;
  int enPassantSq = -1; // 0-63, -1 for none
  uint8_t castlingRights =
      0xF; // Bitmask: 1:K, 2:Q, 4:k, 8:q. castlingRights is defined using 4
           // bits where each bit represents a castling right.
  // 1: white king side, 2: white queen side, 4: black king side, 8: black queen
  // side
  uint64_t hash = 0;
};

// Zobrist Hashing
extern uint64_t pieceKeys[12][64];
extern uint64_t sideKey;
extern uint64_t castlingKeys[16];
extern uint64_t enPassantKeys[8];

void initZobrist();
uint64_t computeHash(const Board &board);

void boardUpdate(Board &board);
void updateBoard(Board &board);
bool isOccupied(uint64_t sq, Board &board);
int evaluate(Board &board);
std::string toBinary(uint64_t x);
void printBoard(std::string s);
void makeMove(Board &board, Move &move, bool isWhite);
void unMakeMove(Board &board, Move &move, bool isWhite);
std::string moveToUCI(Move move);
std::string moveToSAN(Board &board, Move move, bool isWhite);

void loadFEN(Board &board, std::string fen);
void resetBoard(Board &board);
Move parseMove(Board &board, std::string moveStr, bool isWhite);

#endif // BOARD_H
