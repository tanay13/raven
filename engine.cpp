#include "engine.h"
#include "board.h"
#include "move_gen.h"
#include <algorithm>
#include <chrono>
#include <climits>

using namespace std;

bool stopSearch = false;
long long nodesSearched = 0;
auto startTime = chrono::high_resolution_clock::now();
int timeLimit = 1000; // default 1s

enum NodeType { EXACT, ALPHA, BETA };

struct TTEntry {
  uint64_t hash;
  int depth;
  int score;
  NodeType type;
};

const int TT_SIZE = 1 << 20; // ~1M entries
TTEntry tt[TT_SIZE];

void clearTT() {
  for (int i = 0; i < TT_SIZE; i++) {
    tt[i].hash = 0;
  }
}

void checkTime() {
  /*
  check every 2048 nodes if we have exceeded the time limit
  checking every 2048 nodes is a heuristic to reduce the overhead of checking
  the time limit
  */
  if (nodesSearched % 2048 == 0) {
    auto currentTime = chrono::high_resolution_clock::now();
    auto elapsed =
        chrono::duration_cast<chrono::milliseconds>(currentTime - startTime)
            .count();
    if (elapsed > timeLimit) {
      stopSearch = true;
    }
  }
}

int minimax(Board &board, int depth, int alpha, int beta, bool isWhite) {
  nodesSearched++;
  checkTime();
  if (stopSearch)
    return 0;

  // TT Lookup
  uint64_t hash = board.hash;
  TTEntry &entry = tt[hash % TT_SIZE];
  if (entry.hash == hash && entry.depth >= depth) {
    if (entry.type == EXACT)
      return entry.score;
    if (entry.type == ALPHA && entry.score <= alpha)
      return alpha;
    if (entry.type == BETA && entry.score >= beta)
      return beta;
  }

  if (depth == 0)
    return evaluate(board);

  vector<Move> moves = generateAllMoves(board, isWhite);
  if (moves.empty()) {
    if (isKingInCheck(board, isWhite))
      return isWhite ? -1000000 : 1000000;
    return 0; // Stalemate
  }

  int originalAlpha = alpha;
  int bestScore = isWhite ? -2000000 : 2000000;

  if (isWhite) {
    for (auto &it : moves) {
      makeMove(board, it, true);
      int score = minimax(board, depth - 1, alpha, beta, false);
      unMakeMove(board, it, true);
      if (stopSearch)
        return 0;
      alpha = max(alpha, score);
      if (beta <= alpha)
        break;
    }
    bestScore = alpha;
  } else {
    for (auto &it : moves) {
      makeMove(board, it, false);
      int score = minimax(board, depth - 1, alpha, beta, true);
      unMakeMove(board, it, false);
      if (stopSearch)
        return 0;
      beta = min(beta, score);
      if (beta <= alpha)
        break;
    }
    bestScore = beta;
  }

  // Store in Transposition Table
  // We store type because if we have gotten a score with pruning then we dont
  // exactly know the exact score, it could be anything between alpha and beta,
  // so we store the type to indicate that the score is not exact.
  entry.hash = hash;
  entry.score = bestScore;
  entry.depth = depth;
  if (bestScore <= originalAlpha)
    entry.type = ALPHA;
  else if (bestScore >= beta)
    entry.type = BETA;
  else
    entry.type = EXACT;

  return bestScore;
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
    if (moves.empty())
      break;

    for (auto &it : moves) {
      makeMove(board, it, isWhite);
      int score = minimax(board, depth - 1, -2000000, 2000000, !isWhite);
      unMakeMove(board, it, isWhite);

      if (stopSearch)
        break;

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
      auto elapsed =
          chrono::duration_cast<chrono::milliseconds>(currentTime - startTime)
              .count();
      if (elapsed == 0)
        elapsed = 1; // avoid div by zero

      // Output info to GUI
      cout << "info depth " << depth << " score cp " << totalBestScore
           << " nodes " << nodesSearched << " nps "
           << (nodesSearched * 1000 / elapsed) << " time " << elapsed << " pv "
           << moveToUCI(bestMoveOverall) << endl;
    } else {
      break;
    }

    // If we already spent more than 50% of our time, don't start a new depth
    auto currentTime = chrono::high_resolution_clock::now();
    auto elapsed =
        chrono::duration_cast<chrono::milliseconds>(currentTime - startTime)
            .count();
    if (elapsed > timeLimit / 2)
      break;
  }
  // if bestMoveOverall.from == 0, it means we didn't find a move
  if (bestMoveOverall.from == 0) {
    vector<Move> moves = generateAllMoves(board, isWhite);
    if (!moves.empty())
      return moveToUCI(moves[0]);
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

  if (bestMove.from == 0)
    return "none";
  return moveToUCI(bestMove);
}
