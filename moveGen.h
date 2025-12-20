#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <assert.h>

#include "board.h"
#include "move.h"

struct MoveList {
    Move moves[256];
    int count = 0;

    inline void add(const Move& m) {
        assert(count < 256);
        moves[count++] = m;
    }
};

void generatePawnMoves(const Board& board, int square, MoveList& moves);
void generateKnightMoves(const Board& board, int square, MoveList& moves);
void generateSlidingMoves(const Board& board, int square, MoveList& moves);
void generateKingMoves(const Board& board, int square, MoveList& moves);

void generateAllMoves(const Board& board, MoveList& moves);
void generateLegalMoves(Board& board, MoveList& moves);

#endif