#pragma once

namespace chess {
class Game;

void saveGame(const chess::Game &current_game);
chess::Game loadGame();

} // namespace chess