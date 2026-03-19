#include "move_gen.h"
#include <algorithm>

uint64_t getKnightMoves(uint64_t POS, Board &board, bool isWhite) {
  uint64_t abFile = board.aFile | board.bFile;
  uint64_t ghFile = board.gfile | board.hfile;

  uint64_t ownPieces = isWhite ? board.whiteBoard : board.blackBoard;
  uint64_t attacks = 0;

  attacks = (POS << 17 & ~board.aFile) | (POS << 15 & ~board.hfile) |
            (POS << 10 & ~abFile) | (POS << 6 & ~ghFile) |
            (POS >> 17 & ~board.hfile) | (POS >> 15 & ~board.aFile) |
            (POS >> 10 & ~ghFile) | (POS >> 6 & ~abFile);

  attacks &= ~ownPieces;
  return attacks;
}

uint64_t getRookMoves(uint64_t pos, Board &board, bool isWhite) {
  int sq = __builtin_ctzll(pos);
  int rank = sq / 8;
  int file = sq % 8;
  uint64_t attack = 0;
  uint64_t sameColorPieces = isWhite ? board.whiteBoard : board.blackBoard;

  for (int rr = rank + 1; rr < 8; rr++) {
    uint64_t s = 1ULL << (rr * 8 + file);
    attack |= s;
    if (isOccupied(s, board)) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }
  for (int rr = rank - 1; rr >= 0; rr--) {
    uint64_t s = 1ULL << (rr * 8 + file);
    attack |= s;
    if (isOccupied(s, board)) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }
  for (int ff = file - 1; ff >= 0; ff--) {
    uint64_t s = 1ULL << (rank * 8 + ff);
    attack |= s;
    if (isOccupied(s, board)) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }
  for (int ff = file + 1; ff < 8; ff++) {
    uint64_t s = 1ULL << (rank * 8 + ff);
    attack |= s;
    if (isOccupied(s, board)) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }
  return attack;
}

uint64_t getPawnMoves(uint64_t pos, Board &board, bool isWhite) {
  uint64_t moves = 0;
  uint64_t singlePush, doublePush;

  if (isWhite) {
    singlePush = pos << 8;
    if (!(singlePush & board.allPieces)) {
      moves |= singlePush;
      if (pos & 0x000000000000FF00ULL) {
        doublePush = singlePush << 8;
        if (!(doublePush & board.allPieces))
          moves |= doublePush;
      }
    }
    moves |= ((pos << 7 & ~board.hfile) | (pos << 9 & ~board.aFile)) &
             board.blackBoard;
  } else {
    singlePush = pos >> 8;
    if (!(singlePush & board.allPieces)) {
      moves |= singlePush;
      if (pos & 0x00FF000000000000ULL) {
        doublePush = singlePush >> 8;
        if (!(doublePush & board.allPieces))
          moves |= doublePush;
      }
    }
    moves |= ((pos >> 7 & ~board.aFile) | (pos >> 9 & ~board.hfile)) &
             board.whiteBoard;
  }
  return moves;
}

uint64_t getKingMoves(uint64_t pos, Board board, bool isWhite) {
  uint64_t ownPieces = isWhite ? board.whiteBoard : board.blackBoard;
  uint64_t attack = (pos << 8) | (pos >> 8) | (pos << 1 & ~board.aFile) |
                    (pos >> 1 & ~board.hfile) | (pos << 9 & ~board.aFile) |
                    (pos >> 9 & ~board.hfile) | (pos << 7 & ~board.hfile) |
                    (pos >> 7 & ~board.aFile);
  attack &= ~ownPieces;
  return attack;
}

uint64_t getBishopMoves(uint64_t pos, Board &board, bool isWhite) {
  int sq = __builtin_ctzll(pos);
  int rank = sq / 8;
  int file = sq % 8;
  uint64_t attack = 0;
  uint64_t sameColorPieces = isWhite ? board.whiteBoard : board.blackBoard;

  for (int i = 1; i <= std::min(7 - file, 7 - rank); i++) {
    uint64_t s = (pos << (9 * i));
    attack |= s;
    if (isOccupied(s, board)) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }
  for (int i = 1; i <= std::min(file, 7 - rank); i++) {
    uint64_t s = (pos << (7 * i));
    attack |= s;
    if (isOccupied(s, board)) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }
  for (int i = 1; i <= std::min(7 - file, rank); i++) {
    uint64_t s = (pos >> (7 * i));
    attack |= s;
    if (isOccupied(s, board)) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }
  for (int i = 1; i <= std::min(file, rank); i++) {
    uint64_t s = (pos >> (9 * i));
    attack |= s;
    if (isOccupied(s, board)) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }
  return attack;
}

uint64_t getQueenMove(uint64_t pos, Board &board, bool isWhite) {
  return getRookMoves(pos, board, isWhite) |
         getBishopMoves(pos, board, isWhite);
}

vector<Move> generateAllMoves(Board &board, bool isWhite) {
  vector<Move> moves;
  uint64_t pawns = isWhite ? board.whitePawns : board.blackPawns;
  uint64_t knights = isWhite ? board.whiteKnights : board.blackKnights;
  uint64_t bishops = isWhite ? board.whiteBishops : board.blackBishops;
  uint64_t rooks = isWhite ? board.whiteRooks : board.blackRooks;
  uint64_t queen = isWhite ? board.whiteQueen : board.blackQueen;
  uint64_t king = isWhite ? board.whiteKing : board.blackKing;

  auto processPieces = [&](uint64_t pieces, PieceType type, auto getMovesFunc) {
    while (pieces) {
      uint64_t pos = pieces & -pieces;
      uint64_t attacks = getMovesFunc(pos, board, isWhite);
      while (attacks) {
        uint64_t to = attacks & -attacks;
        PieceType capturedPiece =
            isOccupied(to, board)
                ? (isWhite ? (board.blackBoard & to
                                  ? (board.blackPawns & to     ? PAWN
                                     : board.blackKnights & to ? KNIGHT
                                     : board.blackBishops & to ? BISHOP
                                     : board.blackRooks & to   ? ROOK
                                     : board.blackQueen & to   ? QUEEN
                                                               : KING)
                                  : NONE)
                           : (board.whiteBoard & to
                                  ? (board.whitePawns & to     ? PAWN
                                     : board.whiteKnights & to ? KNIGHT
                                     : board.whiteBishops & to ? BISHOP
                                     : board.whiteRooks & to   ? ROOK
                                     : board.whiteQueen & to   ? QUEEN
                                                               : KING)
                                  : NONE))
                : NONE;
        moves.push_back({(int)__builtin_ctzll(pos) + 1,
                         (int)__builtin_ctzll(to) + 1, type, capturedPiece});
        attacks &= attacks - 1;
      }
      pieces &= pieces - 1;
    }
  };

  processPieces(pawns, PAWN, getPawnMoves);
  processPieces(knights, KNIGHT, getKnightMoves);
  processPieces(bishops, BISHOP, getBishopMoves);
  processPieces(rooks, ROOK, getRookMoves);
  processPieces(queen, QUEEN, getQueenMove);
  processPieces(king, KING, getKingMoves);

  return moves;
}

bool isKingInCheck(Board &board, bool isWhite) {
  uint64_t kingPos = isWhite ? board.whiteKing : board.blackKing;
  uint64_t attackedSqrs = 0ULL;
  bool oppColor = !isWhite;

  uint64_t oppPawns = isWhite ? board.blackPawns : board.whitePawns;
  uint64_t oppKnights = isWhite ? board.blackKnights : board.whiteKnights;
  uint64_t oppBishops = isWhite ? board.blackBishops : board.whiteBishops;
  uint64_t oppRooks = isWhite ? board.blackRooks : board.whiteRooks;
  uint64_t oppQueen = isWhite ? board.blackQueen : board.whiteQueen;
  uint64_t oppKing = isWhite ? board.blackKing : board.whiteKing;

  if (isWhite)
    attackedSqrs |=
        ((oppPawns >> 7) & ~board.aFile) | ((oppPawns >> 9) & ~board.hfile);
  else
    attackedSqrs |=
        ((oppPawns << 7) & ~board.hfile) | ((oppPawns << 9) & ~board.aFile);

  while (oppKnights) {
    uint64_t sq = oppKnights & -oppKnights;
    attackedSqrs |= getKnightMoves(sq, board, oppColor);
    oppKnights &= oppKnights - 1;
  }

  while (oppBishops) {
    uint64_t sq = oppBishops & -oppBishops;
    attackedSqrs |= getBishopMoves(sq, board, oppColor);
    oppBishops &= oppBishops - 1;
  }

  while (oppRooks) {
    uint64_t sq = oppRooks & -oppRooks;
    attackedSqrs |= getRookMoves(sq, board, oppColor);
    oppRooks &= oppRooks - 1;
  }

  attackedSqrs |= getQueenMove(oppQueen, board, oppColor);
  attackedSqrs |= getKingMoves(oppKing, board, oppColor);

  return attackedSqrs & kingPos;
}
