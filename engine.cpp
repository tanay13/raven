#include "engine.h"
#include "board.h"
#include "move_gen.h"
#include <climits>
#include <algorithm>

using namespace std;

int minimax(Board &board, int depth, int alpha, int beta, bool isWhite) {
  if (depth == 0)
    return evaluate(board);

  vector<Move> moves;
  if (isWhite) {
    moves = generateAllMoves(board, true);
    for (auto &it : moves) {
      makeMove(board, it, true);
      int score = minimax(board, depth - 1, alpha, beta, false);
      unMakeMove(board, it, true);
      alpha = max(alpha, score);
      if (beta <= alpha)
        break;
    }
    return alpha;
  } else {
    moves = generateAllMoves(board, false);
    for (auto &it : moves) {
      makeMove(board, it, false);
      int score = minimax(board, depth - 1, alpha, beta, true);
      unMakeMove(board, it, false);
      beta = min(beta, score);
      if (beta <= alpha)
        break;
    }
    return beta;
  }
}

string findBestMove(Board &board, int depth, bool isWhite) {
  Move bestMove = {0, 0, NONE, NONE};
  vector<Move> moves = generateAllMoves(board, isWhite);

  if (isWhite) {
    int bestScore = INT_MIN;
    for (auto &it : moves) {
      makeMove(board, it, true);
      int score = minimax(board, depth - 1, INT_MIN, INT_MAX, false);
      unMakeMove(board, it, true);
      if (score > bestScore) {
        bestScore = score;
        bestMove = it;
      }
    }
  } else {
    int bestScore = INT_MAX;
    for (auto &it : moves) {
      makeMove(board, it, false);
      int score = minimax(board, depth - 1, INT_MIN, INT_MAX, true);
      unMakeMove(board, it, false);
      if (score < bestScore) {
        bestScore = score;
        bestMove = it;
      }
    }
  }

  if (bestMove.from == 0) return "none";
  return moveToSAN(board, bestMove, isWhite);
}
