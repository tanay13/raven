#include "uci.h"
#include "board.h"
#include "engine.h"
#include "move_gen.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

void uciLoop() {
  Board board;
  string line, cmd;

  while (getline(cin, line)) {
    stringstream ss(line);
    ss >> cmd;

    if (cmd == "uci") {
      cout << "id name Raven" << endl;
      cout << "id author Tanay" << endl;
      cout << "uciok" << endl;
    } else if (cmd == "isready") {
      cout << "readyok" << endl;
    } else if (cmd == "ucinewgame") {
      loadFEN(board,
              "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    } else if (cmd == "position") {
      string type;
      ss >> type;
      if (type == "startpos") {
        loadFEN(board,
                "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
      } else if (type == "fen") {
        string fen;
        string word;
        for (int i = 0; i < 6; i++) {
          if (ss >> word) {
            fen += word + " ";
          }
        }
        loadFEN(board, fen);
      }

      string movesCmd;
      if (ss >> movesCmd && movesCmd == "moves") {
        string moveStr;
        while (ss >> moveStr) {
          Move m = parseMove(board, moveStr, board.whiteToMove);
          if (m.from != 0) {
            makeMove(board, m, board.whiteToMove);
            board.whiteToMove = !board.whiteToMove;
          }
        }
      }
    } else if (cmd == "go") {
      string sub;
      int depth = 50;
      int wtime = -1, btime = -1, winc = 0, binc = 0, movestogo = 40;
      bool useTime = false;
      bool depthSpecified = false;

      while (ss >> sub) {
        if (sub == "depth") {
          ss >> depth;
          depthSpecified = true;
        } else if (sub == "wtime") {
          ss >> wtime;
          useTime = true;
        } else if (sub == "btime") {
          ss >> btime;
          useTime = true;
        } else if (sub == "winc") {
          ss >> winc;
        } else if (sub == "binc") {
          ss >> binc;
        } else if (sub == "movestogo") {
          ss >> movestogo;
        }
      }
      // wtime - how much time i have left
      // winc - how much time i get per move
      // movestogo - how many moves left
      string bestMove;
      if (useTime && !depthSpecified) {
        int myTime = board.whiteToMove ? wtime : btime;
        int myInc = board.whiteToMove ? winc : binc;

        // timeLimitMs = (time left / moves left) + (increment / 2)
        // this is a rough estimate of how much time we should spend on each
        // move if timeLimitMs is less than 0ms, we will spend 50ms on each move

        int timeLimitMs = (myTime / movestogo) + (myInc / 2);
        if (timeLimitMs <= 0)
          timeLimitMs = 50;

        bestMove = iterativeDeepening(board, timeLimitMs, board.whiteToMove);
      } else {
        bestMove =
            findBestMove(board, depthSpecified ? depth : 6, board.whiteToMove);
      }
      cout << "bestmove " << bestMove << endl;
    } else if (cmd == "quit") {
      break;
    }
  }
}
