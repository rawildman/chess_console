#include "defends_attack.hpp"

#include <core/board.hpp>
#include <core/board_view.hpp>
#include <core/logic.hpp>

#include <functional>
#include <ranges>
#include <unordered_map>

namespace chess::score {

namespace {
std::unordered_map<chess::Piece, double> kPieceValues = {
    {Piece::kPawn, 1.0}, {Piece::kKnight, 3.0}, {Piece::kBishop, 3.0},
    {Piece::kRook, 5.0}, {Piece::kQueen, 9.0},  {Piece::kKing, 100.0},
};

class HasPieceWithSide {
public:
  HasPieceWithSide(const chess::Side side) : mSide(side) {}

  [[nodiscard]] bool
  operator()(const chess::Board::Iterator::value_type &stateAndPosition) const {
    return stateAndPosition.first.has_value()
               ? stateAndPosition.first->mSide == mSide
               : false;
  }

private:
  chess::Side mSide;
};
} // namespace

double DefendsAttack::operator()(const chess::Board &board,
                                 const chess::IntendedMove &move) const {
  const chess::Side defendingSide = move.piece.mSide;
  double defendedValue = 0.0;
  namespace rv = std::ranges::views;
  for (const auto [state, position] :
       chess::BoardView{board} | rv::filter(HasPieceWithSide{defendingSide})) {

    const chess::UnderAttack underAttackBefore =
        chess::underAttack(position, defendingSide, board);
    if (underAttackBefore.bUnderAttack) {
      const chess::UnderAttack underAttackAfter =
          chess::underAttack(position, defendingSide, board, move);
      defendedValue +=
          underAttackAfter.bUnderAttack ? 0.0 : kPieceValues.at(state->mPiece);
    }
  }
  return defendedValue;
}
} // namespace chess::score

#if defined(UNIT_TEST)

#include "test_utility.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("HasPieceWithSide range loop") {
  namespace cs = chess::score;
  namespace rv = std::ranges::views;

  const auto board = chess::Board{};
  int count = 0;
  for (const auto [state, position] :
       chess::BoardView{board} |
           rv::filter(cs::HasPieceWithSide{chess::Side::kWhite})) {
    ++count;
  }
  constexpr int numPieces = 16;
  CHECK(count == numPieces);
}

TEST_CASE("DefendsAttack") {
  using namespace chess::pieces;

  const auto scorer = chess::score::DefendsAttack{};
  // clang-format off
  constexpr chess::Board::BoardArray board{
    E, E, E, E, E, E, E, E,
    E, E, Q, E, k, E, E, E,
    E, E, E, q, E, E, E, E,
    E, E, E, E, E, E, E, E,
    E, E, E, E, E, E, E, E,
    E, E, E, E, E, E, E, E,
    E, E, E, E, E, E, E, E,
    E, E, E, E, E, E, E, E};
  // clang-format on
  const auto queenDefendsKing = chess::IntendedMove{
      .piece = {.mPiece = chess::Piece::kQueen, .mSide = chess::Side::kBlack},
      .from = {2, 3},
      .to = {1, 3}};
  CHECK(scorer(board, queenDefendsKing) == 100.0);

  const auto noDefense = chess::IntendedMove{
      .piece = {.mPiece = chess::Piece::kQueen, .mSide = chess::Side::kBlack},
      .from = {2, 3},
      .to = {0, 3}};
  CHECK(scorer(board, noDefense) == 0.0);
}

#endif
