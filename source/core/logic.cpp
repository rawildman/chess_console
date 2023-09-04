#include "logic.hpp"

#include <algorithm>
#include <cassert>
#include <ranges>

namespace chess {
namespace {
constexpr std::array<Position, 8> knight_moves = {
    Position{1, -2},  Position{2, -1},  Position{2, 1},  Position{1, 2},
    Position{-1, -2}, Position{-2, -1}, Position{-2, 1}, Position{-1, 2}};

void updateAttack(UnderAttack &attack, const Position position,
                  const Direction direction) {
  attack.bUnderAttack = true;
  assert(attack.iNumAttackers >= 0);
  assert(attack.iNumAttackers < 9);
  attack.attacker[attack.iNumAttackers].pos = position;
  attack.attacker[attack.iNumAttackers].dir = direction;
  ++attack.iNumAttackers;
}
} // namespace

bool isKingInCheck(const Board &board, Side side,
                   const std::optional<IntendedMove> &intended_move) {
  const Position king =
      intended_move.has_value() && Piece::kKing == intended_move->chPiece.mPiece
          ? intended_move->to
          : findKing(board, side);
  const UnderAttack king_attacked =
      underAttack(king, side, board, intended_move);
  return king_attacked.bUnderAttack;
}

UnderAttack underAttack(const Position pos, const Side side, const Board &board,
                        const std::optional<IntendedMove> &intended_move) {
  UnderAttack attack;
  // a) Direction: HORIZONTAL
  // Check to left
  if (pos.iColumn > 0) {
    const auto leftIndexer = [pos](const int j) {
      return Position{pos.iRow, pos.iColumn - j};
    };
    for (const int j : std::views::iota(1, pos.iColumn + 1) |
                           std::views::drop_while(emptySquareFunctor(
                               leftIndexer, board, intended_move))) {
      const Position checkPos = leftIndexer(j);
      const SquareState chPieceFound =
          board.getPieceConsiderMove(checkPos, intended_move);
      if (side != getPieceSide(chPieceFound.value()) &&
          (chPieceFound->mPiece == Piece::kQueen ||
           chPieceFound->mPiece == Piece::kRook)) {
        updateAttack(attack, checkPos, Direction::HORIZONTAL);
      }
      break;
    }
  }
  // Check to right
  if (pos.iColumn < kNumCols - 1) {
    const auto rightIndexer = [pos](const int j) {
      return Position{pos.iRow, pos.iColumn + j};
    };
    for (const int j : std::views::iota(1, kNumCols - pos.iColumn) |
                           std::views::drop_while(emptySquareFunctor(
                               rightIndexer, board, intended_move))) {
      const Position checkPos = rightIndexer(j);
      const SquareState chPieceFound =
          board.getPieceConsiderMove(checkPos, intended_move);
      if (side != getPieceSide(chPieceFound.value()) &&
          (chPieceFound->mPiece == Piece::kQueen ||
           chPieceFound->mPiece == Piece::kRook)) {
        updateAttack(attack, checkPos, Direction::HORIZONTAL);
      }
      break;
    }
  }
  // b) Direction: VERTICAL
  // check below
  if (pos.iRow > 0) {
    const auto downIndexer = [pos](const int i) {
      return Position{pos.iRow - i, pos.iColumn};
    };
    for (const int i : std::views::iota(1, pos.iRow + 1) |
                           std::views::drop_while(emptySquareFunctor(
                               downIndexer, board, intended_move))) {
      const Position checkPos = downIndexer(i);
      const SquareState chPieceFound =
          board.getPieceConsiderMove(checkPos, intended_move);
      if (side != getPieceSide(chPieceFound.value()) &&
          (chPieceFound->mPiece == Piece::kQueen ||
           chPieceFound->mPiece == Piece::kRook)) {
        updateAttack(attack, checkPos, Direction::VERTICAL);
      }
      break;
    }
  }
  // Check above
  if (pos.iRow < kNumRows - 1) {
    const auto upIndexer = [pos](const int i) {
      return Position{pos.iRow + i, pos.iColumn};
    };
    for (const int i : std::views::iota(1, kNumRows - pos.iRow) |
                           std::views::drop_while(emptySquareFunctor(
                               upIndexer, board, intended_move))) {
      const Position checkPos = upIndexer(i);
      const SquareState chPieceFound =
          board.getPieceConsiderMove(checkPos, intended_move);
      if (side != getPieceSide(chPieceFound.value()) &&
          (chPieceFound->mPiece == Piece::kQueen ||
           chPieceFound->mPiece == Piece::kRook)) {
        updateAttack(attack, checkPos, Direction::VERTICAL);
      }
      break;
    }
  }
  // c) Direction: DIAGONAL
  // Check the diagonal up-right
  if (pos.iRow < kNumRows - 1 && pos.iColumn < kNumCols - 1) {
    const auto upRightIndexer = [pos](const int i) {
      return Position{pos.iRow + i, pos.iColumn + i};
    };
    for (const int i : std::views::iota(1, std::min(kNumRows - pos.iRow,
                                                    kNumCols - pos.iColumn)) |
                           std::views::drop_while(emptySquareFunctor(
                               upRightIndexer, board, intended_move))) {
      const Position checkPos = upRightIndexer(i);
      const SquareState chPieceFound =
          board.getPieceConsiderMove(checkPos, intended_move);
      const auto isValidPawn = [chPieceFound, pos,
                                side](const Position checkPos) {
        return (chPieceFound->mPiece == Piece::kPawn) &&
               (checkPos.iRow == pos.iRow + 1) &&
               (checkPos.iColumn == pos.iColumn + 1) && (side == Side::kWhite);
      };
      if (side != getPieceSide(chPieceFound.value()) &&
          (isValidPawn(checkPos) || chPieceFound->mPiece == Piece::kQueen ||
           chPieceFound->mPiece == Piece::kBishop)) {
        updateAttack(attack, checkPos, Direction::DIAGONAL);
      }
      break;
    }
  }
  // Check the diagonal up-left
  if (pos.iRow < kNumRows - 1 && pos.iColumn > 0) {
    const auto upLeftIndexer = [pos](const int i) {
      return Position{pos.iRow + i, pos.iColumn - i};
    };
    for (const int i :
         std::views::iota(1, std::min(kNumRows - pos.iRow, pos.iColumn + 1)) |
             std::views::drop_while(
                 emptySquareFunctor(upLeftIndexer, board, intended_move))) {
      const Position checkPos = upLeftIndexer(i);
      const SquareState chPieceFound =
          board.getPieceConsiderMove(checkPos, intended_move);
      const auto isValidPawn = [chPieceFound, pos,
                                side](const Position checkPos) {
        return (chPieceFound->mPiece == Piece::kPawn) &&
               (checkPos.iRow == pos.iRow + 1) &&
               (checkPos.iColumn == pos.iColumn - 1) && (side == Side::kWhite);
      };
      if (side != getPieceSide(chPieceFound.value()) &&
          (isValidPawn(checkPos) || chPieceFound->mPiece == Piece::kQueen ||
           chPieceFound->mPiece == Piece::kBishop)) {
        updateAttack(attack, checkPos, Direction::DIAGONAL);
      }
      break;
    }
  }
  // Check the diagonal down-right
  if (pos.iRow > 0 && pos.iColumn < kNumCols - 1) {
    const auto downRightIndexer = [pos](const int i) {
      return Position{pos.iRow - i, pos.iColumn + i};
    };
    for (const int i :
         std::views::iota(1, std::min(pos.iRow + 1, kNumCols - pos.iColumn)) |
             std::views::drop_while(
                 emptySquareFunctor(downRightIndexer, board, intended_move))) {
      const Position checkPos = downRightIndexer(i);
      const SquareState chPieceFound =
          board.getPieceConsiderMove(checkPos, intended_move);
      const auto isValidPawn = [chPieceFound, pos,
                                side](const Position checkPos) {
        return (chPieceFound->mPiece == Piece::kPawn) &&
               (checkPos.iRow == pos.iRow - 1) &&
               (checkPos.iColumn == pos.iColumn + 1) && (side == Side::kBlack);
      };
      if (side != getPieceSide(chPieceFound.value()) &&
          (isValidPawn(checkPos) || chPieceFound->mPiece == Piece::kQueen ||
           chPieceFound->mPiece == Piece::kBishop)) {
        updateAttack(attack, checkPos, Direction::DIAGONAL);
      }
      break;
    }
  }
  // Check the diagonal down-left
  if (pos.iRow > 0 && pos.iColumn > 0) {
    const auto downLeftIndexer = [pos](const int i) {
      return Position{pos.iRow - i, pos.iColumn - i};
    };
    for (const int i :
         std::views::iota(1, std::min(pos.iRow + 1, pos.iColumn + 1)) |
             std::views::drop_while(
                 emptySquareFunctor(downLeftIndexer, board, intended_move))) {
      const Position checkPos = downLeftIndexer(i);
      const SquareState chPieceFound =
          board.getPieceConsiderMove(checkPos, intended_move);
      const auto isValidPawn = [chPieceFound, pos,
                                side](const Position checkPos) {
        return (chPieceFound->mPiece == Piece::kPawn) &&
               (checkPos.iRow == pos.iRow - 1) &&
               (checkPos.iColumn == pos.iColumn - 1) && (side == Side::kBlack);
      };
      if (side != getPieceSide(chPieceFound.value()) &&
          (isValidPawn(checkPos) || chPieceFound->mPiece == Piece::kQueen ||
           chPieceFound->mPiece == Piece::kBishop)) {
        updateAttack(attack, checkPos, Direction::DIAGONAL);
      }
      break;
    }
  }
  // d) Direction: L_SHAPED
  // Check if the piece is put in jeopardy by a knight
  const auto lShapedIndexer = [pos](const int i) {
    return Position{pos.iRow + knight_moves[i].iRow,
                    pos.iColumn + knight_moves[i].iColumn};
  };
  for (const int i :
       std::views::iota(0, 8) | std::views::filter(occupiedSquareFunctor(
                                    lShapedIndexer, board, intended_move))) {
    const Position checkPos = lShapedIndexer(i);
    const SquareState chPieceFound =
        board.getPieceConsiderMove(checkPos, intended_move);
    if (side != getPieceSide(chPieceFound.value()) &&
        chPieceFound->mPiece == Piece::kKnight) {
      updateAttack(attack, checkPos, Direction::L_SHAPE);
    }
  }
  return attack;
}
} // namespace chess

#if defined(UNIT_TEST)

#include <catch2/catch_test_macros.hpp>

TEST_CASE("logic uderAttack initialBoard") {
  const chess::Board board;
  // Not under attack
  {
    CHECK(!underAttack(chess::Position{0, 0}, chess::Side::kWhite, board)
               .bUnderAttack);
    CHECK(!underAttack(chess::Position{1, 0}, chess::Side::kWhite, board)
               .bUnderAttack);
    CHECK(!underAttack(chess::Position{2, 0}, chess::Side::kWhite, board)
               .bUnderAttack);
  }
  // Under attack by white pawn and knight
  {
    const chess::UnderAttack under_attack =
        chess::underAttack(chess::Position{2, 0}, chess::Side::kBlack, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 2);
    CHECK(under_attack.attacker[0].pos == chess::Position{1, 1});
    CHECK(under_attack.attacker[0].dir == chess::Direction::DIAGONAL);
    CHECK(under_attack.attacker[1].pos == chess::Position{0, 1});
    CHECK(under_attack.attacker[1].dir == chess::Direction::L_SHAPE);
  }
}

TEST_CASE("logic underAttack queen horizontal edges") {
  using namespace chess::pieces;

  // clang-format off
  constexpr std::array<chess::SquareState, chess::kNumPositions> initial_board{
      E, E, E, Q, E, E, E, E, 
      E, E, E, E, E, E, E, E, 
      E, E, E, E, E, E, E, E, 
      E, E, E, E, E, E, E, E, 
      E, E, E, E, E, E, E, E, 
      E, E, E, E, E, E, E, E, 
      E, E, E, E, E, E, E, E, 
      E, E, E, q, E, E, E, E};
  // clang-format on

  const auto board = chess::Board{std::move(initial_board)};
  SECTION("Horizontal to left") {
    const chess::UnderAttack under_attack =
        chess::underAttack(chess::Position{0, 0}, chess::Side::kBlack, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 1);
    CHECK(under_attack.attacker[0].pos == chess::Position{0, 3});
    CHECK(under_attack.attacker[0].dir == chess::Direction::HORIZONTAL);
  }
  SECTION("Horizontal to right") {
    const chess::UnderAttack under_attack =
        chess::underAttack(chess::Position{0, 4}, chess::Side::kBlack, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 1);
    CHECK(under_attack.attacker[0].pos == chess::Position{0, 3});
    CHECK(under_attack.attacker[0].dir == chess::Direction::HORIZONTAL);
  }
  SECTION("Vertical above") {
    const chess::UnderAttack under_attack =
        chess::underAttack(chess::Position{3, 3}, chess::Side::kBlack, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 1);
    CHECK(under_attack.attacker[0].pos == chess::Position{0, 3});
    CHECK(under_attack.attacker[0].dir == chess::Direction::VERTICAL);
  }
  SECTION("Vertical below") {
    const chess::UnderAttack under_attack =
        chess::underAttack(chess::Position{3, 3}, chess::Side::kWhite, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 1);
    CHECK(under_attack.attacker[0].pos == chess::Position{7, 3});
    CHECK(under_attack.attacker[0].dir == chess::Direction::VERTICAL);
  }
}

TEST_CASE("logic underAttack queen vertical edges") {
  using namespace chess::pieces;

  // clang-format off
  constexpr std::array<chess::SquareState, chess::kNumPositions> initial_board{
      Q, E, E, E, E, E, E, E, 
      E, E, E, E, E, E, E, E, 
      E, E, E, E, E, E, E, E, 
      E, E, E, E, E, E, E, E, 
      E, E, E, E, E, E, E, q, 
      E, E, E, E, E, E, E, E, 
      E, E, E, E, E, E, E, E, 
      E, E, E, E, E, E, E, E};
  // clang-format on

  const auto board = chess::Board{std::move(initial_board)};
  SECTION("Horizontal to left") {
    const chess::UnderAttack under_attack =
        chess::underAttack(chess::Position{4, 3}, chess::Side::kWhite, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 1);
    CHECK(under_attack.attacker[0].pos == chess::Position{4, 7});
    CHECK(under_attack.attacker[0].dir == chess::Direction::HORIZONTAL);
  }
  SECTION("Horizontal to right") {
    const chess::UnderAttack under_attack =
        chess::underAttack(chess::Position{0, 4}, chess::Side::kBlack, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 1);
    CHECK(under_attack.attacker[0].pos == chess::Position{0, 0});
    CHECK(under_attack.attacker[0].dir == chess::Direction::HORIZONTAL);
  }
  SECTION("Vertical above") {
    const chess::UnderAttack under_attack =
        chess::underAttack(chess::Position{0, 7}, chess::Side::kWhite, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 1);
    CHECK(under_attack.attacker[0].pos == chess::Position{4, 7});
    CHECK(under_attack.attacker[0].dir == chess::Direction::VERTICAL);
  }
  SECTION("Vertical below") {
    const chess::UnderAttack under_attack =
        chess::underAttack(chess::Position{3, 0}, chess::Side::kBlack, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 1);
    CHECK(under_attack.attacker[0].pos == chess::Position{0, 0});
    CHECK(under_attack.attacker[0].dir == chess::Direction::VERTICAL);
  }
}

TEST_CASE("logic underAttack white pawn on edge") {
  using namespace chess::pieces;
  // clang-format off
  constexpr std::array<chess::SquareState, chess::kNumPositions> initial_board{
      p, E, E, E, p, E, E, p, 
      E, E, E, E, E, E, E, E, 
      E, E, E, E, E, E, E, E, 
      E, E, p, E, E, E, E, p, 
      p, E, E, E, E, E, E, E, 
      E, E, E, E, E, E, E, E, 
      E, E, E, E, E, E, E, E, 
      p, E, E, p, E, E, E, p};
  // clang-format on

  const auto board = chess::Board{std::move(initial_board)};

  SECTION("Expected attacks") {
    chess::UnderAttack under_attack =
        chess::underAttack(chess::Position{3, 1}, chess::Side::kWhite, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 1);
    CHECK(under_attack.attacker[0].pos == chess::Position{4, 0});
    CHECK(under_attack.attacker[0].dir == chess::Direction::DIAGONAL);

    under_attack =
        chess::underAttack(chess::Position{2, 1}, chess::Side::kWhite, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 1);
    CHECK(under_attack.attacker[0].pos == chess::Position{3, 2});
    CHECK(under_attack.attacker[0].dir == chess::Direction::DIAGONAL);

    under_attack =
        chess::underAttack(chess::Position{2, 3}, chess::Side::kWhite, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 1);
    CHECK(under_attack.attacker[0].pos == chess::Position{3, 2});
    CHECK(under_attack.attacker[0].dir == chess::Direction::DIAGONAL);

    under_attack =
        chess::underAttack(chess::Position{2, 6}, chess::Side::kWhite, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 1);
    CHECK(under_attack.attacker[0].pos == chess::Position{3, 7});
    CHECK(under_attack.attacker[0].dir == chess::Direction::DIAGONAL);

    under_attack =
        chess::underAttack(chess::Position{6, 1}, chess::Side::kWhite, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 1);
    CHECK(under_attack.attacker[0].pos == chess::Position{7, 0});
    CHECK(under_attack.attacker[0].dir == chess::Direction::DIAGONAL);

    under_attack =
        chess::underAttack(chess::Position{6, 2}, chess::Side::kWhite, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 1);
    CHECK(under_attack.attacker[0].pos == chess::Position{7, 3});
    CHECK(under_attack.attacker[0].dir == chess::Direction::DIAGONAL);

    under_attack =
        chess::underAttack(chess::Position{6, 4}, chess::Side::kWhite, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 1);
    CHECK(under_attack.attacker[0].pos == chess::Position{7, 3});
    CHECK(under_attack.attacker[0].dir == chess::Direction::DIAGONAL);

    under_attack =
        chess::underAttack(chess::Position{6, 6}, chess::Side::kWhite, board);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 1);
    CHECK(under_attack.attacker[0].pos == chess::Position{7, 7});
    CHECK(under_attack.attacker[0].dir == chess::Direction::DIAGONAL);
  }
}

TEST_CASE("logic underAttack black pawn on edge") {
  using namespace chess::pieces;
  // clang-format off
  constexpr std::array<chess::SquareState, chess::kNumPositions> initial_board{
      P, E, E, E, P, E, E, P,
      E, E, E, E, E, E, E, E, 
      E, E, E, E, E, E, E, E, 
      E, E, E, E, E, E, E, P, 
      P, E, E, E, E, E, E, E, 
      E, E, P, E, E, E, E, E, 
      E, E, E, E, E, E, E, E, 
      P, E, E, P, E, E, E, P};
  // clang-format on

  const auto board = chess::Board{std::move(initial_board)};

  SECTION("Expected attacks") {
    chess::UnderAttack under_attack =
        chess::underAttack(chess::Position{5, 1}, chess::Side::kBlack, board);
    auto expected = chess::UnderAttack{
        .bUnderAttack = true,
        .iNumAttackers = 1,
        .attacker = {
            chess::Attacker{.pos = {4, 0}, .dir = chess::Direction::DIAGONAL}}};
    CHECK(under_attack == expected);

    under_attack =
        chess::underAttack(chess::Position{1, 1}, chess::Side::kBlack, board);
    expected = chess::UnderAttack{
        .bUnderAttack = true,
        .iNumAttackers = 1,
        .attacker = {
            chess::Attacker{.pos = {0, 0}, .dir = chess::Direction::DIAGONAL}}};
    CHECK(under_attack == expected);

    under_attack =
        chess::underAttack(chess::Position{1, 3}, chess::Side::kBlack, board);
    expected = chess::UnderAttack{
        .bUnderAttack = true,
        .iNumAttackers = 1,
        .attacker = {
            chess::Attacker{.pos = {0, 4}, .dir = chess::Direction::DIAGONAL}}};
    CHECK(under_attack == expected);

    under_attack =
        chess::underAttack(chess::Position{1, 5}, chess::Side::kBlack, board);
    expected = chess::UnderAttack{
        .bUnderAttack = true,
        .iNumAttackers = 1,
        .attacker = {
            chess::Attacker{.pos = {0, 4}, .dir = chess::Direction::DIAGONAL}}};
    CHECK(under_attack == expected);

    under_attack =
        chess::underAttack(chess::Position{1, 6}, chess::Side::kBlack, board);
    expected = chess::UnderAttack{
        .bUnderAttack = true,
        .iNumAttackers = 1,
        .attacker = {
            chess::Attacker{.pos = {0, 7}, .dir = chess::Direction::DIAGONAL}}};
    CHECK(under_attack == expected);

    under_attack =
        chess::underAttack(chess::Position{4, 6}, chess::Side::kBlack, board);
    expected = chess::UnderAttack{
        .bUnderAttack = true,
        .iNumAttackers = 1,
        .attacker = {
            chess::Attacker{.pos = {3, 7}, .dir = chess::Direction::DIAGONAL}}};
    CHECK(under_attack == expected);

    under_attack =
        chess::underAttack(chess::Position{6, 1}, chess::Side::kBlack, board);
    expected = chess::UnderAttack{
        .bUnderAttack = true,
        .iNumAttackers = 1,
        .attacker = {
            chess::Attacker{.pos = {5, 2}, .dir = chess::Direction::DIAGONAL}}};
    CHECK(under_attack == expected);

    under_attack =
        chess::underAttack(chess::Position{6, 3}, chess::Side::kBlack, board);
    expected = chess::UnderAttack{
        .bUnderAttack = true,
        .iNumAttackers = 1,
        .attacker = {
            chess::Attacker{.pos = {5, 2}, .dir = chess::Direction::DIAGONAL}}};
    CHECK(under_attack == expected);
  }
}

#endif
