#ifndef EVALUATE_H
#define EVALUATE_H

#include "board.hpp"


int pstValue(int piece, int square);
int kingSafety(const Board& board);

int evaluate(Board& board);

#endif 