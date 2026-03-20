#include "engine.h"
#include "board.h"
#include "move_gen.h"
#include <climits>
#include <algorithm>
#include <chrono>

using namespace std;

bool stopSearch = false;
long long nodesSearched = 0;
auto startTime = chrono::high_resolution_clock::now();
int timeLimit = 1000; // default 1s

void checkTime() {
    if (nodesSearched % 2048 == 0) {
        auto currentTime = chrono::high_resolution_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(currentTime - startTime).count();
        if (elapsed > timeLimit) {
            stopSearch = true;
        }
    }
}

int minimax(Board &board, int depth, int alpha, int beta, bool isWhite) {
  nodesSearched++;
  checkTime();
  if (stopSearch) return 0;

  if (depth == 0)
    return evaluate(board);

  vector<Move> moves = generateAllMoves(board, isWhite);
  if (moves.empty()) {
    if (isKingInCheck(board, isWhite)) return isWhite ? -1000000 : 1000000;
    return 0; // Stalemate
  }

  if (isWhite) {
    for (auto &it : moves) {
      makeMove(board, it, true);
      int score = minimax(board, depth - 1, alpha, beta, false);
      unMakeMove(board, it, true);
      if (stopSearch) return 0;
      alpha = max(alpha, score);
      if (beta <= alpha)
        break;
    }
    return alpha;
  } else {
    for (auto &it : moves) {
      makeMove(board, it, false);
      int score = minimax(board, depth - 1, alpha, beta, true);
      unMakeMove(board, it, false);
      if (stopSearch) return 0;
      beta = min(beta, score);
      if (beta <= alpha)
        break;
    }
    return beta;
  }
}

string iterativeDeepening(Board &board, int timeLimitMs, bool isWhite) {
  startTime = chrono::high_resolution_clock::now();
  timeLimit = timeLimitMs;
  stopSearch = false;
  nodesSearched = 0;

  Move bestMoveOverall = {0, 0, NONE, NONE};
  int totalBestScore = 0;
  
  // Start from depth 1 and increase
  for (int depth = 1; depth <= 50; depth++) {
    Move bestMoveAtDepth = {0, 0, NONE, NONE};
    int currentBestScore = isWhite ? -2000000 : 2000000;
    
    vector<Move> moves = generateAllMoves(board, isWhite);
    if (moves.empty()) break;

    for (auto &it : moves) {
      makeMove(board, it, isWhite);
      int score = minimax(board, depth - 1, -2000000, 2000000, !isWhite);
      unMakeMove(board, it, isWhite);
      
      if (stopSearch) break;

      if (isWhite) {
        if (score > currentBestScore) {
          currentBestScore = score;
          bestMoveAtDepth = it;
        }
      } else {
        if (score < currentBestScore) {
          currentBestScore = score;
          bestMoveAtDepth = it;
        }
      }
    }

    if (!stopSearch) {
      bestMoveOverall = bestMoveAtDepth;
      totalBestScore = currentBestScore;
      
      auto currentTime = chrono::high_resolution_clock::now();
      auto elapsed = chrono::duration_cast<chrono::milliseconds>(currentTime - startTime).count();
      if (elapsed == 0) elapsed = 1; // avoid div by zero

      // Output info to GUI
      cout << "info depth " << depth 
           << " score cp " << totalBestScore 
           << " nodes " << nodesSearched 
           << " nps " << (nodesSearched * 1000 / elapsed)
           << " time " << elapsed 
           << " pv " << moveToUCI(bestMoveOverall) << endl;
    } else {
      break; 
    }
    
    // If we already spent more than 50% of our time, don't start a new depth
    auto currentTime = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(currentTime - startTime).count();
    if (elapsed > timeLimit / 2) break;
  }

  if (bestMoveOverall.from == 0) {
    vector<Move> moves = generateAllMoves(board, isWhite);
    if (!moves.empty()) return moveToUCI(moves[0]);
    return "none";
  }
  return moveToUCI(bestMoveOverall);
}

string findBestMove(Board &board, int depth, bool isWhite) {
  // Keeping this for compatibility but we will use iterativeDeepening in UCI
  stopSearch = false;
  timeLimit = 1000000; // effectively no limit
  nodesSearched = 0;
  
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
  return moveToUCI(bestMove);
}
