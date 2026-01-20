#include "eval/evaluate.hpp"
#include "search/search.hpp"
#include "search/ordering.hpp"
#include "search/quiescence.hpp"

namespace Search {

TranspositionTable TT(Constants::Search::TT_SIZE_MB);

int negamax(Board& board, int depth, int alpha, int beta) {
    const int alphaOrig = alpha;

    if (depth == 0) {
        const int score = QSearch::qSearch(board, alpha, beta);
        // int score = Constants::Eval::evaluate(board);
        return score;
    }

    if (board.isDraw()) {
        return 0;
    }

    const u64 key = board.getZobristKey();
    TTEntry& entry = TT[key];

    if (board.isTerminal()) {
        const int score = Evaluate::evaluate(board);
        updateEntry(entry, key, depth, score, Move::nullMove(), TTFlag::TT_EXACT);

        return score;
    }

    if (entry.key == key && entry.depth >= depth) {
        if (entry.flag == TTFlag::TT_EXACT)
            return entry.score;

        if (entry.flag == TTFlag::TT_LOWER && entry.score > alpha)
            alpha = entry.score;

        if (entry.flag == TTFlag::TT_UPPER && entry.score < beta)
            beta = entry.score;

        if (alpha >= beta)
            return entry.score;
    }

    MoveList legalMoves;
    MoveGen::generateLegalMoves(board, legalMoves);

    if (entry.key == key && entry.bestMove.isValid()) {
        for (int i = 0; i < legalMoves.count; i++) {
            if (legalMoves.moves[i] == entry.bestMove)
                legalMoves.moves[i].score = Constants::Search::TT_MOVE_BONUS;
        }
    }

    if (legalMoves.count == 0) {
        const int score = Evaluate::evaluate(board);
        updateEntry(entry, key, depth, score, Move::nullMove(), TTFlag::TT_EXACT);

        return score;
    }

    Ordering::scoreMoves(legalMoves, board);

    Move bestMove;
    int maxVal = -Constants::Search::INF;

    for (int i = 0; i < legalMoves.count; i++) {
        const Move m = legalMoves.moves[i];

        board.makeMove(m, true);
        int value = -negamax(board, depth - 1, -beta, -alpha);
        board.unmakeMove(true);

        if (value > maxVal) {
            maxVal = value;
            bestMove = m;
        }

        alpha = std::max(alpha, value);
        if (alpha >= beta) {
            updateEntry(entry, key, depth, value, m, TTFlag::TT_LOWER);

            return alpha;
        }
    }

    // store in TT
    const u8 flag = (maxVal <= alphaOrig) ? TTFlag::TT_UPPER : (maxVal >= beta) ? TTFlag::TT_LOWER : TTFlag::TT_EXACT;
    updateEntry(entry, key, depth, maxVal, bestMove, flag);

    return maxVal;
}

SearchResult searchBestMove(Board& board, int depth) {
    int bestScore = -Constants::Search::INF;
    Move bestMove;

    MoveList legalMoves;
    MoveGen::generateLegalMoves(board, legalMoves);
    Ordering::scoreMoves(legalMoves, board);


    for (int i = 0; i < legalMoves.count; i++) {
        const Move m = legalMoves.moves[i];
        board.makeMove(m, true);
        const int score = -negamax(board, depth - 1, -Constants::Search::INF, Constants::Search::INF);
        board.unmakeMove(true);

        if (score > bestScore) {
            bestScore = score;
            bestMove = m;
        }
    }

    return { bestMove, bestScore };
}

}
