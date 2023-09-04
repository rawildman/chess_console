#include "threatens_king.hpp"

#include <core/board.hpp>
#include <core/logic.hpp>

namespace chess::score {
double ThreatensKing::operator()(const chess::Board &board,
                                 const chess::IntendedMove &move) const {
  const bool kingInCheck =
      chess::isKingInCheck(board, chess::opponentSide(move.piece.mSide), move);
  return kingInCheck ? 1.0 : 0.0;
}
} // namespace chess::score

#if defined(UNIT_TEST)

#include "test_utility.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Threatens King") {
  using namespace chess::pieces;
  namespace cst = chess::score::test;

  const auto scorer = chess::score::ThreatensKing{};
  SECTION("Initial board") {
    const auto board = chess::Board{};
    CHECK(scorer(board, cst::testMove(chess::Position{0, 0},
                                      chess::Side::kBlack)) == 0.0);
  }
  SECTION("Modified board") {
    // clang-format off
    constexpr chess::Board::BoardArray board{
      E, E, E, E, E, E, E, E,
      E, E, E, E, k, E, E, E,
      E, E, Q, E, E, E, E, E,
      E, E, E, E, E, E, E, E,
      E, E, E, E, E, E, E, E,
      E, E, E, E, E, E, E, E,
      E, E, E, K, E, E, E, E,
      E, E, E, E, E, E, n, E};
    // clang-format on
    const auto queenCheck = chess::IntendedMove{
        .piece = {.mPiece = chess::Piece::kQueen, .mSide = chess::Side::kWhite},
        .from = {2, 2},
        .to = {2, 3}};
    CHECK(scorer(board, queenCheck) == 1.0);

    const auto knightCheck =
        chess::IntendedMove{.piece = {.mPiece = chess::Piece::kKnight,
                                      .mSide = chess::Side::kBlack},
                            .from = {7, 6},
                            .to = {5, 5}};
    CHECK(scorer(board, queenCheck) == 1.0);
  }
}

#endif
