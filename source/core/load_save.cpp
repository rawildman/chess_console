#include "game.hpp"
#include "user_interface.hpp"
#include "validation.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

namespace chess {
void saveGame(const chess::Game &current_game) {
  std::string file_name;
  std::cout << "Type file name to be saved (no extension): ";

  getline(std::cin, file_name);
  file_name += ".dat";

  std::ofstream ofs(file_name);
  if (ofs.is_open()) {
    // Write the date and time of save operation
    auto time_now = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(time_now);
    ofs << "[Chess console] Saved at: " << std::ctime(&end_time);

    // Write the moves
    for (unsigned i = 0; i < current_game.rounds.size(); i++) {
      ofs << current_game.rounds[i].white_move.c_str() << " | "
          << current_game.rounds[i].black_move.c_str() << "\n";
    }

    ofs.close();
    createNextMessage("Game saved as " + file_name + "\n");
  } else {
    std::cout << "Error creating file! Save failed\n";
  }

  return;
}

chess::Game loadGame() {
  std::string file_name;
  std::cout << "Type file name to be loaded (no extension): ";

  getline(std::cin, file_name);
  file_name += ".dat";

  std::ifstream ifs(file_name);

  if (ifs) {
    // First, reset the pieces
    auto current_game = chess::Game{};

    // Now, read the lines from the file and then make the moves
    std::string line;
    while (std::getline(ifs, line)) {
      // Skip lines that starts with "[]"
      if (0 == line.compare(0, 1, "[")) {
        continue;
      }

      // There might be one or two moves in the line
      std::array<std::string, 2> loaded_move;

      // Find the separator and subtract one
      std::size_t separator = line.find(" |");

      // For the first move, read from the beginning of the std::string until
      // the separator
      loaded_move[0] = line.substr(0, separator);

      // For the second move, read from the separator until the end of the
      // std::string (omit second parameter)
      loaded_move[1] = line.substr(line.find("|") + 2);

      for (int i = 0; i < 2 && loaded_move[i] != ""; i++) {
        const auto [from, to, promoted] =
            current_game.parseMoveWithPromotion(loaded_move[i]);

        // Check if line is valid
        if (from.iColumn < 0 || from.iColumn > 7 || from.iRow < 0 ||
            from.iRow > 7 || to.iColumn < 0 || to.iColumn > 7 || to.iRow < 0 ||
            to.iRow > 7) {
          createNextMessage("[Invalid] Can't load this game because there are "
                            "invalid lines!\n");
          return chess::Game{};
        }

        // Is that move allowed? (should be because we already validated before
        // saving)
        chess::EnPassant S_enPassant = {0};
        chess::Castling S_castling = {0};
        chess::Promotion S_promotion = {0};

        if (!isMoveValid(current_game, from, to, S_enPassant, S_castling,
                         S_promotion)) {
          createNextMessage("[Invalid] Can't load this game because there are "
                            "invalid moves!\n");
          return chess::Game{};
        }

        // ---------------------------------------------------
        // A promotion occurred
        // ---------------------------------------------------
        if (S_promotion.bApplied && promoted) {
          if (promoted->mPiece != Piece::kQueen &&
              promoted->mPiece != Piece::kRook &&
              promoted->mPiece != Piece::kKnight &&
              promoted->mPiece != Piece::kBishop) {
            createNextMessage("[Invalid] Can't load this game because there is "
                              "an invalid promotion!\n");
            return chess::Game{};
          }

          S_promotion.chBefore = *current_game.getPieceAtPosition(from);

          if (chess::Side::kWhite == current_game.getCurrentTurn()) {
            S_promotion.chAfter = *promoted;
          } else {
            S_promotion.chAfter = *promoted;
          }
        }

        // Log the move
        current_game.logMove(loaded_move[i]);

        // Make the move
        makeTheMove(current_game, from, to, S_enPassant, S_castling,
                    S_promotion);
      }
    }
    // Extra line after the user input
    createNextMessage("Game loaded from " + file_name + "\n");
    return current_game;
  } else {
    createNextMessage("Error loading " + file_name +
                      ". Creating a new game instead\n");
    return chess::Game{};
  }
}
} // namespace chess