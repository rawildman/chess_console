#pragma once

#include "core_fwds.hpp"

namespace chess::score {
class TakesPiece {
public:
  [[nodiscard]] double operator()(const chess::Board &board,
                                  const chess::IntendedMove &move) const;
};
} // namespace chess::score
