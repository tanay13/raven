#ifndef ENGINE_H
#define ENGINE_H

#include "move_gen.h"

int minimax(Board &board, int depth, int alpha, int beta, bool isWhite);
void clearTT();
std::string findBestMove(Board &board, int depth, bool isWhite);
std::string iterativeDeepening(Board &board, int timeLimitMs, bool isWhite);

#endif
