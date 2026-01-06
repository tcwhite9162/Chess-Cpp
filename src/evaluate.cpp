#include <cstdlib>

#include "board.hpp"
#include "eval-helpers.hpp"
#include "constants.hpp"
#include "evaluate.hpp"

auto mirrorSquare = [](int x) { return 63 -x; };

// main eval function
int evaluate(Board& board) {
    int material = 0;
    int pstSum = 0;

    // material / pst
    for (int square = 0; square < 64; square++) {
        int piece = board.getPiece(square);
        if (piece == EMPTY) continue;

        int val = 0;
        switch (std::abs(piece)) {
            case PAWN: val = PAWN_VAL; break;
            case KNIGHT: val = KNIGHT_VAL; break;
            case BISHOP: val = BISHOP_VAL; break;
            case ROOK: val = ROOK_VAL; break;
            case QUEEN: val = QUEEN_VAL; break;
            case KING: val = KING_VAL; break;
        }

        material += (piece > 0) ? val : -val;
        pstSum  += pstValue(piece, square);
    }

    // mobility
    int myMoves  = countMobility(board, board.getTurn());
    int oppMoves = countMobility(board, -board.getTurn());
    int mobilityScore = MOBILITY_FACTOR * (myMoves - oppMoves);

    // pawn structure
    int pawnScore = 0;
    int files[8] = {0};
    int filesOpp[8] = {0};

    for (int square = 0; square < 64; square++) {
        int piece = board.getPiece(square);

        if (piece == W_PAWN) {
            int file = square % 8;
            files[file]++;
        }
        else if (piece == B_PAWN) {
            int file = square % 8;
            filesOpp[file]++;
        }
    }

    for (int f = 0; f < 8; f++) {
        if (files[f] > 1) {
            pawnScore -= PAWN_PENALTY * (files[f] - 1);
        } 
        if (filesOpp[f] > 1) { 
            pawnScore += PAWN_PENALTY * (filesOpp[f] - 1);
        }
    }

    int whiteScore = material + pstSum + mobilityScore + pawnScore + kingSafety(board);

    return whiteScore * board.getTurn();
}

int pstValue(int piece, int square) {
    const int* table = nullptr;

    switch(std::abs(piece)) {
        case PAWN:   table = PAWN_PST; break;
        case KNIGHT: table = KNIGHT_PST; break;
        case BISHOP: table = BISHOP_PST; break;
        case ROOK:   table = ROOK_PST; break;
        case QUEEN:  table = QUEEN_PST; break;
        case KING:   table = KING_PST; break;
        default: return 0;
    }

    if (piece > 0) {
        return table[square];
    } 
    else {
        return -table[mirrorSquare(square)];
    }
}

int kingSafety(const Board& board) {
    int score = 0;

    int wkPos  = board.getWhiteKingPos();
    int wkFile = wkPos % 8;
    int wkRank = wkPos / 8;
    
    if (wkPos == WHITE_KINGSIDE_END || wkPos == WHITE_QUEENSIDE_END) { 
        score += 40;
    }
    else if (wkPos == WHITE_KING_START) {
        score -= 20;
    }

    if (wkRank == 7) {
        for (int dFile = -1; dFile <= 1; dFile++) {
            int file = wkFile + dFile;

            if (0 <= file && file < 8) {
                int square = (wkRank - 1) * 8 + file;

                if (board.getPiece(square) == W_PAWN) {
                    score += 10; }
                else {
                    score -= 10; }
            }
        }
    }

    int bkPos = board.getBlackKingPos();
    int bkFile = bkPos % 8;
    int bkRank = bkPos / 8;

    if (bkPos == BLACK_KINGSIDE_END || bkPos == BLACK_QUEENSIDE_END) {
        score -= 40;
    }
    else if (bkPos == BLACK_KING_START) {
        score += 20;
    }

    if (bkRank == 0) {
        for (int dFile = -1; dFile <= 1; dFile++) {
            int file = bkFile + dFile;

            if (0 <= file && file < 8) {
                int square = (bkRank + 1) * 8 + file;
                
                if (board.getPiece(square) == B_PAWN) {
                    score -= 10; }
                else {
                    score += 10; }
            }
        }
    }

    return score;
}


