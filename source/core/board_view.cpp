#include "board_view.hpp"

namespace chess {

BoardView::BoardView(const Board &board) : mBegin(board.begin()) {}

Board::Iterator BoardView::begin() const { return mBegin; }

Board::EndSentinel BoardView::end() const { return Board::EndSentinel{}; }

} // namespace chess

#if defined(UNIT_TEST)

#include <catch2/catch_test_macros.hpp>

TEST_CASE("BoardView range loop") {
  int count = 0;
  const auto board = chess::Board{};
  for (const auto [state, position] : chess::BoardView{board}) {
    ++count;
  }
  CHECK(count == chess::kNumPositions);
}

TEST_CASE("BoardView side filter") {
  static_assert(std::input_iterator<chess::Board::Iterator>);
  int count = 0;
  const auto board = chess::Board{};
  const auto black_side =
      [side = chess::Side::kBlack](
          const chess::Board::Iterator::value_type &stateAndPosition) {
        return stateAndPosition.first.has_value()
                   ? stateAndPosition.first->mSide == side
                   : false;
      };
  for (const auto [state, position] :
       chess::BoardView{board} | std::ranges::views::filter(black_side)) {
    ++count;
  }
  constexpr int numPieces = 16;
  CHECK(count == numPieces);
}

#endif