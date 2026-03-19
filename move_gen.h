#ifndef MOVE_GEN_H
#define MOVE_GEN_H

#include "board.h"

uint64_t getKnightMoves(uint64_t POS, Board &board, bool isWhite);
uint64_t getRookMoves(uint64_t pos, Board &board, bool isWhite);
uint64_t getPawnMoves(uint64_t pos, Board &board, bool isWhite);
uint64_t getKingMoves(uint64_t pos, Board board, bool isWhite);
uint64_t getBishopMoves(uint64_t pos, Board &board, bool isWhite);
uint64_t getQueenMove(uint64_t pos, Board &board, bool isWhite);
std::vector<Move> generateAllMoves(Board &board, bool isWhite);
bool isKingInCheck(Board &board, bool isWhite);

#endif // MOVE_GEN_H
