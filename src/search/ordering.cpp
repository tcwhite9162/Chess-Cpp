#include <algorithm>

#include "core/utils.hpp"
#include "movegen/moveGen.hpp"

namespace Search::Ordering {

static int scoreCapture(const Move& m, const Board& board) {
    int victim;
    if (m.isEnPassant()) {
        int epCaptureSq = board.getEnPassantCaptureSquare();
        victim = getPieceValue(board.getPiece(epCaptureSq));
    } else {
        victim = getPieceValue(board.getPiece(m.to_square()));
    }

    int attacker = getPieceValue(board.getPiece(m.from_square()));

    return victim * Data::Eval::MVV_VICTIM_MULT - attacker * Data::Eval::MVV_ATTACKER_MULT;
}

static int scorePromotion(const Move& m) {
    if (!m.isPromotion()) { return 0; }

    switch (m.promotionPiece()) {
        case Data::Piece::QUEEN:  return Data::Eval::PROMO_QUEEN_BONUS;
        case Data::Piece::ROOK:   return Data::Eval::PROMO_ROOK_BONUS;
        case Data::Piece::BISHOP: return Data::Eval::PROMO_BISHOP_BONUS;
        case Data::Piece::KNIGHT: return Data::Eval::PROMO_KNIGHT_BONUS;
        default:     return 0;
    }
}

inline static int scoreQuiet(const Move& m) { return Data::Eval::QUIET_BASE_SCORE; }

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
            score += scoreQuiet(m);
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
