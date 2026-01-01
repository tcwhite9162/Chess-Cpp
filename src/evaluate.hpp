#pragma once

#include "board.hpp"


int pstValue(int piece, int square);
int kingSafety(const Board& board);

int evaluate(Board& board);
