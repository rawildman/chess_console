#pragma once
#include "board.h"
#include "chess.h"
#include "game.h"

namespace chess {
void createNextMessage(const std::string &msg);
void appendToNextMessage(const std::string &msg);
void clearScreen(void);
void printLogo(void);
void printLogo(void);
void printMenu(void);
void printMessage(void);
void printLine(int iLine, Side color1, Side color2, const Game &game);
void printSituation(const Game &game);
void printBoard(const Game &game);
Position toPosition(const std::string &move);
} // namespace chess