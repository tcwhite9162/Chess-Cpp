#include <cstdlib>

#include "eval/evaluate.hpp"
#include "eval/eval-helpers.hpp"

namespace Evaluate {

int evaluate(const Board& board) {
    int material = 0;
    int pstSum = 0;

    // material / pst
    for (int square = 0; square < 64; square++) {
        const int piece = board.getPiece(square);
        if (piece == Constants::Piece::EMPTY) continue;

        int val = 0;
        switch (std::abs(piece)) {
            case Constants::Piece::PAWN:   val = Constants::Eval::PAWN_VAL; break;
            case Constants::Piece::KNIGHT: val = Constants::Eval::KNIGHT_VAL; break;
            case Constants::Piece::BISHOP: val = Constants::Eval::BISHOP_VAL; break;
            case Constants::Piece::ROOK:   val = Constants::Eval::ROOK_VAL; break;
            case Constants::Piece::QUEEN:  val = Constants::Eval::QUEEN_VAL; break;
            case Constants::Piece::KING:   val = Constants::Eval::KING_VAL; break;
            default: ;
        }

        material += (piece > 0) ? val : -val;
        pstSum  += pstValue(piece, square);
    }

    // mobility
    const int myMoves  = countMobility(board, board.getTurn());
    const int oppMoves = countMobility(board, -board.getTurn());
    const int mobilityScore = Constants::Eval::MOBILITY_FACTOR * (myMoves - oppMoves);

    // pawn structure
    int pawnScore = 0;
    int files[8] = {0};
    int filesOpp[8] = {0};

    for (int square = 0; square < 64; square++) {
        const int piece = board.getPiece(square);

        if (piece == Constants::Piece::W_PAWN) {
            const int file = square % 8;
            files[file]++;
        }
        else if (piece == Constants::Piece::B_PAWN) {
            const int file = square % 8;
            filesOpp[file]++;
        }
    }

    for (int f = 0; f < 8; f++) {
        if (files[f] > 1) {
            pawnScore -= Constants::Eval::PAWN_PENALTY * (files[f] - 1);
        }
        if (filesOpp[f] > 1) {
            pawnScore += Constants::Eval::PAWN_PENALTY * (filesOpp[f] - 1);
        }
    }

    const int whiteScore = material + pstSum + mobilityScore + pawnScore + kingSafety(board);

    return whiteScore * board.getTurn(); // negative if blacks turn (-1)
}

int pstValue(const int piece, const int square) {
    const int* table = getPiecePstTable(piece);

    if (!table) return 0;

    if (piece > 0)
        return table[square];
    else
        return -table[mirrorSquare(square)];
}

int kingSafety(const Board& board) {
    int score = 0;

    const int wkPos  = board.getWhiteKingPos();
    const int wkFile = wkPos % 8;
    const int wkRank = wkPos / 8;

    if (wkPos == Constants::Castling::WHITE_KINGSIDE_END || wkPos == Constants::Castling::WHITE_QUEENSIDE_END) {
        score += 40;
    }
    else if (wkPos == Constants::Castling::WHITE_KING_START) {
        score -= 20;
    }

    if (wkRank == 7) {
        for (int dFile = -1; dFile <= 1; dFile++) {
            if (const int file = wkFile + dFile; 0 <= file && file < 8) {
                if (const int square = (wkRank - 1) * 8 + file; board.getPiece(square) == Constants::Piece::W_PAWN) {
                    score += 10; }
                else {
                    score -= 10; }
            }
        }
    }

    const int bkPos = board.getBlackKingPos();
    const int bkFile = bkPos % 8;
    const int bkRank = bkPos / 8;

    if (bkPos == Constants::Castling::BLACK_KINGSIDE_END || bkPos == Constants::Castling::BLACK_QUEENSIDE_END) {
        score -= 40;
    }
    else if (bkPos == Constants::Castling::BLACK_KING_START) {
        score += 20;
    }

    if (bkRank == 0) {
        for (int dFile = -1; dFile <= 1; dFile++) {
            if (const int file = bkFile + dFile; 0 <= file && file < 8) {
                if (const int square = (bkRank + 1) * 8 + file; board.getPiece(square) == Constants::Piece::B_PAWN) {
                    score -= 10; }
                else {
                    score += 10; }
            }
        }
    }

    return score;
}

}
