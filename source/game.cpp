#include "game.h"
#include "logic.h"
#include "user_interface.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <ranges>

namespace chess {
namespace {
constexpr std::array<Position, 8> knight_moves = {
    Position{1, -2},  Position{2, -1},  Position{2, 1},  Position{1, 2},
    Position{-1, -2}, Position{-2, -1}, Position{-2, 1}, Position{-1, 2}};

bool isValidPosition(const Position pos) {
  return pos.iColumn >= 0 && pos.iColumn < kNumCols && pos.iRow >= 0 &&
         pos.iRow < kNumRows;
}

int charToRow(const char row) { return row - '1'; }

int charToColumn(const char col) { return col - 'A'; }

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
} // namespace

std::pair<Position, Position> parseMove(const std::string &move) {
  const Position from{.iRow = charToRow(move[1]),
                      .iColumn = charToColumn(move[0])};
  const Position to{.iRow = charToRow(move[4]),
                    .iColumn = charToColumn(move[3])};
  return {from, to};
}

void Game::movePiece(Position present, Position future, EnPassant &S_enPassant,
                     Castling &S_castling, Promotion &S_promotion) {
  // Get the piece to be moved
  const SquareState chPiece = getPieceAtPosition(present);
  assert(chPiece);

  // Is the destination square occupied?
  const SquareState chCapturedPiece = getPieceAtPosition(future);

  // So, was a piece captured in this move?
  if (chCapturedPiece) {
    capturePiece(*chCapturedPiece);
    // Set Undo structure. If a piece was captured, then no "en passant" move
    // performed
    m_undo.bCapturedLastMove = true;
    m_undo.en_passant = std::nullopt;
  } else if (S_enPassant.bApplied) {
    const SquareState chCapturedEP =
        getPieceAtPosition(S_enPassant.PawnCaptured);
    capturePiece(*chCapturedEP);

    // Now, remove the captured pawn
    board(S_enPassant.PawnCaptured.iRow, S_enPassant.PawnCaptured.iColumn) =
        pieces::E;

    // Set Undo structure as piece was captured and "en passant" move was
    // performed
    m_undo.bCapturedLastMove = true;
    m_undo.en_passant = S_enPassant;
  } else {
    m_undo.bCapturedLastMove = false;
    m_undo.en_passant = std::nullopt;
  }

  // Remove piece from present position
  board(present.iRow, present.iColumn) = pieces::E;

  // Move piece to new position
  if (S_promotion.bApplied) {
    board(future.iRow, future.iColumn) = S_promotion.chAfter;
    // Set Undo structure as a promotion occured
    m_undo.promotion = S_promotion;
  } else {
    board(future.iRow, future.iColumn) = chPiece;
    // Reset m_undo.promotion
    m_undo.promotion = std::nullopt;
  }

  // Was it a castling move?
  if (S_castling.bApplied) {
    // The king was already move, but we still have to move the rook to 'jump'
    // the king
    const SquareState chPiece = getPieceAtPosition(S_castling.rook_before);

    // Remove the rook from present position
    board(S_castling.rook_before) = pieces::E;

    // 'Jump' into to new position
    board(S_castling.rook_after) = chPiece;

    // Write this information to the m_undo struct
    m_undo.castling = S_castling;

    // Save the 'CastlingAllowed' information in case the move is undone
    m_undo.bCastlingKingSideAllowed =
        m_bCastlingKingSideAllowed[getCurrentTurn()];
    m_undo.bCastlingQueenSideAllowed =
        m_bCastlingQueenSideAllowed[getCurrentTurn()];
  } else {
    // Reset m_undo.castling
    m_undo.castling = std::nullopt;
  }

  // Castling requirements
  if (chPiece->mPiece == Piece::kKing) {
    // After the king has moved once, no more castling allowed
    m_bCastlingKingSideAllowed[getCurrentTurn()] = false;
    m_bCastlingQueenSideAllowed[getCurrentTurn()] = false;
  } else if (chPiece->mPiece == Piece::kRook) {
    // If the rook moved from column 'A', no more castling allowed on the queen
    // side
    if (0 == present.iColumn) {
      m_bCastlingQueenSideAllowed[getCurrentTurn()] = false;
    }

    // If the rook moved from column 'A', no more castling allowed on the queen
    // side
    else if (7 == present.iColumn) {
      m_bCastlingKingSideAllowed[getCurrentTurn()] = false;
    }
  }

  // Change turns
  changeTurns();

  // This move can be undone
  m_undo.bCanUndo = true;
}

void Game::undoLastMove() {
  const std::string last_move = getLastMove();

  // Parse the line
  const auto [from, to] = parseMove(last_move);

  // Since we want to undo a move, we will be moving the piece from (iToRow,
  // iToColumn) to (iFromRow, iFromColumn)
  const SquareState chPiece = getPieceAtPosition(to);

  // Moving it back
  // If there was a castling
  if (m_undo.promotion && m_undo.promotion->bApplied) {
    board(from.iRow, from.iColumn) = m_undo.promotion->chBefore;
  } else {
    board(from.iRow, from.iColumn) = chPiece;
  }

  // Change turns
  changeTurns();

  // If a piece was captured, move it back to the board
  if (m_undo.bCapturedLastMove) {
    // Let's retrieve the last captured piece
    PieceWithSide chCaptured;
    // Since we already changed turns back, it means we should we pop a piece
    // from the oponents vector
    if (Side::kWhite == m_CurrentTurn) {
      chCaptured = black_captured.back();
      black_captured.pop_back();
    } else {
      chCaptured = white_captured.back();
      white_captured.pop_back();
    }

    // Move the captured piece back. Was this an "en passant" move?
    if (m_undo.en_passant && m_undo.en_passant->bApplied) {
      // Move the captured piece back
      board(m_undo.en_passant->PawnCaptured.iRow,
            m_undo.en_passant->PawnCaptured.iColumn) = chCaptured;
      // Remove the attacker
      board(to.iRow, to.iColumn) = pieces::E;
    } else {
      board(to.iRow, to.iColumn) = chCaptured;
    }
  } else {
    board(to.iRow, to.iColumn) = pieces::E;
  }

  // If there was a castling
  if (m_undo.castling && m_undo.castling->bApplied) {
    const SquareState chRook = getPieceAtPosition(m_undo.castling->rook_after);
    // Remove the rook from present position
    board(m_undo.castling->rook_after) = pieces::E;
    // 'Jump' into to new position
    board(m_undo.castling->rook_before) = chRook;
    // Restore the values of castling allowed or not
    m_bCastlingKingSideAllowed[getCurrentTurn()] =
        m_undo.bCastlingKingSideAllowed;
    m_bCastlingQueenSideAllowed[getCurrentTurn()] =
        m_undo.bCastlingQueenSideAllowed;
  }

  // Clean m_undo struct
  m_undo.bCanUndo = false;
  m_undo.bCapturedLastMove = false;
  m_undo.en_passant = std::nullopt;
  m_undo.castling = std::nullopt;
  m_undo.promotion = std::nullopt;

  // If it was a checkmate, toggle back to game not finished
  m_bGameFinished = false;

  // Finally, remove the last move from the list
  deleteLastMove();
}

bool Game::undoIsPossible() const { return m_undo.bCanUndo; }

bool Game::castlingAllowed(const BoardSide iSide, const Side iColor) const {
  if (BoardSide::QUEEN_SIDE == iSide) {
    return m_bCastlingQueenSideAllowed.at(iColor);
  } else // if ( KING_SIDE == iSide )
  {
    return m_bCastlingKingSideAllowed.at(iColor);
  }
}

SquareState Game::getPieceAtPosition(const Position pos) const {
  return board(pos.iRow, pos.iColumn);
}

SquareState Game::getPieceConsiderMove(
    const Position pos,
    const std::optional<IntendedMove> &intended_move) const {
  return board.getPieceConsiderMove(pos, intended_move);
}

UnderAttack
Game::isUnderAttack(const Position pos, const Side iColor,
                    const std::optional<IntendedMove> &intended_move) const {
  return underAttack(pos, iColor, board, intended_move);
}

bool Game::isReachable(const Position pos, const Side iColor) const {
  bool bReachable = false;

  // a) Direction: HORIZONTAL
  // Check all the way to the right
  for (int i = pos.iColumn + 1; i < 8; i++) {
    if (const SquareState chPieceFound = getPieceAtPosition({pos.iRow, i})) {
      if (iColor == getPieceColor(*chPieceFound)) {
        // This is a piece of the same color
        break;
      } else if ((chPieceFound->mPiece == Piece::kQueen) ||
                 (chPieceFound->mPiece == Piece::kRook)) {
        // There is a queen or a rook to the right, so the square is reachable
        bReachable = true;
        break;
      } else {
        // There is a piece that does not move horizontally
        break;
      }
    }
  }

  // Check all the way to the left
  for (int i = pos.iColumn - 1; i >= 0; i--) {
    if (const SquareState chPieceFound = getPieceAtPosition({pos.iRow, i})) {
      if (iColor == getPieceColor(*chPieceFound)) {
        // This is a piece of the same color
        break;
      } else if ((chPieceFound->mPiece == Piece::kQueen) ||
                 (chPieceFound->mPiece == Piece::kRook)) {
        // There is a queen or a rook to the left, so the square is reachable
        bReachable = true;
        break;
      } else {
        // There is a piece that does not move horizontally
        break;
      }
    }
  }

  // b) Direction: VERTICAL
  // Check all the way up
  for (int i = pos.iRow + 1; i < 8; i++) {
    if (const SquareState chPieceFound = getPieceAtPosition({pos.iRow, i})) {
      if (iColor == getPieceColor(*chPieceFound)) {
        // This is a piece of the same color
        break;
      } else if ((chPieceFound->mPiece == Piece::kPawn) &&
                 (getPieceColor(*chPieceFound) == Side::kBlack) &&
                 (i == pos.iRow + 1)) {
        // There is a pawn one square up, so the square is reachable
        bReachable = true;
        break;
      } else if ((chPieceFound->mPiece == Piece::kQueen) ||
                 (chPieceFound->mPiece == Piece::kRook)) {
        // There is a queen or a rook on the way up, so the square is reachable
        bReachable = true;
        break;
      } else {
        // There is a piece that does not move vertically
        break;
      }
    }
  }

  // Check all the way down
  for (int i = pos.iRow - 1; i >= 0; i--) {
    if (const SquareState chPieceFound = getPieceAtPosition({pos.iRow, i})) {
      if (iColor == getPieceColor(*chPieceFound)) {
        // This is a piece of the same color
        break;
      } else if ((chPieceFound->mPiece == Piece::kPawn) &&
                 (getPieceColor(*chPieceFound) == Side::kWhite) &&
                 (i == pos.iRow - 1)) {
        // There is a pawn one square down, so the square is reachable
        bReachable = true;
        break;
      } else if ((chPieceFound->mPiece == Piece::kQueen) ||
                 (chPieceFound->mPiece == Piece::kRook)) {
        // There is a queen or a rook on the way down, so the square is
        // reachable
        bReachable = true;
        break;
      } else {
        // There is a piece that does not move vertically
        break;
      }
    }
  }

  // c) Direction: DIAGONAL
  {
    // Check the diagonal up-right
    for (int i = pos.iRow + 1, j = pos.iColumn + 1; i < 8 && j < 8; i++, j++) {
      const SquareState chPieceFound = getPieceAtPosition({i, j});
      if (!chPieceFound) {
        // This square is empty, move on
        continue;
      }

      if (iColor == getPieceColor(*chPieceFound)) {
        // This is a piece of the same color
        break;
      } else if ((chPieceFound->mPiece == Piece::kQueen) ||
                 (chPieceFound->mPiece == Piece::kBishop)) {
        // There is a queen or a bishop in that direction, so the square is
        // reachable
        bReachable = true;
        break;
      } else {
        // There is a piece that does not move diagonally
        break;
      }
    }

    // Check the diagonal up-left
    for (int i = pos.iRow + 1, j = pos.iColumn - 1; i < 8 && j > 0; i++, j--) {
      const SquareState chPieceFound = getPieceAtPosition({i, j});
      if (!chPieceFound) {
        // This square is empty, move on
        continue;
      }

      if (iColor == getPieceColor(*chPieceFound)) {
        // This is a piece of the same color
        break;
      } else if ((chPieceFound->mPiece == Piece::kQueen) ||
                 (chPieceFound->mPiece == Piece::kBishop)) {
        // There is a queen or a bishop in that direction, so the square is
        // reachable
        bReachable = true;
        break;
      } else {
        // There is a piece that does not move diagonally
        break;
      }
    }

    // Check the diagonal down-right
    for (int i = pos.iRow - 1, j = pos.iColumn + 1; i > 0 && j < 8; i--, j++) {
      const SquareState chPieceFound = getPieceAtPosition({i, j});
      if (!chPieceFound) {
        // This square is empty, move on
        continue;
      }

      if (iColor == getPieceColor(*chPieceFound)) {
        // This is a piece of the same color
        break;
      } else if ((chPieceFound->mPiece == Piece::kQueen) ||
                 (chPieceFound->mPiece == Piece::kBishop)) {
        // There is a queen or a bishop in that direction, so the square is
        // reachable
        bReachable = true;
        break;
      } else {
        // There is a piece that does not move diagonally
        break;
      }
    }

    // Check the diagonal down-left
    for (int i = pos.iRow - 1, j = pos.iColumn - 1; i > 0 && j > 0; i--, j--) {
      const SquareState chPieceFound = getPieceAtPosition({i, j});
      if (!chPieceFound) {
        // This square is empty, move on
        continue;
      }

      if (iColor == getPieceColor(*chPieceFound)) {
        // This is a piece of the same color
        break;
      } else if ((chPieceFound->mPiece == Piece::kQueen) ||
                 (chPieceFound->mPiece == Piece::kBishop)) {
        // There is a queen or a bishop in that direction, so the square is
        // reachable
        bReachable = true;
        break;
      } else {
        // There is a piece that does not move diagonally
        break;
      }
    }
  }

  // d) Direction: L_SHAPED
  {
    // Check if the piece is put in jeopardy by a knigh

    for (int i = 0; i < 8; i++) {
      const Position posToTest{pos.iRow + knight_moves[i].iRow,
                               pos.iColumn + knight_moves[i].iColumn};

      if (!isValidPosition(posToTest)) {
        // This square does not even exist, so no need to test
        continue;
      }
      const SquareState chPieceFound = getPieceAtPosition(posToTest);
      if (!chPieceFound || iColor == getPieceColor(*chPieceFound)) {
        // This square is empty
        // or this is a piece of the same color
        continue;
      } else if (chPieceFound->mPiece == Piece::kKnight) {
        bReachable = true;
        break;
      }
    }
  }

  return bReachable;
}

bool Game::isSquareOccupied(const Position pos) const {
  return getPieceAtPosition(pos).has_value();
}

bool Game::isPathFree(const Position startingPos, const Position finishingPos,
                      const Direction iDirection) const {
  bool bFree = false;

  switch (iDirection) {
  case Direction::HORIZONTAL: {
    // If it is a horizontal move, we can assume the startingPos.iRow ==
    // finishingPos.iRow If the piece wants to move from column 0 to column 7,
    // we must check if columns 1-6 are free
    if (startingPos.iColumn == finishingPos.iColumn) {
      std::cout << "Error. Movement is horizontal but column is the same\n";
    }

    // Moving to the right
    else if (startingPos.iColumn < finishingPos.iColumn) {
      // Settting bFree as initially true, only inside the cases, guarantees
      // that the path is checked
      bFree = true;

      for (int i = startingPos.iColumn + 1; i < finishingPos.iColumn; i++) {
        if (isSquareOccupied({startingPos.iRow, i})) {
          bFree = false;
          std::cout << "Horizontal path to the right is not clear!\n";
        }
      }
    }

    // Moving to the left
    else // if (startingPos.iColumn > finishingPos.iColumn)
    {
      // Settting bFree as initially true, only inside the cases, guarantees
      // that the path is checked
      bFree = true;

      for (int i = startingPos.iColumn - 1; i > finishingPos.iColumn; i--) {
        if (isSquareOccupied({startingPos.iRow, i})) {
          bFree = false;
          std::cout << "Horizontal path to the left is not clear!\n";
        }
      }
    }
  } break;

  case Direction::VERTICAL: {
    // If it is a vertical move, we can assume the startingPos.iColumn ==
    // finishingPos.iColumn If the piece wants to move from column 0 to column
    // 7, we must check if columns 1-6 are free
    if (startingPos.iRow == finishingPos.iRow) {
      std::cout << "Error. Movement is vertical but row is the same\n";
      throw("Error. Movement is vertical but row is the same");
    }

    // Moving up
    else if (startingPos.iRow < finishingPos.iRow) {
      // Settting bFree as initially true, only inside the cases, guarantees
      // that the path is checked
      bFree = true;

      for (int i = startingPos.iRow + 1; i < finishingPos.iRow; i++) {
        if (isSquareOccupied({i, startingPos.iColumn})) {
          bFree = false;
          std::cout << "Vertical path up is not clear!\n";
        }
      }
    }

    // Moving down
    else // if (startingPos.iColumn > finishingPos.iRow)
    {
      // Settting bFree as initially true, only inside the cases, guarantees
      // that the path is checked
      bFree = true;

      for (int i = startingPos.iRow - 1; i > finishingPos.iRow; i--) {
        if (isSquareOccupied({i, startingPos.iColumn})) {
          bFree = false;
          std::cout << "Vertical path down is not clear!\n";
        }
      }
    }
  } break;

  case Direction::DIAGONAL: {
    // Moving up and right
    if ((finishingPos.iRow > startingPos.iRow) &&
        (finishingPos.iColumn > startingPos.iColumn)) {
      // Settting bFree as initially true, only inside the cases, guarantees
      // that the path is checked
      bFree = true;

      for (int i = 1; i < abs(finishingPos.iRow - startingPos.iRow); i++) {
        if (isSquareOccupied({startingPos.iRow + i, startingPos.iColumn + i})) {
          bFree = false;
          std::cout << "Diagonal path up-right is not clear!\n";
        }
      }
    }

    // Moving up and left
    else if ((finishingPos.iRow > startingPos.iRow) &&
             (finishingPos.iColumn < startingPos.iColumn)) {
      // Settting bFree as initially true, only inside the cases, guarantees
      // that the path is checked
      bFree = true;

      for (int i = 1; i < abs(finishingPos.iRow - startingPos.iRow); i++) {
        if (isSquareOccupied({startingPos.iRow + i, startingPos.iColumn - i})) {
          bFree = false;
          std::cout << "Diagonal path up-left is not clear!\n";
        }
      }
    }

    // Moving down and right
    else if ((finishingPos.iRow < startingPos.iRow) &&
             (finishingPos.iColumn > startingPos.iColumn)) {
      // Settting bFree as initially true, only inside the cases, guarantees
      // that the path is checked
      bFree = true;

      for (int i = 1; i < abs(finishingPos.iRow - startingPos.iRow); i++) {
        if (isSquareOccupied({startingPos.iRow - i, startingPos.iColumn + i})) {
          bFree = false;
          std::cout << "Diagonal path down-right is not clear!\n";
        }
      }
    }

    // Moving down and left
    else if ((finishingPos.iRow < startingPos.iRow) &&
             (finishingPos.iColumn < startingPos.iColumn)) {
      // Settting bFree as initially true, only inside the cases, guarantees
      // that the path is checked
      bFree = true;

      for (int i = 1; i < abs(finishingPos.iRow - startingPos.iRow); i++) {
        if (isSquareOccupied({startingPos.iRow - i, startingPos.iColumn - i})) {
          bFree = false;
          std::cout << "Diagonal path down-left is not clear!\n";
        }
      }
    }

    else {
      throw("Error. Diagonal move not allowed");
    }
  } break;
  }

  return bFree;
}

bool Game::canBeBlocked(Position startingPos, Position finishingPos,
                        const Direction iDirection) const {
  bool bBlocked = false;

  switch (iDirection) {
  case Direction::HORIZONTAL: {
    // If it is a horizontal move, we can assume the startingPos.iRow ==
    // finishingPos.iRow If the piece wants to move from column 0 to column 7,
    // we must check if columns 1-6 are free
    if (startingPos.iColumn == finishingPos.iColumn) {
      std::cout << "Error. Movement is horizontal but column is the same\n";
    }

    // Moving to the right
    else if (startingPos.iColumn < finishingPos.iColumn) {
      for (int i = startingPos.iColumn + 1; i < finishingPos.iColumn; i++) {
        if (isReachable({startingPos.iRow, i}, getOpponentColor())) {
          // Some piece can block the way
          bBlocked = true;
        }
      }
    }

    // Moving to the left
    else // if (startingPos.iColumn > finishingPos.iColumn)
    {
      for (int i = startingPos.iColumn - 1; i > finishingPos.iColumn; i--) {
        if (isReachable({startingPos.iRow, i}, getOpponentColor())) {
          // Some piece can block the way
          bBlocked = true;
        }
      }
    }
  } break;

  case Direction::VERTICAL: {
    // If it is a vertical move, we can assume the startingPos.iColumn ==
    // finishingPos.iColumn If the piece wants to move from column 0 to column
    // 7, we must check if columns 1-6 are free
    if (startingPos.iRow == finishingPos.iRow) {
      std::cout << "Error. Movement is vertical but row is the same\n";
      throw("Error. Movement is vertical but row is the same");
    }

    // Moving up
    else if (startingPos.iRow < finishingPos.iRow) {
      for (int i = startingPos.iRow + 1; i < finishingPos.iRow; i++) {
        if (isReachable({i, startingPos.iColumn}, getOpponentColor())) {
          // Some piece can block the way
          bBlocked = true;
        }
      }
    }

    // Moving down
    else // if (startingPos.iColumn > finishingPos.iRow)
    {
      for (int i = startingPos.iRow - 1; i > finishingPos.iRow; i--) {
        if (isReachable({i, startingPos.iColumn}, getOpponentColor())) {
          // Some piece can block the way
          bBlocked = true;
        }
      }
    }
  } break;

  case Direction::DIAGONAL: {
    // Moving up and right
    if ((finishingPos.iRow > startingPos.iRow) &&
        (finishingPos.iColumn > startingPos.iColumn)) {
      for (int i = 1; i < abs(finishingPos.iRow - startingPos.iRow); i++) {
        if (isReachable({startingPos.iRow + i, startingPos.iColumn + i},
                        getOpponentColor())) {
          // Some piece can block the way
          bBlocked = true;
        }
      }
    }

    // Moving up and left
    else if ((finishingPos.iRow > startingPos.iRow) &&
             (finishingPos.iColumn < startingPos.iColumn)) {
      for (int i = 1; i < abs(finishingPos.iRow - startingPos.iRow); i++) {
        if (isReachable({startingPos.iRow + i, startingPos.iColumn - i},
                        getOpponentColor())) {
          // Some piece can block the way
          bBlocked = true;
        }
      }
    }

    // Moving down and right
    else if ((finishingPos.iRow < startingPos.iRow) &&
             (finishingPos.iColumn > startingPos.iColumn)) {
      for (int i = 1; i < abs(finishingPos.iRow - startingPos.iRow); i++) {
        if (isReachable({startingPos.iRow - i, startingPos.iColumn + i},
                        getOpponentColor())) {
          // Some piece can block the way
          bBlocked = true;
        }
      }
    }

    // Moving down and left
    else if ((finishingPos.iRow < startingPos.iRow) &&
             (finishingPos.iColumn < startingPos.iColumn)) {
      for (int i = 1; i < abs(finishingPos.iRow - startingPos.iRow); i++) {
        if (isReachable({startingPos.iRow - i, startingPos.iColumn - i},
                        getOpponentColor())) {
          // Some piece can block the way
          bBlocked = true;
        }
      }
    }

    else {
      std::cout << "Error. Diagonal move not allowed\n";
      throw("Error. Diagonal move not allowed");
    }
  } break;
  }

  return bBlocked;
}

bool Game::isCheckMate() {
  bool bCheckmate = false;

  // 1. First of all, is the king in check?
  if (!playerKingInCheck()) {
    return false;
  }

  // 2. Can the king move to another square?
  constexpr std::array<Position, 8> king_moves{
      Position{1, -1}, Position{1, 0},  Position{1, 1},   Position{0, 1},
      Position{-1, 1}, Position{-1, 0}, Position{-1, -1}, Position{0, -1}};

  const Position king = findKing(getCurrentTurn());

  for (int i = 0; i < 8; i++) {
    const Position posToTest{king.iRow + king_moves[i].iRow,
                             king.iColumn + king_moves[i].iColumn};

    if (!isValidPosition(posToTest)) {
      // This square does not even exist, so no need to test
      continue;
    }

    if (const SquareState square = getPieceAtPosition(posToTest);
        square->mSide == m_CurrentTurn) {
      // That square is not empty, so no need to test
      // TODO: what if it's an oponent piece?
      continue;
    }

    const IntendedMove intended_move{.chPiece =
                                         getPieceAtPosition(king).value(),
                                     .from = king,
                                     .to = posToTest};

    // Now, for every possible move of the king, check if it would be in
    // jeopardy Since the move has already been made,
    // current_game->getCurrentTurn() now will return the next player's color.
    // And it is in fact this king that we want to check for
    const UnderAttack king_moved =
        isUnderAttack(posToTest, getCurrentTurn(), intended_move);

    if (!king_moved.bUnderAttack) {
      // This means there is at least one position when the king would not be in
      // jeopardy, so that's not a checkmate
      return false;
    }
  }

  // 3. Can the attacker be taken or another piece get into the way?
  const UnderAttack king_attacked = isUnderAttack(king, getCurrentTurn());
  if (king_attacked.iNumAttackers == 1) {
    // Can the attacker be taken?
    const UnderAttack king_attacker =
        isUnderAttack(king_attacked.attacker.front().pos, getOpponentColor());

    if (king_attacker.bUnderAttack) {
      // This means that the attacker can be taken, so it's not a checkmate
      return false;
    } else {
      // Last resort: can any piece get in between the attacker and the king?
      const SquareState chAttacker =
          getPieceAtPosition(king_attacked.attacker.front().pos);

      switch (chAttacker->mPiece) {
      case Piece::kPawn:
      case Piece::kKing: {
        // If it's a pawn, there's no space in between the attacker and the king
        // If it's a knight, it doesn't matter because the knight can 'jump'
        // So, this is checkmate
        bCheckmate = true;
      } break;

      case Piece::kBishop: {
        if (!canBeBlocked(king_attacked.attacker[0].pos, king,
                          Direction::DIAGONAL)) {
          // If no piece can get in the way, it's a checkmate
          bCheckmate = true;
        }
      } break;

      case Piece::kRook: {
        if (!canBeBlocked(king_attacked.attacker[0].pos, king,
                          king_attacked.attacker[0].dir)) {
          // If no piece can get in the way, it's a checkmate
          bCheckmate = true;
        }
      } break;

      case Piece::kQueen: {
        if (!canBeBlocked(king_attacked.attacker[0].pos, king,
                          king_attacked.attacker[0].dir)) {
          // If no piece can get in the way, it's a checkmate
          bCheckmate = true;
        }
      } break;

      default: {
        throw("!!!!Should not reach here. Invalid piece");
      } break;
      }
    }
  } else {
    // If there is more than one attacker and we reached this point, it's a
    // checkmate
    bCheckmate = true;
  }

  // If the game has ended, store in the class variable
  m_bGameFinished = bCheckmate;

  return bCheckmate;
}

bool Game::isKingInCheck(
    const Side iColor,
    const std::optional<IntendedMove> &pintended_move) const {
  const Position king = pintended_move.has_value() &&
                                Piece::kKing == pintended_move->chPiece.mPiece
                            ? pintended_move->to
                            : findKing(iColor);
  const UnderAttack king_attacked = isUnderAttack(king, iColor, pintended_move);
  return king_attacked.bUnderAttack;
}

bool Game::playerKingInCheck(
    const std::optional<IntendedMove> &intended_move) const {
  return isKingInCheck(getCurrentTurn(), intended_move);
}

bool Game::wouldKingBeInCheck(const PieceWithSide chPiece,
                              const Position present, const Position future,
                              EnPassant &S_enPassant) const {
  const IntendedMove intended_move{
      .chPiece = chPiece, .from = present, .to = future};
  return playerKingInCheck(intended_move);
}

Position Game::findKing(const Side iColor) const {
  const PieceWithSide chKing{.mPiece = Piece::kKing, .mSide = iColor};
  Position king;

  for (const Position pos : BoardPositions{}) {
    if (const SquareState square = getPieceAtPosition(pos);
        square && *square == chKing) {
      king = pos;
    }
  }

  return king;
}

void Game::changeTurns(void) {
  if (Side::kWhite == m_CurrentTurn) {
    m_CurrentTurn = Side::kBlack;
  } else {
    m_CurrentTurn = Side::kWhite;
  }
}

bool Game::isFinished() const { return m_bGameFinished; }

Side Game::getCurrentTurn() const { return m_CurrentTurn; }

Side Game::getOpponentColor() const {
  return Side::kWhite == getCurrentTurn() ? Side::kBlack : Side::kWhite;
}

std::tuple<Position, Position, SquareState>
Game::parseMoveWithPromotion(const std::string &move) const {
  const SquareState chPromoted =
      move[5] == '=' ? std::make_optional(charToPiece(move[6])) : std::nullopt;
  const auto [from, to] = parseMove(move);
  return {from, to, chPromoted};
}

void Game::logMove(std::string &to_record) {
  // If record contains only 5 chracters, add spaces
  // Because when
  if (to_record.length() == 5) {
    to_record += "  ";
  }

  if (Side::kWhite == getCurrentTurn()) {
    // If this was a white player move, create a new round and leave the
    // black_move empty
    Round round;
    round.white_move = to_record;
    round.black_move = "";

    rounds.push_back(round);
  } else {
    // If this was a black_move, just update the last Round
    Round round = rounds[rounds.size() - 1];
    round.black_move = to_record;

    // Remove the last round and put it back, now with the black move
    rounds.pop_back();
    rounds.push_back(round);
  }
}

std::string Game::getLastMove() const {
  std::string last_move;

  // Who did the last move?
  if (Side::kBlack == getCurrentTurn()) {
    // If it's black's turn now, white had the last move
    last_move = rounds[rounds.size() - 1].white_move;
  } else {
    // Last move was black's
    last_move = rounds[rounds.size() - 1].black_move;
  }

  return last_move;
}

void Game::deleteLastMove(void) {
  // Notice we already changed turns back
  if (Side::kWhite == getCurrentTurn()) {
    // Last move was white's turn, so simply pop from the back
    rounds.pop_back();
  } else {
    // Last move was black's, so let's
    Round round = rounds[rounds.size() - 1];
    round.black_move = "";

    // Pop last round and put it back, now without the black move
    rounds.pop_back();
    rounds.push_back(round);
  }
}

void Game::capturePiece(const PieceWithSide piece) {
  if (Side::kWhite == getPieceColor(piece)) {
    // A white piece was captured
    white_captured.push_back(piece);
  } else {
    // A black piece was captured
    black_captured.push_back(piece);
  }
}

void makeTheMove(chess::Game &current_game, const chess::Position present,
                 const chess::Position future, chess::EnPassant &S_enPassant,
                 chess::Castling &S_castling, chess::Promotion &S_promotion) {
  const SquareState chPiece = current_game.getPieceAtPosition(present);
  // -----------------------
  // Captured a piece?
  // -----------------------
  if (chPiece && current_game.isSquareOccupied(future)) {
    if (const SquareState chAuxPiece = current_game.getPieceAtPosition(future);
        chAuxPiece &&
        chess::getPieceColor(*chPiece) != chess::getPieceColor(*chAuxPiece)) {
      createNextMessage(chess::describePiece(*chAuxPiece) + " captured!\n");
    } else {
      std::cout << "Error. We should not be making this move\n";
      throw("Error. We should not be making this move");
    }
  } else if (S_enPassant.bApplied) {
    createNextMessage("Pawn captured by \"en passant\" move!\n");
  }

  if ((S_castling.bApplied)) {
    createNextMessage("Castling applied!\n");
  }

  current_game.movePiece(present, future, S_enPassant, S_castling, S_promotion);
}

} // namespace chess

#if defined(UNIT_TEST)

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Parse move", "[charToRow]") {
  CHECK(chess::charToRow('1') == 0);
  CHECK(chess::charToRow('2') == 1);
  CHECK(chess::charToRow('3') == 2);
  CHECK(chess::charToRow('4') == 3);
  CHECK(chess::charToRow('5') == 4);
  CHECK(chess::charToRow('6') == 5);
  CHECK(chess::charToRow('7') == 6);
  CHECK(chess::charToRow('8') == 7);
}

TEST_CASE("Parse move", "[charToCol]") {
  CHECK(chess::charToColumn('A') == 0);
  CHECK(chess::charToColumn('B') == 1);
  CHECK(chess::charToColumn('C') == 2);
  CHECK(chess::charToColumn('D') == 3);
  CHECK(chess::charToColumn('E') == 4);
  CHECK(chess::charToColumn('F') == 5);
  CHECK(chess::charToColumn('G') == 6);
  CHECK(chess::charToColumn('H') == 7);
}

TEST_CASE("Parse move", "[parseMove]") {
  const auto [from, to] = chess::parseMove("A2-A4");
  CHECK(from.iRow == 1);
  CHECK(from.iColumn == 0);
  CHECK(to.iRow == 3);
  CHECK(to.iColumn == 0);
}

TEST_CASE("Game", "[getPieceAtPosition]") {
  const chess::Game game;
  CHECK(game.getPieceAtPosition(chess::Position{0, 0}).has_value());
  CHECK(game.getPieceAtPosition(chess::Position{0, 0}).value() ==
        chess::pieces::R);
  CHECK(game.getPieceAtPosition(chess::Position{7, 7}).has_value());
  CHECK(game.getPieceAtPosition(chess::Position{7, 7}).value() ==
        chess::pieces::r);
  CHECK(!game.getPieceAtPosition(chess::Position{3, 3}).has_value());
}

TEST_CASE("Game", "[getPieceConsideredMove]") {
  const chess::Game game;
  // No intended move
  CHECK(game.getPieceConsiderMove(chess::Position{0, 0}).has_value());
  // With intended move of pawn
  constexpr auto intendedMove =
      chess::IntendedMove{.chPiece = chess::pieces::P,
                          .from = chess::Position{.iRow = 1, .iColumn = 0},
                          .to = chess::Position{.iRow = 3, .iColumn = 0}};
  CHECK(!game.getPieceConsiderMove(chess::Position{1, 0}, intendedMove)
             .has_value());
  CHECK(game.getPieceConsiderMove(chess::Position{3, 0}, intendedMove)
            .has_value());
  CHECK(
      game.getPieceConsiderMove(chess::Position{3, 0}, intendedMove).value() ==
      chess::pieces::P);
}

TEST_CASE("Game", "[isUnderAttack_initialBoard]") {
  const chess::Game game;
  // Not under attack
  {
    CHECK(!game.isUnderAttack(chess::Position{0, 0}, chess::Side::kWhite)
               .bUnderAttack);
    CHECK(!game.isUnderAttack(chess::Position{1, 0}, chess::Side::kWhite)
               .bUnderAttack);
    CHECK(!game.isUnderAttack(chess::Position{2, 0}, chess::Side::kWhite)
               .bUnderAttack);
  }
  // Under attack by white pawn and knight
  {
    const chess::UnderAttack under_attack =
        game.isUnderAttack(chess::Position{2, 0}, chess::Side::kBlack);
    CHECK(under_attack.bUnderAttack);
    CHECK(under_attack.iNumAttackers == 2);
    CHECK(under_attack.attacker[0].pos == chess::Position{1, 1});
    CHECK(under_attack.attacker[0].dir == chess::Direction::DIAGONAL);
    CHECK(under_attack.attacker[1].pos == chess::Position{0, 1});
    CHECK(under_attack.attacker[1].dir == chess::Direction::L_SHAPE);
  }
}

TEST_CASE("Game", "[findKing]") {
  chess::Game game;
  const chess::Position whiteKingPosition = game.findKing(chess::Side::kWhite);
  CHECK(whiteKingPosition == chess::Position{0, 4});

  const chess::Position blackKingPosition = game.findKing(chess::Side::kBlack);
  CHECK(blackKingPosition == chess::Position{7, 4});
}

#endif
