#pragma once

#include <optional>

#include "core/move.hpp"
#include "core/board.hpp"

namespace Interface {

  enum class PerftMode {
    Single,
    Divide,
    Suite,
    All
  };

  std::string getCommandWord(const std::string& line);

  void userLoop(Board& board);

  void handleCommand(std::string& line, Board& board);

  void handlePosition(const std::string& line, Board& board);
  void handleMove(const std::string& line, Board& board);
  void handleMoves(const std::string& line, Board& board);
  void handlePerft(const std::string& line, Board& board, PerftMode mode);
  void handleSearch(const std::string& line, Board& board, bool range=false);
  void showState(Board& board, bool verbose=false);
  void showCommands();
  void clearScreen();

  std::optional<Move> parseMoveStr(const std::string& moveStr, Board& board);
}
