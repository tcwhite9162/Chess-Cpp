#ifndef MOVEGEN_H
#define MOVEGEN_H

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

void generatePawnMoves(const Board& board, int square, MoveList& moves);
void generateKnightMoves(const Board& board, int square, MoveList& moves);
void generateSlidingMoves(const Board& board, int square, MoveList& moves);
void generateKingMoves(const Board& board, int square, MoveList& moves);

void generatePseudoLegalMoves(const Board& board, MoveList& moves);
void generateLegalMoves(Board& board, MoveList& moves);

int countPseudoLegalMoves(Board& board, int turn);

int countMobility(const Board& board, int side);
int countPawnMobility(const Board& board, int square, int piece);
int countKnightMobility(const Board& board, int square, int piece);
int countBishopMobility(const Board& board, int square, int piece);
int countRookMobility(const Board& board, int square, int piece);
int countQueenMobility(const Board& board, int square, int piece);
int countKingMobility(const Board& board, int square, int piece);


#endif