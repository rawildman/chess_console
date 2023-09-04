#include "board_positions.hpp"

namespace chess {

Position BoardPositions::Iterator::operator*() const { return mPos; }

BoardPositions::Iterator BoardPositions::Iterator::operator++() {
  ++mPos.iColumn;
  if (mPos.iColumn == kNumCols) {
    mPos.iColumn = 0;
    ++mPos.iRow;
  }
  return *this;
}

bool BoardPositions::Iterator::operator==(EndSentinel) const {
  return mPos.iRow >= kNumRows;
}

BoardPositions::Iterator BoardPositions::begin() const { return Iterator{}; }

BoardPositions::EndSentinel BoardPositions::end() const {
  return EndSentinel{};
}

} // namespace chess

#if defined(UNIT_TEST)

#include <catch2/catch_test_macros.hpp>

TEST_CASE("BoardPositions iterator values") {
  const auto boardPositions = chess::BoardPositions{};
  auto iter = boardPositions.begin();

  CHECK(*iter == chess::Position{});
  for (int k = 0; k < chess::kNumCols; ++k) {
    CHECK(*iter == chess::Position{.iRow = 0, .iColumn = k});
    ++iter;
  }
  CHECK(*iter == chess::Position{.iRow = 1, .iColumn = 0});
}

TEST_CASE("BoardPositions iterator predicates") {
  const auto boardPositions = chess::BoardPositions{};
  const auto begin = boardPositions.begin();
  const auto end = boardPositions.end();
  CHECK(begin == begin);
  CHECK(begin != end);

  auto iter = begin;
  ++iter;
  CHECK(iter == iter);
  CHECK(begin != iter);
  CHECK(end != iter);
}

TEST_CASE("BoardPositions for loop") {
  int count = 0;
  for (const chess::Position position : chess::BoardPositions{}) {
    ++count;
  }
  CHECK(count == chess::kNumPositions);
}

#endif
