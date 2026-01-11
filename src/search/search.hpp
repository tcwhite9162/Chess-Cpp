#pragma once

#include "core/move.hpp"
#include "core/board.hpp"
#include "search/tt.hpp"

struct SearchResult {
    Move bestMove;
    int score;
};

namespace Search {

    int negamax(Board& board, int depth, int alpha, int beta);
    SearchResult searchBestMove(Board& board, int depth);


    inline void updateEntry(TTEntry& entry, u64 key, int depth, int score, const Move& bestMove, u8 flag) {
        entry.key = key;
        entry.depth = depth;
        entry.score = score;
        entry.bestMove = bestMove;
        entry.flag = flag;
    }

}