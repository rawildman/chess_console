#pragma once

#include "pieces.hpp"

#include <array>
#include <functional>
#include <initializer_list>
#include <optional>
#include <ostream>

namespace chess {
constexpr int kNumRows = 8;
constexpr int kNumCols = 8;
constexpr int kNumPositions = kNumRows * kNumCols;

struct Position {
  int iRow = 0;
  int iColumn = 0;

  constexpr bool operator==(const Position &) const = default;
  constexpr bool operator!=(const Position &) const = default;
};

std::ostream &operator<<(std::ostream &stream, const Position &pos);

struct IntendedMove {
  PieceWithSide piece;
  Position from;
  Position to;
};

namespace pieces {
// clang-format off
constexpr std::array<SquareState, kNumPositions> kInitialBoardState = {
    R, N, B, Q, K, B, N, R,
    P, P, P, P, P, P, P, P,
    E, E, E, E, E, E, E, E,
    E, E, E, E, E, E, E, E,
    E, E, E, E, E, E, E, E,
    E, E, E, E, E, E, E, E,
    p, p, p, p, p, p, p, p,
    r, n, b, q, k, b, n, r};
// clang-format on
} // namespace pieces

class Board {
public:
  using BoardArray = std::array<SquareState, kNumPositions>;

  Board() = default;
  Board(BoardArray initial_board);

  [[nodiscard]] SquareState &operator()(int row, int col);
  [[nodiscard]] SquareState operator()(int row, int col) const;

  [[nodiscard]] SquareState &operator()(Position pos);
  [[nodiscard]] SquareState operator()(Position pos) const;

  [[nodiscard]] SquareState getPieceConsiderMove(
      Position pos,
      const std::optional<IntendedMove> &intended_move = std::nullopt) const;

  [[nodiscard]] const BoardArray &boardState() const;

  struct EndSentinel {};
  class Iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<SquareState, Position>;
    using difference_type = std::ptrdiff_t;

    Iterator(const BoardArray &board);

    [[nodiscard]] value_type operator*() const;
    Iterator &operator++();
    Iterator operator++(int);

    [[nodiscard]] bool operator==(const Iterator &rhs) const;
    [[nodiscard]] bool operator==(EndSentinel rhs) const;

  private:
    Position mPos{.iRow = 0, .iColumn = 0};
    std::reference_wrapper<const BoardArray> mBoard;
  };

  [[nodiscard]] Iterator begin() const;
  [[nodiscard]] EndSentinel end() const;

private:
  BoardArray mBoard = pieces::kInitialBoardState;
};

/// @brief Converts a character representation to a PieceWithSide.
///
/// Upper case indicates white and lower case indicates black. Characters
/// are the first letter of the piece name, aside from knight, which is N.
[[nodiscard]] PieceWithSide charToPiece(char piece);

/// Upper case indicates white and lower case indicates black. Characters
/// are the first letter of the piece name, aside from knight, which is N.
[[nodiscard]] char pieceToChar(PieceWithSide piece);

/// @brief Checks if the position given by @a pos has a row in the range
///  [0, 7] and a column in the range [0, 7]
[[nodiscard]] bool validBoardPosition(const Position &pos);

/// @return The Position of the king on side @a side.
[[nodiscard]] Position findKing(const Board &board, Side side);

/// @return The Side opposite of @a side. E.g. if @a side is white, returns
/// black.
[[nodiscard]] Side opponentSide(Side side);

} // namespace chess
