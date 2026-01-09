#include "search.hpp"
#include "core/move.hpp"
#include "core/board.hpp"
#include "movegen/moveGen.hpp"
#include "eval/evaluate.hpp"

int negamax(Board& board, int depth, int alpha, int beta) {
    if (depth == 0 || board.isTerminal()) {
        // TODO: add quiescence search here (when depth == 0)
        return evaluate(board);
    }

    MoveList legalMoves;
    generateLegalMoves(board, legalMoves);
    
    int maxVal = -INF;
    for (int i = 0; i < legalMoves.count; i++) {
        Move m = legalMoves.moves[i];

        board.makeMove(m, true);
        int value = -negamax(board, depth - 1, -beta, -alpha);
        board.unmakeMove(m, true);

        maxVal = std::max(value, maxVal);
        alpha  = std::max(value, alpha);
        if (alpha > beta) break;
    }

    return maxVal;
}

SearchResult searchBestMove(Board& board, int depth) {
    int bestScore = -INF;
    Move bestMove;

    MoveList legalMoves;
    generateLegalMoves(board, legalMoves);

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