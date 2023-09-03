#pragma once

#include <filesystem>

namespace chess {
class Game;

void saveGame(const chess::Game &current_game);
chess::Game loadGame();
chess::Game loadGame(const std::filesystem::path &file);

} // namespace chess