#pragma once

#include "board.h"
#include "chess.h"

#include <optional>

namespace chess {
/// @param pos Position at which to check if it is under attack
/// @param iColor Side which would be under attack (defending side)
/// @param intended_move A potential move to consider
UnderAttack
underAttack(Position pos, Side iColor, const Board &board,
            const std::optional<IntendedMove> &intended_move = std::nullopt);

template <typename Indexer>
auto emptySquareFunctor(const Indexer &indexer, const Board &board,
                        const std::optional<IntendedMove> &intended_move) {
  return [indexer, &board, intended_move](const int index) {
    return !board.getPieceConsiderMove(indexer(index), intended_move)
                .has_value();
  };
}

template <typename Indexer>
auto occupiedSquareFunctor(const Indexer &indexer, const Board &board,
                           const std::optional<IntendedMove> &intended_move) {
  return [indexer, &board, intended_move](const int index) {
    const Position pos = indexer(index);
    return validBoardPosition(pos) &&
           board.getPieceConsiderMove(pos, intended_move).has_value();
  };
}

} // namespace chess
