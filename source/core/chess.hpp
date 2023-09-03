#pragma once
#include "board.hpp"

#include <array>

namespace chess {
Side getPieceColor(PieceWithSide piece);

bool isWhitePiece(PieceWithSide piece);

bool isBlackPiece(PieceWithSide piece);

std::string describePiece(PieceWithSide piece);

enum struct BoardSide { QUEEN_SIDE = 2, KING_SIDE = 3 };

enum struct Direction { HORIZONTAL = 0, VERTICAL, DIAGONAL, L_SHAPE };

struct EnPassant {
  bool bApplied = false;
  Position PawnCaptured{};
};

struct Castling {
  bool bApplied = false;
  Position rook_before{};
  Position rook_after{};
};

struct Promotion {
  bool bApplied = false;
  PieceWithSide chBefore;
  PieceWithSide chAfter;
};

struct Attacker {
  Position pos;
  Direction dir = Direction::HORIZONTAL;

  bool operator==(const Attacker &) const = default;
};

struct UnderAttack {
  bool bUnderAttack = false;
  int iNumAttackers = 0;
  std::array<Attacker, 9> attacker; // maximum theorical number of attackers

  bool operator==(const UnderAttack &) const = default;
};
} // namespace chess
