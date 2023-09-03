#include "chess.hpp"
#include "load_save.hpp"
#include "user_interface.hpp"
#include "validation.hpp"

#include <cassert>
#include <iostream>

namespace chess {

void undoMove(chess::Game &current_game) {
  if (!current_game.undoIsPossible()) {
    createNextMessage("Undo is not possible now!\n");
    return;
  }
  current_game.undoLastMove();
  createNextMessage("Last move was undone\n");
}

void movePiece(chess::Game &current_game) {
  // Get user input for the piece they want to move
  std::cout << "Choose piece to be moved. (example: A1 or b2): ";

  std::string move_from;
  getline(std::cin, move_from);

  if (move_from.length() > 2) {
    createNextMessage("You should type only two characters (column and row)\n");
    return;
  }

  std::string to_record;
  to_record += move_from[0];
  to_record += move_from[1];
  to_record += "-";

  const Position present = toPosition(move_from);

  const SquareState chPiece = current_game.getPieceAtPosition(present);

  if (!chPiece) {
    createNextMessage("You picked an EMPTY square.\n");
    return;
  }
  std::cout << "Piece is " << pieceToChar(*chPiece) << "\n";
  if (Side::kWhite == current_game.getCurrentTurn() &&
      chess::isBlackPiece(*chPiece)) {
    createNextMessage("It is WHITE's turn and you picked a BLACK piece\n");
    return;
  } else if (Side::kBlack == current_game.getCurrentTurn() &&
             chess::isWhitePiece(*chPiece)) {
    createNextMessage("It is BLACK's turn and you picked a WHITE piece\n");
    return;
  }

  // ---------------------------------------------------
  // Get user input for the square to move to
  // ---------------------------------------------------
  std::cout << "Move to: ";
  std::string move_to;
  getline(std::cin, move_to);

  if (move_to.length() > 2) {
    createNextMessage("You should type only two characters (column and row)\n");
    return;
  }

  // Put in the std::string to be logged
  to_record += move_to[0];
  to_record += move_to[1];

  const Position future = toPosition(move_to);
  // Check if it is not the exact same square
  if (future == present) {
    createNextMessage("[Invalid] You picked the same square!\n");
    return;
  }

  // Is that move allowed?
  chess::EnPassant S_enPassant = {0};
  chess::Castling S_castling = {0};
  chess::Promotion S_promotion = {0};

  if (!isMoveValid(current_game, present, future, S_enPassant, S_castling,
                   S_promotion)) {
    createNextMessage("[Invalid] Piece can not move to that square!\n");
    return;
  }

  // ---------------------------------------------------
  // Promotion: user most choose a piece to
  // replace the pawn
  // ---------------------------------------------------
  if (S_promotion.bApplied) {
    std::cout << "Promote to (Q, R, N, B): ";
    std::string piece;
    getline(std::cin, piece);

    if (piece.length() > 1) {
      createNextMessage("You should type only one character (Q, R, N or B)\n");
      return;
    }

    const char chPromoted = toupper(piece[0]);

    if (chPromoted != 'Q' && chPromoted != 'R' && chPromoted != 'N' &&
        chPromoted != 'B') {
      createNextMessage("Invalid character.\n");
      return;
    }

    S_promotion.chBefore = *current_game.getPieceAtPosition(present);

    if (chess::Side::kWhite == current_game.getCurrentTurn()) {
      S_promotion.chAfter = charToPiece(toupper(chPromoted));
    } else {
      S_promotion.chAfter = charToPiece(tolower(chPromoted));
    }

    to_record += '=';
    to_record += toupper(chPromoted); // always log with a capital letter
  }

  // ---------------------------------------------------
  // Log the move: do it prior to making the move
  // because we need the getCurrentTurn()
  // ---------------------------------------------------
  current_game.logMove(to_record);

  // ---------------------------------------------------
  // Make the move
  // ---------------------------------------------------
  makeTheMove(current_game, present, future, S_enPassant, S_castling,
              S_promotion);

  // ---------------------------------------------------------------
  // Check if this move we just did put the oponent's king in check
  // Keep in mind that player turn has already changed
  // ---------------------------------------------------------------
  if (current_game.playerKingInCheck()) {
    if (current_game.isCheckMate()) {
      if (chess::Side::kWhite == current_game.getCurrentTurn()) {
        appendToNextMessage("Checkmate! Black wins the game!\n");
      } else {
        appendToNextMessage("Checkmate! White wins the game!\n");
      }
    } else {
      if (chess::Side::kWhite == current_game.getCurrentTurn()) {
        appendToNextMessage("White king is in check!\n");
      } else {
        appendToNextMessage("Black king is in check!\n");
      }
    }
  }

  return;
}

} // namespace chess

int main() {
  chess::clearScreen();
  chess::printLogo();

  bool bRun = true;
  chess::Game current_game;
  while (bRun) {
    chess::printMessage();
    chess::printMenu();

    // Get input from user
    std::cout << "Type here: ";

    std::string input = "";
    getline(std::cin, input);

    if (input.length() != 1) {
      std::cout << "Invalid option. Type one letter only\n\n";
      continue;
    }

    try {
      switch (input[0]) {
      case 'N':
      case 'n': {
        current_game = chess::Game{};
        chess::clearScreen();
        chess::printLogo();
        chess::printSituation(current_game);
        chess::printBoard(current_game);
      } break;

      case 'M':
      case 'm': {
        if (current_game.isFinished()) {
          std::cout << "This game has already finished!\n";
        } else {
          chess::movePiece(current_game);
          // clearScreen();
          chess::printLogo();
          chess::printSituation(current_game);
          chess::printBoard(current_game);
        }
      } break;

      case 'Q':
      case 'q': {
        bRun = false;
      } break;

      case 'U':
      case 'u': {
        chess::undoMove(current_game);
        // clearScreen();
        chess::printLogo();
        chess::printSituation(current_game);
        chess::printBoard(current_game);
      } break;

      case 'S':
      case 's': {
        chess::saveGame(current_game);
        chess::clearScreen();
        chess::printLogo();
        chess::printSituation(current_game);
        chess::printBoard(current_game);
      } break;

      case 'L':
      case 'l': {
        current_game = chess::loadGame();
        chess::clearScreen();
        chess::printLogo();
        chess::printSituation(current_game);
        chess::printBoard(current_game);
      } break;

      default: {
        std::cout << "Option does not exist\n\n";
      } break;
      }
    } catch (const chess::GameException& err) {
      std::cout << "Error: " << err.what() << std::endl;
    }
  }

  return 0;
}
