#pragma once

#include <array>
#include <optional>
#include <ostream>

namespace chess {
enum struct Piece { kPawn, kRook, kKnight, kBishop, kQueen, kKing };

enum struct Side { kWhite, kBlack };

struct PieceWithSide {
  Piece mPiece = Piece::kPawn;
  Side mSide = Side::kBlack;

  auto operator<=>(const PieceWithSide &) const = default;
};
std::ostream &operator<<(std::ostream &stream, const PieceWithSide &piece);

using SquareState = std::optional<PieceWithSide>;

std::ostream &operator<<(std::ostream &stream, const SquareState &piece);

namespace pieces {
constexpr auto R = PieceWithSide{.mPiece = Piece::kRook, .mSide = Side::kWhite};
constexpr auto N =
    PieceWithSide{.mPiece = Piece::kKnight, .mSide = Side::kWhite};
constexpr auto B =
    PieceWithSide{.mPiece = Piece::kBishop, .mSide = Side::kWhite};
constexpr auto Q =
    PieceWithSide{.mPiece = Piece::kQueen, .mSide = Side::kWhite};
constexpr auto K = PieceWithSide{.mPiece = Piece::kKing, .mSide = Side::kWhite};
constexpr auto P = PieceWithSide{.mPiece = Piece::kPawn, .mSide = Side::kWhite};
constexpr auto r = PieceWithSide{.mPiece = Piece::kRook, .mSide = Side::kBlack};
constexpr auto n =
    PieceWithSide{.mPiece = Piece::kKnight, .mSide = Side::kBlack};
constexpr auto b =
    PieceWithSide{.mPiece = Piece::kBishop, .mSide = Side::kBlack};
constexpr auto q =
    PieceWithSide{.mPiece = Piece::kQueen, .mSide = Side::kBlack};
constexpr auto k = PieceWithSide{.mPiece = Piece::kKing, .mSide = Side::kBlack};
constexpr auto p = PieceWithSide{.mPiece = Piece::kPawn, .mSide = Side::kBlack};
constexpr auto E = std::nullopt;
} // namespace pieces

} // namespace chess