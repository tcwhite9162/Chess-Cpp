#pragma once

#include "../board.hpp"
#include "../move.hpp"

namespace Interface {
  void inputLoop(Board& board);
  void handleCommand(std::string& line, Board& board);

  void handlePosition(const std::string& line, Board& board);
  void handleMove(const std::string& line, Board& board);
  void handleMoves(const std::string& line, Board& board);
  void handlePerft(const std::string& line, Board& board, bool divide=false, bool suite=false, bool all=false);
  void showState(Board& board);
  void showCommands();

  Move parseMoveStr(const std::string& moveStr, Board& board);
}
