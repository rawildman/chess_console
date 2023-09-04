#pragma once

#include "board.hpp"

#include <ranges>

namespace chess {

class BoardView : public std::ranges::view_interface<BoardView> {
public:
  BoardView(const Board &board);

  Board::Iterator begin() const;
  Board::EndSentinel end() const;

private:
  Board::Iterator mBegin;
};
} // namespace chess