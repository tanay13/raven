#include "engine.h"
#include "move_gen.h"

using namespace std;

int main() {
  /*Initialize the board*/
  Board board;

  cout << findBestMove(board, 7, true) << endl;

  return 0;
}
