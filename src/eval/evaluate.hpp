#pragma once

#include "core/board.hpp"


int pstValue(int piece, int square);
int kingSafety(const Board& board);

int evaluate(Board& board);
