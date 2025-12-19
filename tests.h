#ifndef TESTS_H
#define TESTS_H

#include <string>
#include "board.h"

long long perft(Board &board, int depth);
long long testPerft(Board &board, int depth);
long long perft_divide(Board &board, int depth);
inline std::string squareToString(int sq);
std::string moveToString(const Move &m);

#endif