#pragma once

#include "core_fwds.hpp"

namespace chess::score::test {
/// Generates a test move to position @a move for side @a side.
/// The piece and starting location are arbitrary.
chess::IntendedMove testMove(const chess::Position &move,
                             const chess::Side side);

} // namespace chess::score::test