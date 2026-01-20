#include <algorithm>

#include "core/utils.hpp"
#include "movegen/moveGen.hpp"

namespace Search::Ordering {

static int scoreCapture(const Move& m, const Board& board) {
    int victim;
    if (m.isEnPassant()) {
        const int epCaptureSq = board.getEnPassantCaptureSquare();
        victim = getPieceValue(board.getPiece(epCaptureSq));
    } else {
        victim = getPieceValue(board.getPiece(m.to_square()));
    }

    const int attacker = getPieceValue(board.getPiece(m.from_square()));

    return victim * Constants::Eval::MVV_VICTIM_MULT - attacker * Constants::Eval::MVV_ATTACKER_MULT;
}

static int scorePromotion(const Move& m) {
    if (!m.isPromotion()) { return 0; }

    switch (m.promotionPiece()) {
        case Constants::Piece::QUEEN:  return Constants::Eval::PROMO_QUEEN_BONUS;
        case Constants::Piece::ROOK:   return Constants::Eval::PROMO_ROOK_BONUS;
        case Constants::Piece::BISHOP: return Constants::Eval::PROMO_BISHOP_BONUS;
        case Constants::Piece::KNIGHT: return Constants::Eval::PROMO_KNIGHT_BONUS;
        default:     return 0;
    }
}

inline static int scoreQuiet() { return Constants::Eval::QUIET_BASE_SCORE; }

void scoreMoves(MoveList& moves, const Board& board){

    for (int i = 0; i < moves.count; i++) {
        Move& m = moves.moves[i];

        int score = 0;

        if (m.isCapture()) {
            score += scoreCapture(m, board);
        }

        if (m.isPromotion()) {
            score += scorePromotion(m);
        }

        if (m.isQuiet()) {
            score += scoreQuiet();
        }

        m.score = score;
    }


    std::sort(moves.moves, moves.moves + moves.count,
            [](const Move& a, const Move& b){
                return a.score > b.score;
                }
            );
}

}
