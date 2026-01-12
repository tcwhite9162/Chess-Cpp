#include "search/quiescence.hpp"
#include "search/ordering.hpp"
#include "eval/evaluate.hpp"
#include "movegen/moveGen.hpp"

namespace Search::QSearch {

    int qSearch(Board& board, int alpha, int beta) {

        if (board.isInCheck(board.getTurn())) {
            MoveList evasions;
            MoveGen::generateEvasions(board, evasions);

            int best = -INF;

            for (int i = 0; i < evasions.count; i++) {
                Move m = evasions.moves[i];

                board.makeMove(m, true);
                int score = -qSearch(board, -beta, -alpha);
                board.unmakeMove(m, true);

                if (score >= beta)
                    return beta;

                if (score > best)
                    best = score;

                if (score > alpha)
                    alpha = score;
            }

            return alpha;
        }

        int standPat = Eval::evaluate(board);

        if (standPat  >= beta)
            return beta;

        if (standPat > alpha)
            alpha = standPat;

        MoveList moves;
        MoveGen::generateCaptures(board, moves);

        Ordering::scoreMoves(moves, board);

        for (int i = 0; i < moves.count; i++) {
            Move m = moves.moves[i];

            board.makeMove(m, true);
            int score = -qSearch(board, -beta, -alpha);
            board.unmakeMove(m, true);

            if (score >= beta)
                return beta;

            if (score > alpha)
                alpha = score;
        }

        return alpha;
    }

}