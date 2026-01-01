#pragma once

#include "move.hpp"
#include "board.hpp"

struct SearchResult {
    Move bestMove;
    int score;
};

int negamax(Board& board, int depth, int alpha, int beta);
SearchResult searchBestMove(Board& board, int depth);
