#pragma once

#include "board.hpp"

namespace chess {

class BoardPositions {
public:
  struct EndSentinel {};
  class Iterator {
  public:
    Iterator() = default;

    [[nodiscard]] Position operator*() const;
    Iterator operator++();

    [[nodiscard]] bool operator==(const Iterator &) const = default;
    [[nodiscard]] bool operator==(EndSentinel) const;

  private:
    Position mPos{.iRow = 0, .iColumn = 0};
  };

  [[nodiscard]] Iterator begin() const;
  [[nodiscard]] EndSentinel end() const;
};
} // namespace chess