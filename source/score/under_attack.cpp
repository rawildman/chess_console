#include "under_attack.hpp"

#include <core/board.hpp>
#include <core/logic.hpp>

namespace chess::score {

double UnderAttack::operator()(const chess::Board &board,
                               const chess::IntendedMove &move) const {
  const chess::UnderAttack result =
      chess::underAttack(move.to, move.chPiece.mSide, board);
  return -result.iNumAttackers;
}

} // namespace chess::score

#if defined(UNIT_TEST)

#include "test_utility.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Initial board") {
  namespace cst = chess::score::test;

  const auto scorer = chess::score::UnderAttack{};
  const auto board = chess::Board{};
  CHECK(scorer(board, cst::testMove(chess::Position{2, 0},
                                    chess::Side::kBlack)) == -2.0);
  CHECK(scorer(board, cst::testMove(chess::Position{5, 6},
                                    chess::Side::kWhite)) == -2.0);
  CHECK(scorer(board, cst::testMove(chess::Position{3, 3},
                                    chess::Side::kWhite)) == 0.0);
  CHECK(scorer(board, cst::testMove(chess::Position{3, 3},
                                    chess::Side::kBlack)) == 0.0);
}

#endif
