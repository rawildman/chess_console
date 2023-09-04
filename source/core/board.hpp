#pragma once

#include <array>
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

struct IntendedMove {
  PieceWithSide chPiece;
  Position from;
  Position to;
};

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

private:
  BoardArray mBoard = pieces::kInitialBoardState;
};

[[nodiscard]] PieceWithSide charToPiece(char piece);
[[nodiscard]] char pieceToChar(PieceWithSide piece);

[[nodiscard]] bool validBoardPosition(const Position &pos);
[[nodiscard]] Position findKing(const Board &board, Side side);
[[nodiscard]] Side opponentSide(Side side);

} // namespace chess
