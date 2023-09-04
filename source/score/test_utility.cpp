#include "test_utility.hpp"

#include <core/board.hpp>

namespace chess::score::test {
/// Generates a test move, the piece and starting location are arbitrary.
chess::IntendedMove testMove(const chess::Position &move,
                             const chess::Side side) {
  return {.piece = {.mPiece = chess::Piece::kPawn, .mSide = side},
          .from = {},
          .to = move};
}

} // namespace chess::score::test