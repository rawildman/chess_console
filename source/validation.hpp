#pragma once

#include "board.h"

namespace chess {
struct EnPassant;
struct Castling;
struct Promotion;
class Game;

bool isMoveValid(const Game &current_game, const Position present,
                 const Position future, chess::EnPassant &S_enPassant,
                 chess::Castling &S_castling, chess::Promotion &S_promotion);
} // namespace chess
