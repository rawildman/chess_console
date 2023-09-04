#include "escapes_attack.hpp"

#include <core/board.hpp>
#include <core/logic.hpp>

namespace chess::score {
double EscapesAttack::operator()(const chess::Board &board,
                                 const chess::IntendedMove &move) const {
  const chess::UnderAttack underAttackAtOriginalLocation =
      chess::underAttack(move.from, move.piece.mSide, board);
  if (!underAttackAtOriginalLocation.bUnderAttack) {
    // Quick exit if we're not actually under attack already
    return 0.0;
  }

  const chess::UnderAttack underAttackAtNewLocation =
      chess::underAttack(move.to, move.piece.mSide, board);
  const bool escapesAttack = underAttackAtOriginalLocation.bUnderAttack &&
                             !underAttackAtNewLocation.bUnderAttack;
  return escapesAttack ? 1.0 : 0.0;
}
} // namespace chess::score

#if defined(UNIT_TEST)

#include "test_utility.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Escapes attack") {
  using namespace chess::pieces;
  namespace cst = chess::score::test;

  const auto scorer = chess::score::EscapesAttack{};
  SECTION("Initial board") {
    const auto board = chess::Board{};
    const auto pawnMove = chess::IntendedMove{
        .piece = {.mPiece = chess::Piece::kPawn, .mSide = chess::Side::kWhite},
        .from = {1, 0},
        .to = {3, 0}};
    CHECK(scorer(board, pawnMove) == 0.0);
  }
  SECTION("Modified board") {
    // clang-format off
    constexpr chess::Board::BoardArray board{
      E, E, E, E, E, E, E, E,
      E, E, E, E, k, E, E, E,
      E, E, Q, E, E, E, E, E,
      E, E, n, E, E, E, E, E,
      E, E, E, E, E, E, E, E,
      E, E, E, E, E, E, E, E,
      B, E, E, K, E, E, E, E,
      E, E, E, E, E, E, n, E};
    // clang-format on
    const auto knightSafeMove =
        chess::IntendedMove{.piece = {.mPiece = chess::Piece::kKnight,
                                      .mSide = chess::Side::kBlack},
                            .from = {3, 2},
                            .to = {5, 3}};
    CHECK(scorer(board, knightSafeMove) == 1.0);
    const auto knightUnsafeMove =
        chess::IntendedMove{.piece = {.mPiece = chess::Piece::kKnight,
                                      .mSide = chess::Side::kBlack},
                            .from = {3, 2},
                            .to = {5, 1}};
    CHECK(scorer(board, knightUnsafeMove) == 0.0);
  }
}

#endif
