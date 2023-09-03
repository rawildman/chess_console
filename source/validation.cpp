#include "validation.hpp"
#include "chess.h"
#include "game.h"
#include "user_interface.h"

#include <cassert>
#include <iostream>

namespace chess {
bool isMoveValid(const Game &current_game, const Position present,
                 const Position future, chess::EnPassant &S_enPassant,
                 chess::Castling &S_castling, chess::Promotion &S_promotion) {
  bool bValid = false;

  const SquareState chPiece = current_game.getPieceAtPosition(present);
  assert(chPiece.has_value());

  // ----------------------------------------------------
  // 1. Is the piece  allowed to move in that direction?
  // ----------------------------------------------------
  switch (chPiece->mPiece) {
  case Piece::kPawn: {
    // Wants to move forward
    if (future.iColumn == present.iColumn) {
      // Simple move forward
      if ((chess::isWhitePiece(*chPiece) && future.iRow == present.iRow + 1) ||
          (chess::isBlackPiece(*chPiece) && future.iRow == present.iRow - 1)) {
        if (!current_game.getPieceAtPosition(future)) {
          bValid = true;
        }
      }

      // Double move forward
      else if ((chess::isWhitePiece(*chPiece) &&
                future.iRow == present.iRow + 2) ||
               (chess::isBlackPiece(*chPiece) &&
                future.iRow == present.iRow - 2)) {
        // This is only allowed if the pawn is in its original place
        if (chess::isWhitePiece(*chPiece)) {
          if (!current_game.getPieceAtPosition(
                  {future.iRow - 1, future.iColumn}) &&
              !current_game.getPieceAtPosition(future) && 1 == present.iRow) {
            bValid = true;
          }
        } else // if ( isBlackPiece(chPiece) )
        {
          if (!current_game.getPieceAtPosition(
                  {future.iRow + 1, future.iColumn}) &&
              !current_game.getPieceAtPosition(future) && 6 == present.iRow) {
            bValid = true;
          }
        }
      } else {
        // This is invalid
        return false;
      }
    }

    // The "en passant" move
    else if ((chess::isWhitePiece(*chPiece) && 4 == present.iRow &&
              5 == future.iRow && 1 == abs(future.iColumn - present.iColumn)) ||
             (chess::isBlackPiece(*chPiece) && 3 == present.iRow &&
              2 == future.iRow && 1 == abs(future.iColumn - present.iColumn))) {
      // It is only valid if last move of the opponent was a double move forward
      // by a pawn on a adjacent column
      const std::string last_move = current_game.getLastMove();

      // Parse the line
      const auto [LastMoveFrom, LastMoveTo] = parseMove(last_move);

      // First of all, was it a pawn?
      const SquareState chLstMvPiece =
          current_game.getPieceAtPosition(LastMoveTo);

      if (!chLstMvPiece.has_value() || chLstMvPiece->mPiece != Piece::kPawn) {
        return false;
      }

      // Did the pawn have a double move forward and was it an adjacent column?
      if (2 == abs(LastMoveTo.iRow - LastMoveFrom.iRow) &&
          1 == abs(LastMoveFrom.iColumn - present.iColumn)) {
        std::cout << "En passant move!\n";
        bValid = true;

        S_enPassant.bApplied = true;
        S_enPassant.PawnCaptured.iRow = LastMoveTo.iRow;
        S_enPassant.PawnCaptured.iColumn = LastMoveTo.iColumn;
      }
    }

    // Wants to capture a piece
    else if (1 == abs(future.iColumn - present.iColumn)) {
      if ((chess::isWhitePiece(*chPiece) && future.iRow == present.iRow + 1) ||
          (chess::isBlackPiece(*chPiece) && future.iRow == present.iRow - 1)) {
        // Only allowed if there is something to be captured in the square
        if (current_game.getPieceAtPosition(future)) {
          bValid = true;
          std::cout << "Pawn captured a piece!\n";
        }
      }
    } else {
      // This is invalid
      return false;
    }

    // If a pawn reaches its eight rank, it must be promoted to another piece
    if ((chess::isWhitePiece(*chPiece) && 7 == future.iRow) ||
        (chess::isBlackPiece(*chPiece) && 0 == future.iRow)) {
      std::cout << "Pawn must be promoted!\n";
      S_promotion.bApplied = true;
    }
  } break;

  case Piece::kRook: {
    // Horizontal move
    if ((future.iRow == present.iRow) && (future.iColumn != present.iColumn)) {
      // Check if there are no pieces on the way
      if (current_game.isPathFree(present, future,
                                  chess::Direction::HORIZONTAL)) {
        bValid = true;
      }
    }
    // Vertical move
    else if ((future.iRow != present.iRow) &&
             (future.iColumn == present.iColumn)) {
      // Check if there are no pieces on the way
      if (current_game.isPathFree(present, future,
                                  chess::Direction::VERTICAL)) {
        bValid = true;
      }
    }
  } break;

  case Piece::kKnight: {
    if ((2 == abs(future.iRow - present.iRow)) &&
        (1 == abs(future.iColumn - present.iColumn))) {
      bValid = true;
    }

    else if ((1 == abs(future.iRow - present.iRow)) &&
             (2 == abs(future.iColumn - present.iColumn))) {
      bValid = true;
    }
  } break;

  case Piece::kBishop: {
    // Diagonal move
    if (abs(future.iRow - present.iRow) ==
        abs(future.iColumn - present.iColumn)) {
      // Check if there are no pieces on the way
      if (current_game.isPathFree(present, future,
                                  chess::Direction::DIAGONAL)) {
        bValid = true;
      }
    }
  } break;

  case Piece::kQueen: {
    // Horizontal move
    if ((future.iRow == present.iRow) && (future.iColumn != present.iColumn)) {
      // Check if there are no pieces on the way
      if (current_game.isPathFree(present, future,
                                  chess::Direction::HORIZONTAL)) {
        bValid = true;
      }
    }
    // Vertical move
    else if ((future.iRow != present.iRow) &&
             (future.iColumn == present.iColumn)) {
      // Check if there are no pieces on the way
      if (current_game.isPathFree(present, future,
                                  chess::Direction::VERTICAL)) {
        bValid = true;
      }
    }

    // Diagonal move
    else if (abs(future.iRow - present.iRow) ==
             abs(future.iColumn - present.iColumn)) {
      // Check if there are no pieces on the way
      if (current_game.isPathFree(present, future,
                                  chess::Direction::DIAGONAL)) {
        bValid = true;
      }
    }
  } break;

  case Piece::kKing: {
    // Horizontal move by 1
    if ((future.iRow == present.iRow) &&
        (1 == abs(future.iColumn - present.iColumn))) {
      bValid = true;
    }

    // Vertical move by 1
    else if ((future.iColumn == present.iColumn) &&
             (1 == abs(future.iRow - present.iRow))) {
      bValid = true;
    }

    // Diagonal move by 1
    else if ((1 == abs(future.iRow - present.iRow)) &&
             (1 == abs(future.iColumn - present.iColumn))) {
      bValid = true;
    }

    // Castling
    else if ((future.iRow == present.iRow) &&
             (2 == abs(future.iColumn - present.iColumn))) {
      // Castling is only allowed in these circunstances:

      // 1. King is not in check
      if (true == current_game.playerKingInCheck()) {
        return false;
      }

      // 2. No pieces in between the king and the rook
      if (false == current_game.isPathFree(present, future,
                                           chess::Direction::HORIZONTAL)) {
        return false;
      }

      // 3. King and rook must not have moved yet;
      // 4. King must not pass through a square that is attacked by an enemy
      // piece
      if (future.iColumn > present.iColumn) {
        // if future.iColumn is greather, it means king side
        if (false ==
            current_game.castlingAllowed(chess::BoardSide::KING_SIDE,
                                         chess::getPieceColor(*chPiece))) {
          createNextMessage("Castling to the king side is not allowed.\n");
          return false;
        } else {
          // Check if the square that the king skips is not under attack
          chess::UnderAttack square_skipped =
              current_game.isUnderAttack({present.iRow, present.iColumn + 1},
                                         current_game.getCurrentTurn());
          if (false == square_skipped.bUnderAttack) {
            // Fill the S_castling structure
            S_castling.bApplied = true;

            // Present position of the rook
            S_castling.rook_before.iRow = present.iRow;
            S_castling.rook_before.iColumn = present.iColumn + 3;

            // Future position of the rook
            S_castling.rook_after.iRow = future.iRow;
            S_castling.rook_after.iColumn =
                present.iColumn + 1; // future.iColumn -1

            bValid = true;
          }
        }
      } else // if (future.iColumn < present.iColumn)
      {
        // if present.iColumn is greather, it means queen side
        if (!current_game.castlingAllowed(BoardSide::QUEEN_SIDE,
                                          getPieceColor(*chPiece))) {
          createNextMessage("Castling to the queen side is not allowed.\n");
          return false;
        } else {
          // Check if the square that the king skips is not attacked
          chess::UnderAttack square_skipped =
              current_game.isUnderAttack({present.iRow, present.iColumn - 1},
                                         current_game.getCurrentTurn());
          if (false == square_skipped.bUnderAttack) {
            // Fill the S_castling structure
            S_castling.bApplied = true;

            // Present position of the rook
            S_castling.rook_before.iRow = present.iRow;
            S_castling.rook_before.iColumn = present.iColumn - 4;

            // Future position of the rook
            S_castling.rook_after.iRow = future.iRow;
            S_castling.rook_after.iColumn =
                present.iColumn - 1; // future.iColumn +1

            bValid = true;
          }
        }
      }
    }
  } break;

  default:
    std::cout << "!!!!Should not reach here. \n\n\n";
    break;
  }

  // If it is a move in an invalid direction, do not even bother to check the
  // rest
  if (!bValid) {
    std::cout << "Piece is not allowed to move to that square\n";
    return false;
  }

  // -------------------------------------------------------------------------
  // 2. Is there another piece of the same color on the destination square?
  // -------------------------------------------------------------------------
  if (current_game.isSquareOccupied(future)) {
    if (const SquareState chAuxPiece = current_game.getPieceAtPosition(future);
        chAuxPiece && getPieceColor(*chPiece) == getPieceColor(*chAuxPiece)) {
      std::cout << "Position is already taken by a piece of the same color\n";
      return false;
    }
  }

  // ----------------------------------------------
  // 3. Would the king be in check after the move?
  // ----------------------------------------------
  if (current_game.wouldKingBeInCheck(*chPiece, present, future, S_enPassant)) {
    std::cout << "Move would put player's king in check\n";
    return false;
  }

  return bValid;
}
} // namespace chess