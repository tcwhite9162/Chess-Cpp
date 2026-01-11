#include "search/search.hpp"
#include "constants.hpp"
#include "movegen/moveGen.hpp"
#include "eval/evaluate.hpp"
#include "search/ordering.hpp"
#include "search/tt.hpp"

namespace Search {

TranspositionTable TT(TT_SIZE_MB);

int negamax(Board& board, int depth, int alpha, int beta) {
    int alphaOrig = alpha;

    u64 key = board.getZobristKey();
    TTEntry& entry = TT[key];

    if (board.isTerminal()) {
        int score = Eval::evaluate(board);
        updateEntry(entry, key, depth, score, Move::nullMove(), TT_EXACT);

        return score;
    }

    if (depth == 0) {
        int score = Eval::evaluate(board);
        // TODO: qsearch here
        return score;
    }

    if (entry.key == key && entry.depth >= depth) {
        if (entry.flag == TT_EXACT) 
            return entry.score;

        if (entry.flag == TT_LOWER && entry.score > alpha)
            alpha = entry.score;

        if (entry.flag == TT_UPPER && entry.score < beta)
            beta = entry.score;

        if (alpha >= beta)
            return entry.score;
    }

    MoveList legalMoves;
    MoveGen::generateLegalMoves(board, legalMoves);

    if (entry.key == key && entry.bestMove.isValid()) {
        for (int i = 0; i < legalMoves.count; i++) {
            if (legalMoves.moves[i] == entry.bestMove)
                legalMoves.moves[i].score = TT_MOVE_BONUS;
        }
    }

    if (legalMoves.count == 0) {
        int score = Eval::evaluate(board);
        updateEntry(entry, key, depth, score, Move::nullMove(), TT_EXACT);

        return score;
    }

    Ordering::scoreMoves(legalMoves, board);

    Move bestMove;
    int maxVal = -INF;

    for (int i = 0; i < legalMoves.count; i++) {
        Move m = legalMoves.moves[i];

        board.makeMove(m, true);
        int value = -negamax(board, depth - 1, -beta, -alpha);
        board.unmakeMove(m, true);

        if (value > maxVal) {
            maxVal = value;
            bestMove = m;
        }

        alpha = std::max(alpha, value);
        if (alpha >= beta) {
            updateEntry(entry, key, depth, value, m, TT_LOWER);

            return alpha;
        }
    }

    // store in TT
    u8 flag = (maxVal <= alphaOrig) ? TT_UPPER : (maxVal >= beta) ? TT_LOWER : TT_EXACT;
    updateEntry(entry, key, depth, maxVal, bestMove, flag);

    return maxVal;
}

SearchResult searchBestMove(Board& board, int depth) {
    int bestScore = -INF;
    Move bestMove;

    MoveList legalMoves;
    MoveGen::generateLegalMoves(board, legalMoves);
    Search::Ordering::scoreMoves(legalMoves, board);
    

    for (int i = 0; i < legalMoves.count; i++) {
        Move m = legalMoves.moves[i];
        board.makeMove(m, true);
        int score = -negamax(board, depth - 1, -INF, INF);
        board.unmakeMove(m, true);

        if (score > bestScore) {
            bestScore = score;
            bestMove = m;
        }
    }

    return { bestMove, bestScore };
}

}