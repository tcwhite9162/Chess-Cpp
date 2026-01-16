#pragma once

#include <cassert>

#include "core/move.hpp"
#include "core/board.hpp"

struct MoveList {
    Move moves[256];
    int count = 0;

    inline void add(const Move& m) {
        assert(count < 256);
        moves[count++] = m;
    }
};

namespace MoveGen {

    void generatePawnMoves(const Board& board, int color, MoveList& moves);
    void generateKnightMoves(const Board& board, int color, MoveList& moves);
    void generateSlidingMoves(const Board& board, int color, MoveList& moves);
    void generateKingMoves(const Board& board, int color, MoveList& moves);

    void generatePseudoLegalMoves(const Board& board, MoveList& moves);
    void generateLegalMoves(Board& board, MoveList& moves);

    void generatePawnCaptures(const Board& board, int side, MoveList& captures);
    void generateKnightCaptures(const Board& board, int side, MoveList& captures);
    void generateSlidingCaptures(const Board& board, int side, MoveList& captures);
    void generateKingCaptures(const Board& board, int side, MoveList& captures);

    void generateCaptures(const Board& board, MoveList& captures);
    void generateEvasions(Board& board, MoveList& evasions);
}