#pragma once

namespace chess {
class Board;
struct Position;
enum struct Side;
} // namespace chess

namespace chess::score {
class UnderAttack {
public:
  [[nodiscard]] double operator()(const chess::Board &board,
                                  const chess::Position &position,
                                  chess::Side side) const;
};
} // namespace chess::score
