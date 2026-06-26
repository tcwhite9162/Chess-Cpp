#pragma once

#include <optional>

#include "core/board.hpp"
#include "core/move.hpp"

namespace Interface {

enum class PerftMode : uint8_t { Single, Divide, Suite, All };

std::string getCommandWord(const std::string& line);

void userLoop(Board& board);

void handleCommand(std::string& line, Board& board);

void handlePosition(const std::string& line, Board& board);
void handleMove(const std::string& line, Board& board);
void handleMoves(const std::string& line, Board& board);
void handlePerft(const std::string& line, Board& board, PerftMode mode);
void handleSearch(const std::string& line, Board& board, bool range = false);

void showState(const Board& board, bool verbose = false);
void showCommands();
void clearScreen();

std::optional<Move> parseMoveStr(const std::string& moveStr, const Board& board);

namespace {
inline std::optional<int> strToSquare(const std::string& s);
}

} // namespace Interface

