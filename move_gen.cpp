#include "move_gen.h"
#include "board.h"
#include <algorithm>

using namespace std;

uint64_t getKnightMoves(uint64_t POS, Board &board, bool isWhite) {
  uint64_t abFile = board.aFile | board.bFile;
  uint64_t ghFile = board.gfile | board.hfile;
  uint64_t ownPieces = isWhite ? board.whiteBoard : board.blackBoard;

  uint64_t attacks = (POS << 17 & ~board.aFile) | (POS << 15 & ~board.hfile) |
                     (POS << 10 & ~abFile) | (POS << 6 & ~ghFile) |
                     (POS >> 17 & ~board.hfile) | (POS >> 15 & ~board.aFile) |
                     (POS >> 10 & ~ghFile) | (POS >> 6 & ~abFile);

  return attacks & ~ownPieces;
}

uint64_t getRookMoves(uint64_t pos, Board &board, bool isWhite) {
  int sq = __builtin_ctzll(pos);
  int rank = sq / 8, file = sq % 8;
  uint64_t attack = 0;
  uint64_t sameColorPieces = isWhite ? board.whiteBoard : board.blackBoard;

  for (int rr = rank + 1; rr < 8; rr++) {
    uint64_t s = 1ULL << (rr * 8 + file);
    attack |= s;
    if (board.allPieces & s) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }
  for (int rr = rank - 1; rr >= 0; rr--) {
    uint64_t s = 1ULL << (rr * 8 + file);
    attack |= s;
    if (board.allPieces & s) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }
  for (int ff = file + 1; ff < 8; ff++) {
    uint64_t s = 1ULL << (rank * 8 + ff);
    attack |= s;
    if (board.allPieces & s) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }
  for (int ff = file - 1; ff >= 0; ff--) {
    uint64_t s = 1ULL << (rank * 8 + ff);
    attack |= s;
    if (board.allPieces & s) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }
  return attack;
}

uint64_t getBishopMoves(uint64_t pos, Board &board, bool isWhite) {
  int sq = __builtin_ctzll(pos);
  int rank = sq / 8, file = sq % 8;
  uint64_t attack = 0;
  uint64_t sameColorPieces = isWhite ? board.whiteBoard : board.blackBoard;

  for (int i = 1; i < 8; i++) {
    if (rank + i > 7 || file + i > 7)
      break;
    uint64_t s = 1ULL << ((rank + i) * 8 + (file + i));
    attack |= s;
    if (board.allPieces & s) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }
  for (int i = 1; i < 8; i++) {
    if (rank + i > 7 || file - i < 0)
      break;
    uint64_t s = 1ULL << ((rank + i) * 8 + (file - i));
    attack |= s;
    if (board.allPieces & s) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }
  for (int i = 1; i < 8; i++) {
    if (rank - i < 0 || file + i > 7)
      break;
    uint64_t s = 1ULL << ((rank - i) * 8 + (file + i));
    attack |= s;
    if (board.allPieces & s) {
      if (s & sameColorPieces)
        attack ^= s;
      break;
    }
  }
  for (int i = 1; i < 8; i++) {
    if (rank - i < 0 || file - i < 0)
      break;
    uint64_t s = 1ULL << ((rank - i) * 8 + (file - i));
    attack |= s;
    if (board.allPieces & s) {
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

uint64_t getKingMoves(uint64_t pos, Board board, bool isWhite) {
  uint64_t ownPieces = isWhite ? board.whiteBoard : board.blackBoard;
  uint64_t attack = (pos << 8) | (pos >> 8) | (pos << 1 & ~board.aFile) |
                    (pos >> 1 & ~board.hfile) | (pos << 9 & ~board.aFile) |
                    (pos >> 9 & ~board.hfile) | (pos << 7 & ~board.hfile) |
                    (pos >> 7 & ~board.aFile);
  return attack & ~ownPieces;
}

uint64_t getPawnMoves(uint64_t pos, Board &board, bool isWhite) {
  uint64_t moves = 0;
  if (isWhite) {
    uint64_t single = pos << 8;
    if (single && !(single & board.allPieces)) {
      moves |= single;
      if ((pos & 0xFF00) && !(single << 8 & board.allPieces))
        moves |= (single << 8);
    }
    uint64_t leftCap = (pos << 7) & ~board.hfile;
    uint64_t rightCap = (pos << 9) & ~board.aFile;
    moves |= (leftCap | rightCap) & board.blackBoard;
    if (board.enPassantSq != -1) {
      uint64_t epBit = 1ULL << board.enPassantSq;
      if ((leftCap | rightCap) & epBit)
        moves |= epBit;
    }
  } else {
    uint64_t single = pos >> 8;
    if (single && !(single & board.allPieces)) {
      moves |= single;
      if ((pos & 0x00FF000000000000ULL) && !(single >> 8 & board.allPieces))
        moves |= (single >> 8);
    }
    uint64_t leftCap = (pos >> 7) & ~board.aFile;
    uint64_t rightCap = (pos >> 9) & ~board.hfile;
    moves |= (leftCap | rightCap) & board.whiteBoard;
    if (board.enPassantSq != -1) {
      uint64_t epBit = 1ULL << board.enPassantSq;
      if ((leftCap | rightCap) & epBit)
        moves |= epBit;
    }
  }
  return moves;
}

bool isKingInCheck(Board &board, bool isWhite) {
  uint64_t kingPos = isWhite ? board.whiteKing : board.blackKing;
  if (!kingPos)
    return false;
  bool opp = !isWhite;
  uint64_t occupied = board.allPieces;

  // Pawns
  uint64_t oppPawns = isWhite ? board.blackPawns : board.whitePawns;
  if (isWhite) {
    if (((kingPos << 7) & ~board.hfile & oppPawns) ||
        ((kingPos << 9) & ~board.aFile & oppPawns))
      return true;
  } else {
    if (((kingPos >> 7) & ~board.aFile & oppPawns) ||
        ((kingPos >> 9) & ~board.hfile & oppPawns))
      return true;
  }

  // Knights
  uint64_t oppKnights = isWhite ? board.blackKnights : board.whiteKnights;
  while (oppKnights) {
    uint64_t pos = oppKnights & -oppKnights;
    if (getKnightMoves(pos, board, opp) & kingPos)
      return true;
    oppKnights &= oppKnights - 1;
  }

  // Sliders
  auto checkSlider = [&](uint64_t pieces, auto func) {
    while (pieces) {
      uint64_t pos = pieces & -pieces;
      if (func(pos, board, opp) & kingPos)
        return true;
      pieces &= pieces - 1;
    }
    return false;
  };
  if (checkSlider(isWhite ? board.blackBishops : board.whiteBishops,
                  getBishopMoves))
    return true;
  if (checkSlider(isWhite ? board.blackRooks : board.whiteRooks, getRookMoves))
    return true;
  if (checkSlider(isWhite ? board.blackQueen : board.whiteQueen, getQueenMove))
    return true;

  // King
  uint64_t oppKing = isWhite ? board.blackKing : board.whiteKing;
  if (oppKing && (getKingMoves(oppKing, board, opp) & kingPos))
    return true;

  return false;
}

// function to compare the moves..used for ordering the moves properly to search
// for potentially good moves first
bool compareMoves(const Move &a, const Move &b) {
  // captured by pawns should be first, ordering by captured piece points in
  // descending order for example, capturing a queen with a pawn should be
  // first, then capturing a rook with a pawn, and so on..

  if (a.capturedPiece == NONE && b.capturedPiece == NONE)
    return false;

  if (a.capturedPiece != NONE && b.capturedPiece == NONE)
    return true;
  if (a.capturedPiece == NONE && b.capturedPiece != NONE)
    return false;
  // order by diff between piece and captured piece points in descending order
  if (PiecePoint[a.capturedPiece] - PiecePoint[a.piece] >
      PiecePoint[b.capturedPiece] - PiecePoint[b.piece])
    return true;
  if (PiecePoint[a.capturedPiece] - PiecePoint[a.piece] <
      PiecePoint[b.capturedPiece] - PiecePoint[b.piece])
    return false;
  return false;
}

vector<Move> generateAllMoves(Board &board, bool isWhite) {
  vector<Move> moves;
  auto addMove = [&](int from, int to, PieceType p, PieceType cap,
                     bool ep = false, bool cast = false,
                     PieceType prom = NONE) {
    Move m = {from + 1, to + 1, p, cap, prom, cast, ep};
    Board temp = board;
    makeMove(temp, m, isWhite);
    if (!isKingInCheck(temp, isWhite))
      moves.push_back(m);
  };

  uint64_t own = isWhite ? board.whiteBoard : board.blackBoard;
  uint64_t opp = isWhite ? board.blackBoard : board.whiteBoard;

  for (int i = 0; i < 64; i++) {
    uint64_t bit = 1ULL << i;
    if (!(bit & own))
      continue;

    PieceType type = NONE;
    uint64_t targets = 0;
    if (bit & (isWhite ? board.whitePawns : board.blackPawns)) {
      type = PAWN;
      targets = getPawnMoves(bit, board, isWhite);
    } else if (bit & (isWhite ? board.whiteKnights : board.blackKnights)) {
      type = KNIGHT;
      targets = getKnightMoves(bit, board, isWhite);
    } else if (bit & (isWhite ? board.whiteBishops : board.blackBishops)) {
      type = BISHOP;
      targets = getBishopMoves(bit, board, isWhite);
    } else if (bit & (isWhite ? board.whiteRooks : board.blackRooks)) {
      type = ROOK;
      targets = getRookMoves(bit, board, isWhite);
    } else if (bit & (isWhite ? board.whiteQueen : board.blackQueen)) {
      type = QUEEN;
      targets = getQueenMove(bit, board, isWhite);
    } else if (bit & (isWhite ? board.whiteKing : board.blackKing)) {
      type = KING;
      targets = getKingMoves(bit, board, isWhite);

      // Castling
      if (!isKingInCheck(board, isWhite)) {
        if (isWhite) {
          if ((board.castlingRights & 1) && !(board.allPieces & 0x60) &&
              !isKingInCheck(board, true)) { // Kingside
            // Need to check intermediate square
            Board t = board;
            Move m = {5, 6, KING, NONE};
            makeMove(t, m, true);
            if (!isKingInCheck(t, true))
              addMove(4, 6, KING, NONE, false, true);
          }
          if ((board.castlingRights & 2) &&
              !(board.allPieces & 0x0E)) { // Queenside
            Board t = board;
            Move m = {5, 4, KING, NONE};
            makeMove(t, m, true);
            if (!isKingInCheck(t, true))
              addMove(4, 2, KING, NONE, false, true);
          }
        } else {
          if ((board.castlingRights & 4) &&
              !(board.allPieces & (0x60ULL << 56))) {
            Board t = board;
            Move m = {61, 62, KING, NONE};
            makeMove(t, m, false);
            if (!isKingInCheck(t, false))
              addMove(60, 62, KING, NONE, false, true);
          }
          if ((board.castlingRights & 8) &&
              !(board.allPieces & (0x0EULL << 56))) {
            Board t = board;
            Move m = {61, 60, KING, NONE};
            makeMove(t, m, false);
            if (!isKingInCheck(t, false))
              addMove(60, 58, KING, NONE, false, true);
          }
        }
      }
    }

    while (targets) {
      int to = __builtin_ctzll(targets);
      PieceType cap = NONE;
      uint64_t toBit = 1ULL << to;
      if (toBit & (isWhite ? board.blackPawns : board.whitePawns))
        cap = PAWN;
      else if (toBit & (isWhite ? board.blackKnights : board.whiteKnights))
        cap = KNIGHT;
      else if (toBit & (isWhite ? board.blackBishops : board.whiteBishops))
        cap = BISHOP;
      else if (toBit & (isWhite ? board.blackRooks : board.whiteRooks))
        cap = ROOK;
      else if (toBit & (isWhite ? board.blackQueen : board.whiteQueen))
        cap = QUEEN;
      else if (toBit & (isWhite ? board.blackKing : board.whiteKing))
        cap = KING;

      bool ep = (type == PAWN && to == board.enPassantSq);
      if (ep)
        cap = PAWN;

      if (type == PAWN && (to / 8 == 0 || to / 8 == 7)) {
        addMove(i, to, type, cap, ep, false, QUEEN);
        addMove(i, to, type, cap, ep, false, ROOK);
        addMove(i, to, type, cap, ep, false, BISHOP);
        addMove(i, to, type, cap, ep, false, KNIGHT);
      } else {

        addMove(i, to, type, cap, ep);
      }
      targets &= targets - 1;
    }
  }
  sort(moves.begin(), moves.end(), compareMoves);
  return moves;
}
