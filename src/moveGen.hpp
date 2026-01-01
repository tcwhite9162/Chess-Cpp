#pragma once

#include <assert.h>

#include "board.hpp"
#include "move.hpp"

struct MoveList {
    Move moves[256];
    int count = 0;

    inline void add(const Move& m) {
        assert(count < 256);
        moves[count++] = m;
    }
};

void generatePawnMoves(const Board& board, int color, MoveList& moves);
void generateKnightMoves(const Board& board, int color, MoveList& moves);
void generateSlidingMoves(const Board& board, int color, MoveList& moves);
void generateKingMoves(const Board& board, int color, MoveList& moves);

void generatePseudoLegalMoves(const Board& board, MoveList& moves);
void generateLegalMoves(Board& board, MoveList& moves);

int countPseudoLegalMoves(Board& board, int turn);
