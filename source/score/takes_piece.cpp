#include "takes_piece.hpp"

#include <core/board.hpp>

#include <unordered_map>

namespace chess::score {
namespace {
std::unordered_map<chess::Piece, double> kPieceValues = {
    {Piece::kPawn, 1.0}, {Piece::kKnight, 3.0}, {Piece::kBishop, 3.0},
    {Piece::kRook, 5.0}, {Piece::kQueen, 9.0},  {Piece::kKing, 100.0},
};
}

double TakesPiece::operator()(const chess::Board &board,
                              const chess::Position &position,
                              const chess::Side side) const {
  const chess::SquareState result = board(position);
  if (result.has_value() && result->mSide != side) {
    return kPieceValues.at(result->mPiece);
  } else {
    return 0.0;
  }
}

} // namespace chess::score

#if defined(UNIT_TEST)

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Takes piece") {
  const auto scorer = chess::score::TakesPiece{};
  const auto board = chess::Board{};
  CHECK(scorer(board, chess::Position{0, 0}, chess::Side::kBlack) ==
        5.0); // Rook
  CHECK(scorer(board, chess::Position{0, 1}, chess::Side::kBlack) ==
        3.0); // Knight
  CHECK(scorer(board, chess::Position{0, 2}, chess::Side::kBlack) ==
        3.0); // Bishop
  CHECK(scorer(board, chess::Position{0, 3}, chess::Side::kBlack) ==
        9.0); // Queen
  CHECK(scorer(board, chess::Position{0, 4}, chess::Side::kBlack) ==
        100.0); // King
  CHECK(scorer(board, chess::Position{1, 4}, chess::Side::kBlack) ==
        1.0); // Pawn
  CHECK(scorer(board, chess::Position{7, 7}, chess::Side::kBlack) ==
        0.0); // Own side

  CHECK(scorer(board, chess::Position{7, 7}, chess::Side::kWhite) ==
        5.0); // Rook
  CHECK(scorer(board, chess::Position{7, 6}, chess::Side::kWhite) ==
        3.0); // Knight
  CHECK(scorer(board, chess::Position{7, 5}, chess::Side::kWhite) ==
        3.0); // Bishop
  CHECK(scorer(board, chess::Position{7, 4}, chess::Side::kWhite) ==
        100.0); // King
  CHECK(scorer(board, chess::Position{7, 3}, chess::Side::kWhite) ==
        9.0); // Queen
  CHECK(scorer(board, chess::Position{6, 3}, chess::Side::kWhite) ==
        1.0); // Pawn
  CHECK(scorer(board, chess::Position{0, 0}, chess::Side::kWhite) ==
        0.0); // Own side

  CHECK(scorer(board, chess::Position{3, 3}, chess::Side::kWhite) ==
        0.0); // Empty
  CHECK(scorer(board, chess::Position{3, 3}, chess::Side::kBlack) ==
        0.0); // Empty
}

#endif
