#include "user_interface.h"
#include "board.h"

#include <cassert>
#include <iostream>
#include <map>

namespace chess {
namespace {
constexpr char WHITE_SQUARE = 0x20;
constexpr char BLACK_SQUARE = 0x2F;
constexpr char EMPTY_SQUARE = 0x20;

const std::map<PieceWithSide, char> kPieceToChar = {
    {pieces::R, 'R'}, {pieces::N, 'N'}, {pieces::B, 'B'}, {pieces::Q, 'Q'},
    {pieces::K, 'K'}, {pieces::P, 'P'}, {pieces::r, 'r'}, {pieces::n, 'n'},
    {pieces::b, 'b'}, {pieces::q, 'q'}, {pieces::k, 'k'}, {pieces::p, 'p'}};

const std::map<Side, char> kSideToChar = {{Side::kWhite, WHITE_SQUARE},
                                          {Side::kBlack, BLACK_SQUARE}};

// Save the next message to be displayed (regardind last command)
std::string next_message;

void printSquare(const Position &position, const int subLine,
                 const int subColumn, const Side color, const Game &game) {
  // The piece should be in the "middle" of the cell
  // For 3 sub-lines, in sub-line 1
  // For 6 sub-columns, sub-column 3
  if (subLine == 1 && subColumn == 3) {
    const SquareState state = game.getPieceAtPosition(position);
    std::cout << (state ? kPieceToChar.at(*state) : kSideToChar.at(color));
  } else {
    std::cout << kSideToChar.at(color);
  }
}
} // namespace

//---------------------------------------------------------------------------------------
// User interface
// All the functions regarding the user interface are in this section
// Logo, Menu, Board, messages to the user
//---------------------------------------------------------------------------------------
void createNextMessage(const std::string &msg) { next_message = msg; }

void appendToNextMessage(const std::string &msg) { next_message += msg; }
void clearScreen(void) { system("cls"); }

void printLogo(void) {
  std::cout << "    ======================================\n";
  std::cout << "       _____ _    _ ______  _____ _____\n";
  std::cout << "      / ____| |  | |  ____|/ ____/ ____|\n";
  std::cout << "     | |    | |__| | |__  | (___| (___ \n";
  std::cout << "     | |    |  __  |  __|  \\___ \\\\___ \\ \n";
  std::cout << "     | |____| |  | | |____ ____) |___) |\n";
  std::cout << "      \\_____|_|  |_|______|_____/_____/\n\n";
  std::cout << "    ======================================\n\n";
}

void printMenu(void) {
  std::cout
      << "Commands: (N)ew game\t(M)ove \t(U)ndo \t(S)ave \t(L)oad \t(Q)uit \n";
}

void printMessage(void) {
  std::cout << next_message << std::endl;

  next_message = "";
}

void printLine(const int iLine, const Side color1, const Side color2,
               const Game &game) {
  // Example (for CELL = 6):

  //  [6-char]
  //  |______| subline 1
  //  |___Q__| subline 2
  //  |______| subline 3

  // Define the CELL variable here.
  // It represents how many horizontal characters will form one square
  // The number of vertical characters will be CELL/2
  // You can change it to alter the size of the board (an odd number will make
  // the squares look rectangular)
  constexpr int CELL = 6;

  // Since the width of the characters BLACK and WHITE is half of the height,
  // we need to use two characters in a row.
  // So if we have CELL characters, we must have CELL/2 sublines
  for (int subLine = 0; subLine < CELL / 2; subLine++) {
    // A sub-line is consisted of 8 cells, but we can group it
    // in 4 iPairs of black&white
    for (int iPair = 0; iPair < 4; iPair++) {
      // First cell of the pair
      for (int subColumn = 0; subColumn < CELL; subColumn++) {
        printSquare(Position{iLine, iPair * 2}, subLine, subColumn, color1,
                    game);
      }

      // Second cell of the pair
      for (int subColumn = 0; subColumn < CELL; subColumn++) {
        printSquare(Position{iLine, iPair * 2 + 1}, subLine, subColumn, color2,
                    game);
      }
    }

    // Write the number of the line on the right
    if (1 == subLine) {
      std::cout << "   " << iLine + 1;
    }

    std::cout << "\n";
  }
}

void printSituation(const Game &game) {
  // Last moves - print only if at least one move has been made
  if (0 != game.rounds.size()) {
    std::cout << "Last moves:\n";

    int iMoves = game.rounds.size();
    int iToShow = iMoves >= 5 ? 5 : iMoves;

    std::string space = "";

    while (iToShow--) {
      if (iMoves < 10) {
        // Add an extra hardspace to allign the numbers that are smaller than 10
        space = " ";
      }

      std::cout << space << iMoves << " ..... "
                << game.rounds[iMoves - 1].white_move.c_str() << " | "
                << game.rounds[iMoves - 1].black_move.c_str() << "\n";
      iMoves--;
    }

    std::cout << "\n";
  }

  // Captured pieces - print only if at least one piece has been captured
  if (0 != game.white_captured.size() || 0 != game.black_captured.size()) {
    std::cout << "---------------------------------------------\n";
    std::cout << "WHITE captured: ";
    for (unsigned i = 0; i < game.white_captured.size(); i++) {
      std::cout << kPieceToChar.at(game.white_captured[i]) << " ";
    }
    std::cout << "\n";

    std::cout << "black captured: ";
    for (unsigned i = 0; i < game.black_captured.size(); i++) {
      std::cout << kPieceToChar.at(game.black_captured[i]) << " ";
    }
    std::cout << "\n";

    std::cout << "---------------------------------------------\n";
  }

  // Current turn
  std::cout << "Current turn: "
            << (game.getCurrentTurn() == Side::kWhite ? "WHITE (upper case)"
                                                      : "BLACK (lower case)")
            << "\n\n";
}

void printBoard(const Game &game) {
  std::cout << "   A     B     C     D     E     F     G     H\n\n";

  for (int iLine = 7; iLine >= 0; iLine--) {
    if (iLine % 2 == 0) {
      // Line starting with BLACK
      printLine(iLine, Side::kBlack, Side::kWhite, game);
    } else {
      // Line starting with WHITE
      printLine(iLine, Side::kWhite, Side::kBlack, game);
    }
  }
}

Position toPosition(const std::string &move) {
  assert(move.size() >= 2);
  const char column = toupper(move[0]);
  const char row = move[1];
  // ---------------------------------------------------
  // Did the user pick a valid piece?
  // Must check if:
  // - It's inside the board (A1-H8)
  // - There is a piece in the square
  // - The piece is consistent with the player's turn
  // ---------------------------------------------------
  if (column < 'A' || column > 'H') {
    createNextMessage("Invalid column.\n");
    return {.iRow = 0, .iColumn = 0};
  }
  if (row < '0' || row > '8') {
    createNextMessage("Invalid row.\n");
    return {.iRow = 0, .iColumn = 0};
  }
  return {.iRow = row - '1', .iColumn = column - 'A'};
}
} // namespace chess
