#pragma once

#include <string>
#include "../src/board.hpp"

long long perft(Board &board, int depth);
void testPerft(Board &board, int depth);
void testSinglePerft(Board &board, int depth);
long long perft_divide(Board &board, int depth);
inline std::string squareToString(int sq);
std::string moveToString(const Move &m);
long long perft_debug(Board &board, int depth);

void debugGeneratePawnMoves(const Board &board, int color);