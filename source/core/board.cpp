#include "board.hpp"

#include <cassert>
#include <iostream>
#include <map>
#include <unordered_map>

namespace chess {
namespace {
void affirmIndices(const int row, const int col) {
  assert(row >= 0 && row < kNumRows);
  assert(col >= 0 && col < kNumCols);
}

class BoardPositions {
public:
  class BoardPositionsIterator {
  public:
    struct EndTag {};

    BoardPositionsIterator() = default;
    BoardPositionsIterator(const EndTag)
        : mPos(Position{.iRow = kNumRows, .iColumn = 0}) {}

    Position operator*() const { return mPos; }
    BoardPositionsIterator operator++() {
      ++mPos.iColumn;
      if (mPos.iColumn == kNumCols) {
        mPos.iColumn = 0;
        ++mPos.iRow;
      }
      return *this;
    }
    bool operator==(const BoardPositionsIterator &) const = default;
    bool operator!=(const BoardPositionsIterator &) const = default;

  private:
    Position mPos{.iRow = 0, .iColumn = 0};
  };

  BoardPositionsIterator begin() const { return BoardPositionsIterator{}; }
  BoardPositionsIterator end() const {
    return BoardPositionsIterator{BoardPositionsIterator::EndTag{}};
  }
};

/// @brief The index into a linear array representing the 2D position given by
///  @a row and @a col. Uses layout-right (row-major) ordering.
/// @param row Must be >= 0 and less than 8
/// @param col Must be >= 0 and less than 8
int linearIndex(const int row, const int col) { return row * kNumCols + col; }

const std::unordered_map<char, PieceWithSide> kCharToPieces = {
    {'P', pieces::P}, {'p', pieces::p}, {'B', pieces::B}, {'b', pieces::b},
    {'R', pieces::R}, {'r', pieces::r}, {'N', pieces::N}, {'n', pieces::n},
    {'Q', pieces::Q}, {'q', pieces::q}, {'K', pieces::K}, {'k', pieces::k}};

const std::map<PieceWithSide, char> kPiecesToChar = {
    {pieces::P, 'P'}, {pieces::p, 'p'}, {pieces::B, 'B'}, {pieces::b, 'b'},
    {pieces::R, 'R'}, {pieces::r, 'r'}, {pieces::N, 'N'}, {pieces::n, 'n'},
    {pieces::Q, 'Q'}, {pieces::q, 'q'}, {pieces::K, 'K'}, {pieces::k, 'k'}};
} // namespace

Board::Board(std::array<SquareState, kNumPositions> initial_board)
    : mBoard(initial_board) {}

std::ostream &operator<<(std::ostream &stream, const Position &pos) {
  stream << static_cast<char>(pos.iColumn + 'A')
         << static_cast<char>(pos.iRow + '1');
  return stream;
}

std::ostream &operator<<(std::ostream &stream, const PieceWithSide &piece) {
  stream << pieceToChar(piece);
  return stream;
}

std::ostream &operator<<(std::ostream &stream, const SquareState &state) {
  stream << (state ? pieceToChar(*state) : 'E');
  return stream;
}

SquareState &Board::operator()(const int row, const int col) {
  affirmIndices(row, col);
  return mBoard[linearIndex(row, col)];
}

SquareState Board::operator()(const int row, const int col) const {
  affirmIndices(row, col);
  return mBoard[linearIndex(row, col)];
}

SquareState &Board::operator()(const Position pos) {
  return (*this)(pos.iRow, pos.iColumn);
}

SquareState Board::operator()(const Position pos) const {
  return (*this)(pos.iRow, pos.iColumn);
}

SquareState Board::getPieceConsiderMove(
    const Position pos,
    const std::optional<IntendedMove> &intended_move) const {
  if (intended_move) {
    SquareState chPiece = (*this)(pos);
    // In this case, we are trying to understand what WOULD happed if the move
    // was made, so we consider a move that has not been made yet
    if (intended_move->from == pos) {
      // The piece wants to move from that square, so it would be empty
      chPiece = pieces::E;
    } else if (intended_move->to == pos) {
      // The piece wants to move to that square, so return the piece
      chPiece = intended_move->chPiece;
    }
    return chPiece;
  } else {
    return (*this)(pos);
  }
}

const Board::BoardArray &Board::boardState() const { return mBoard; }

PieceWithSide charToPiece(const char piece) { return kCharToPieces.at(piece); }

char pieceToChar(const PieceWithSide piece) { return kPiecesToChar.at(piece); }

bool validBoardPosition(const Position &pos) {
  return pos.iRow >= 0 && pos.iRow < kNumRows && pos.iColumn >= 0 &&
         pos.iColumn < kNumCols;
}

Position findKing(const Board &board, const Side side) {
  const PieceWithSide chKing{.mPiece = Piece::kKing, .mSide = side};
  Position king;

  for (const Position pos : BoardPositions{}) {
    if (const SquareState square = board(pos); square && *square == chKing) {
      king = pos;
    }
  }

  return king;
}

Side opponentSide(const Side side) {
  return side == Side::kWhite ? Side::kBlack : Side::kWhite;
}

} // namespace chess

#if defined(UNIT_TEST)

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Initial board") {
  chess::Board board;
  CHECK(board(0, 0) == chess::pieces::R);
  CHECK(board(1, 0) == chess::pieces::P);
  CHECK(board(0, 3) == chess::pieces::Q);
  CHECK(board(0, 4) == chess::pieces::K);
  CHECK(board(0, 7) == chess::pieces::R);

  CHECK(board(3, 7) == std::nullopt);

  CHECK(board(chess::kNumRows - 1, chess::kNumCols - 1) == chess::pieces::r);
}

TEST_CASE("Piece conversion pieceToChar") {
  CHECK(chess::pieceToChar(chess::pieces::P) == 'P');
  CHECK(chess::pieceToChar(chess::pieces::p) == 'p');
}

TEST_CASE("Piece conversion charToPiece") {
  CHECK(chess::charToPiece('P') == chess::pieces::P);
  CHECK(chess::charToPiece('p') == chess::pieces::p);
  CHECK(chess::charToPiece('Q') == chess::pieces::Q);
  CHECK(chess::charToPiece('q') == chess::pieces::q);
}

TEST_CASE("Board validBoardPosition") {
  // Valid
  CHECK(chess::validBoardPosition(chess::Position{.iRow = 0, .iColumn = 0}));
  CHECK(chess::validBoardPosition(
      chess::Position{.iRow = chess::kNumRows - 1, .iColumn = 0}));
  CHECK(chess::validBoardPosition(
      chess::Position{.iRow = 0, .iColumn = chess::kNumCols - 1}));
  CHECK(chess::validBoardPosition(chess::Position{
      .iRow = chess::kNumRows - 1, .iColumn = chess::kNumCols - 1}));

  // Invalid
  CHECK(!chess::validBoardPosition(chess::Position{.iRow = -1, .iColumn = 0}));
  CHECK(!chess::validBoardPosition(chess::Position{.iRow = 0, .iColumn = -1}));
  CHECK(!chess::validBoardPosition(
      chess::Position{.iRow = chess::kNumRows, .iColumn = 0}));
  CHECK(!chess::validBoardPosition(
      chess::Position{.iRow = chess::kNumRows, .iColumn = -1}));
  CHECK(!chess::validBoardPosition(
      chess::Position{.iRow = -1, .iColumn = chess::kNumCols}));
  CHECK(!chess::validBoardPosition(
      chess::Position{.iRow = chess::kNumRows, .iColumn = chess::kNumCols}));
}

TEST_CASE("Board getPieceConsideredMove") {
  const chess::Board board;
  // No intended move
  CHECK(board.getPieceConsiderMove(chess::Position{0, 0}).has_value());
  // With intended move of pawn
  constexpr auto intendedMove =
      chess::IntendedMove{.chPiece = chess::pieces::P,
                          .from = chess::Position{.iRow = 1, .iColumn = 0},
                          .to = chess::Position{.iRow = 3, .iColumn = 0}};
  CHECK(!board.getPieceConsiderMove(chess::Position{1, 0}, intendedMove)
             .has_value());
  CHECK(board.getPieceConsiderMove(chess::Position{3, 0}, intendedMove)
            .has_value());
  CHECK(
      board.getPieceConsiderMove(chess::Position{3, 0}, intendedMove).value() ==
      chess::pieces::P);
}

TEST_CASE("Board findKing") {
  const chess::Board board;
  const chess::Position whiteKingPosition =
      findKing(board, chess::Side::kWhite);
  CHECK(whiteKingPosition == chess::Position{0, 4});

  const chess::Position blackKingPosition =
      findKing(board, chess::Side::kBlack);
  CHECK(blackKingPosition == chess::Position{7, 4});
}

TEST_CASE("Board opponentSide") {
  CHECK(chess::opponentSide(chess::Side::kWhite) == chess::Side::kBlack);
  CHECK(chess::opponentSide(chess::Side::kBlack) == chess::Side::kWhite);
}

#endif
