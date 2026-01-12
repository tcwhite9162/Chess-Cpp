#include "bitboard/bitboard.hpp"
#include "constants.hpp"
#include "core/utils.hpp"
#include "movegen/moveGen.hpp"

namespace MoveGen {

void generatePawnMoves(const Board &board, int color, MoveList& moves) {
    u64 empty = ~board.occupancyAll;

    // white pawns
    if (color == WHITE){
        u64 pawns = board.whitePawns;
        
        // single pushes
        u64 singlePush = (pawns >> 8) & empty;

        u64 promoSingle = singlePush & RANK_8;
        u64 quietSingle = singlePush & ~RANK_8;

        u64 bb = quietSingle;
        while (bb) {
            int to = popLeastSigBit(bb);
            int from = to + 8;
            moves.add(Move(from, to, FLAG_NONE));
        }

        bb = promoSingle;
        while (bb) {
            int to = popLeastSigBit(bb);
            int from = to + 8;
            moves.add(Move(from, to, FLAG_PROMO_N | FLAG_PROMOTION));
            moves.add(Move(from, to, FLAG_PROMO_B | FLAG_PROMOTION));
            moves.add(Move(from, to, FLAG_PROMO_R | FLAG_PROMOTION));
            moves.add(Move(from, to, FLAG_PROMO_Q | FLAG_PROMOTION));
        }

        // double pushes
        u64 doublePush = ((quietSingle >> 8) & empty) & RANK_4;

        bb = doublePush;
        while (bb) {
            int to = popLeastSigBit(bb);
            int from = to + 16;
            moves.add(Move(from, to, FLAG_DOUBLE_PUSH));
        }

        // // captures
        generatePawnCaptures(board, color, moves);
    }

    
    // black pawns
    else{
        u64 pawns = board.blackPawns;

        // single pushes
        u64 singlePush = (pawns << 8) & empty;

        // promotions
        u64 promoSingle = singlePush & RANK_1;
        u64 quietSingle = singlePush & ~RANK_1;

        // quiet single pushes
        u64 bb = quietSingle;
        while (bb) {
            int to = popLeastSigBit(bb);
            int from = to - 8;
            moves.add(Move(from, to, FLAG_NONE));
        }

        // promotion single pushes
        bb = promoSingle;
        while(bb) {
            int to = popLeastSigBit(bb);
            int from = to - 8;
            moves.add(Move(from, to, FLAG_PROMO_N | FLAG_PROMOTION));
            moves.add(Move(from, to, FLAG_PROMO_B | FLAG_PROMOTION));
            moves.add(Move(from, to, FLAG_PROMO_R | FLAG_PROMOTION));
            moves.add(Move(from, to, FLAG_PROMO_Q | FLAG_PROMOTION));
        }

        // double pushes
        u64  doublePush = ((quietSingle << 8) & empty) & RANK_5;

        bb = doublePush;
        while (bb) {
            int to = popLeastSigBit(bb);
            int from = to - 16;
            moves.add(Move(from, to, FLAG_DOUBLE_PUSH));
        }

        // captures
        generatePawnCaptures(board, color, moves);
    }
}

void generateKnightMoves(const Board &board, int color, MoveList& moves) {
    u64 knights = (color == WHITE) ? board.whiteKnights : board.blackKnights;
    u64 ownPieces = (color == WHITE) ? board.occupancyWhite : board.occupancyBlack;

    u64 bb = knights;
    while (bb) {
        int from = popLeastSigBit(bb);

        u64 attacks = knightAttacks[from] & ~ownPieces;

        u64 atk = attacks;
        while (atk) {
            int to = popLeastSigBit(atk);
            
            if (board.getPiece(to) == EMPTY) {
                moves.add(Move(from, to, FLAG_NONE));
            }else {
                moves.add(Move(from, to, FLAG_CAPTURE));
            }
        }
    }

}

void generateKingMoves(const Board &board, int color, MoveList& moves) {
    u64 king = (color == WHITE) ? board.whiteKing : board.blackKing;
    u64 ownPieces = (color == WHITE) ? board.occupancyWhite : board.occupancyBlack;

    // finds the king
    int from = __builtin_ctzll(king);

    u64 attacks = kingAttacks[from] & ~ownPieces;

    u64 bb = attacks;
    while (bb) {
        int to = popLeastSigBit(bb);

        if (board.getPiece(to) == EMPTY) {
            moves.add(Move(from, to, FLAG_NONE));
        } else {
            moves.add(Move(from, to, FLAG_CAPTURE));
        }
    }

    // castling
    if (color == WHITE) {
        // white kingside
        if (board.canCastleWhiteKingside()) {
            bool empty = (board.getPiece(61) == EMPTY &&
                          board.getPiece(62) == EMPTY);

            bool safe = (!board.isSquareAttacked(60, BLACK) &&
                         !board.isSquareAttacked(61, BLACK) &&
                         !board.isSquareAttacked(62, BLACK));

            if (empty && safe)
                moves.add(Move(from, 62, FLAG_CASTLE_WK));
        }

        // white queenside
        if (board.canCastleWhiteQueenside()) {
            bool empty = (board.getPiece(59) == EMPTY &&
                          board.getPiece(58) == EMPTY &&
                          board.getPiece(57) == EMPTY);

            bool safe = (!board.isSquareAttacked(60, BLACK) &&
                         !board.isSquareAttacked(59, BLACK) &&
                         !board.isSquareAttacked(58, BLACK));

            if (empty && safe)
                moves.add(Move(from, 58, FLAG_CASTLE_WQ));
        }
    } else {
        // black kingside
        if (board.canCastleBlackKingside()) {
            bool empty = (board.getPiece(5) == EMPTY &&
                          board.getPiece(6) == EMPTY);

            bool safe = (!board.isSquareAttacked(4, WHITE) &&
                         !board.isSquareAttacked(5, WHITE) &&
                         !board.isSquareAttacked(6, WHITE));

            if (empty && safe)
                moves.add(Move(from, 6, FLAG_CASTLE_BK));
        }

        // black queenside
        if (board.canCastleBlackQueenside()) {
            bool empty = (board.getPiece(3) == EMPTY &&
                          board.getPiece(2) == EMPTY &&
                          board.getPiece(1) == EMPTY);

            bool safe = (!board.isSquareAttacked(4, WHITE) &&
                         !board.isSquareAttacked(3, WHITE) &&
                         !board.isSquareAttacked(2, WHITE));

            if (empty && safe)
                moves.add(Move(from, 2, FLAG_CASTLE_BQ));
        }
    }


}

void generateSlidingMoves(const Board &board, int color, MoveList& moves) {
    u64 own   = (color == WHITE) ? board.occupancyWhite : board.occupancyBlack;
    u64 enemy = (color == WHITE) ? board.occupancyBlack : board.occupancyWhite;
    
    // rooks
    {
        u64 rooks = (color == WHITE) ? board.whiteRooks : board.blackRooks;

        u64 bb = rooks;
        while (bb) {
            int from = popLeastSigBit(bb);

            // u64 attacks = rookAttacksMagic(from, board.occupancyAll) & ~own;
            u64 attacks = rookAttacksMagic(from, board.occupancyAll) & ~own;

            u64 atk = attacks;
            while (atk) {
                int to = popLeastSigBit(atk);
                if (enemy & (1ULL << to)) {
                    moves.add(Move(from, to, FLAG_CAPTURE));
                } else {
                    moves.add(Move(from, to, FLAG_NONE));
                }
            }
        }
    }

    // bishops
    {
        u64 bishops = (color == WHITE) ? board.whiteBishops : board.blackBishops;

        u64 bb = bishops;
        while (bb) {
            int from = popLeastSigBit(bb);

            // u64 attacks = bishopAttacksMagic(from, board.occupancyAll) & ~own;
            u64 attacks = bishopAttacksMagic(from, board.occupancyAll) & ~own;

            u64 atk = attacks;
            while (atk) {
                int to = popLeastSigBit(atk);
                if (enemy & (1ULL << to)) {
                    moves.add(Move(from, to, FLAG_CAPTURE));
                } else {
                    moves.add(Move(from, to, FLAG_NONE));
                }
            }
        }
    }

    // queens
    {
        u64 queens = (color == WHITE) ? board.whiteQueens : board.blackQueens;

        u64 bb = queens;
        while (bb) {
            int from = popLeastSigBit(bb);

            u64 attacks = (rookAttacksMagic(from, board.occupancyAll) | 
                           bishopAttacksMagic(from, board.occupancyAll)) & ~own;

            u64 atk = attacks;
            while (atk) {
                int to = popLeastSigBit(atk);
                if (enemy & (1ULL << to)) {
                    moves.add(Move(from, to, FLAG_CAPTURE));
                } else {
                    moves.add(Move(from, to, FLAG_NONE));
                }
            }
        }
    }
}

void generatePseudoLegalMoves(const Board &board, MoveList& moves) {
    int side = board.getTurn();

    generatePawnMoves(board, side, moves);
    generateKnightMoves(board, side, moves);
    generateKingMoves(board, side, moves);
    generateSlidingMoves(board, side, moves);
}


void generateLegalMoves(Board &board, MoveList& legalMoves) {
    MoveList pseudoLegal;
    generatePseudoLegalMoves(board, pseudoLegal);

    for (int i = 0; i < pseudoLegal.count; i++) {
        Move m = pseudoLegal.moves[i];
        int side = board.getTurn();

        board.makeMove(m, false);
        if (!board.isInCheck(side)) { legalMoves.add(m); }
        board.unmakeMove(m, false);
    }
}

void generatePawnCaptures(const Board& board, int side, MoveList& captures) {
    u64 pawns = (side == WHITE) ? board.whitePawns : board.blackPawns;
    u64 enemy = (side == WHITE) ? board.occupancyBlack : board.occupancyWhite;

    if (side == WHITE) {

        u64 left  = ((pawns & NOT_FILE_A) >> 9) & enemy;
        u64 right = ((pawns & NOT_FILE_H) >> 7) & enemy;

        // promotion captures
        u64 promoLeft  = left  & RANK_8;
        u64 promoRight = right & RANK_8;

        // normal captures
        u64 quietLeft  = left  & ~RANK_8;
        u64 quietRight = right & ~RANK_8;

        // normal left captures
        u64 bb = quietLeft;
        while (bb) {
            int to = popLeastSigBit(bb);
            int from = to + 9;
            captures.add(Move(from, to, FLAG_CAPTURE));
        }

        // normal right captures
        bb = quietRight;
        while (bb) {
            int to = popLeastSigBit(bb);
            int from = to + 7;
            captures.add(Move(from, to, FLAG_CAPTURE));
        }

        // left promotion captures
        bb = promoLeft;
        while (bb) {
            int to = popLeastSigBit(bb);
            int from = to + 9;
            captures.add(Move(from, to, FLAG_PROMO_N | FLAG_PROMOTION | FLAG_CAPTURE));
            captures.add(Move(from, to, FLAG_PROMO_B | FLAG_PROMOTION | FLAG_CAPTURE));
            captures.add(Move(from, to, FLAG_PROMO_R | FLAG_PROMOTION | FLAG_CAPTURE));
            captures.add(Move(from, to, FLAG_PROMO_Q | FLAG_PROMOTION | FLAG_CAPTURE));
        }

        // right promotion captures
        bb = promoRight;
        while (bb) {
            int to = popLeastSigBit(bb);
            int from = to + 7;
            captures.add(Move(from, to, FLAG_PROMO_N | FLAG_PROMOTION | FLAG_CAPTURE));
            captures.add(Move(from, to, FLAG_PROMO_B | FLAG_PROMOTION | FLAG_CAPTURE));
            captures.add(Move(from, to, FLAG_PROMO_R | FLAG_PROMOTION | FLAG_CAPTURE));
            captures.add(Move(from, to, FLAG_PROMO_Q | FLAG_PROMOTION | FLAG_CAPTURE));
        }

        // en passant
        int ep = board.getEnPassant();
        if (ep != -1) {
            u64 attackers = pawnAttacks[0][ep] & pawns; // white pawns attacking ep
            while (attackers) {
                int from = popLeastSigBit(attackers);
                captures.add(Move(from, ep, FLAG_CAPTURE | FLAG_EN_PASSANT));
            }
        }
    }

    // black
    else {

        u64 left  = ((pawns & NOT_FILE_A) << 7) & enemy;
        u64 right = ((pawns & NOT_FILE_H) << 9) & enemy;

        // promotion captures
        u64 promoLeft  = left  & RANK_1;
        u64 promoRight = right & RANK_1;

        // normal captures
        u64 quietLeft  = left  & ~RANK_1;
        u64 quietRight = right & ~RANK_1;

        // normal left captures
        u64 bb = quietLeft;
        while (bb) {
            int to = popLeastSigBit(bb);
            int from = to - 7;
            captures.add(Move(from, to, FLAG_CAPTURE));
        }

        // normal right captures
        bb = quietRight;
        while (bb) {
            int to = popLeastSigBit(bb);
            int from = to - 9;
            captures.add(Move(from, to, FLAG_CAPTURE));
        }

        // promotions captures left
        bb = promoLeft;
        while (bb) {
            int to = popLeastSigBit(bb);
            int from = to - 7;
            captures.add(Move(from, to, FLAG_PROMO_N | FLAG_PROMOTION | FLAG_CAPTURE));
            captures.add(Move(from, to, FLAG_PROMO_B | FLAG_PROMOTION | FLAG_CAPTURE));
            captures.add(Move(from, to, FLAG_PROMO_R | FLAG_PROMOTION | FLAG_CAPTURE));
            captures.add(Move(from, to, FLAG_PROMO_Q | FLAG_PROMOTION | FLAG_CAPTURE));
        }

        // promotion captures right
        bb = promoRight;
        while (bb) {
            int to = popLeastSigBit(bb);
            int from = to - 9;
            captures.add(Move(from, to, FLAG_PROMO_N | FLAG_PROMOTION | FLAG_CAPTURE));
            captures.add(Move(from, to, FLAG_PROMO_B | FLAG_PROMOTION | FLAG_CAPTURE));
            captures.add(Move(from, to, FLAG_PROMO_R | FLAG_PROMOTION | FLAG_CAPTURE));
            captures.add(Move(from, to, FLAG_PROMO_Q | FLAG_PROMOTION | FLAG_CAPTURE));

        }

        // en passant
        int ep = board.getEnPassant();
        if (ep != -1) {
            u64 attackers = pawnAttacks[1][ep] & pawns; // black pawns attacking ep
            while (attackers) {
                int from = popLeastSigBit(attackers);
                captures.add(Move(from, ep, FLAG_CAPTURE | FLAG_EN_PASSANT));
            }
        }
    }
}

void generateKnightCaptures(const Board& board, int side, MoveList& captures) {
    u64 knights = (side == WHITE) ? board.whiteKnights : board.blackKnights;
    u64 enemy   = (side == WHITE) ? board.occupancyBlack : board.occupancyWhite;
    
    u64 bb = knights;
    while (bb) {
        int from = popLeastSigBit(bb);
        u64 attacks = knightAttacks[from] & enemy;

        u64 atk = attacks;
        while (atk) {
            int to = popLeastSigBit(atk);
            captures.add(Move(from, to, FLAG_CAPTURE));
        }
    }
}

void generateSlidingCaptures(const Board& board, int side, MoveList& captures) {
    u64 enemy = (side == WHITE) ? board.occupancyBlack : board.occupancyWhite;
    
    // rooks
    {
        u64 rooks = (side == WHITE) ? board.whiteRooks : board.blackRooks;

        u64 bb = rooks;
        while (bb) {
            int from = popLeastSigBit(bb);
            u64 attacks = rookAttacksMagic(from, board.occupancyAll) & enemy;

            u64 atk = attacks;
            while (atk) {
                int to = popLeastSigBit(atk);
                captures.add(Move(from, to, FLAG_CAPTURE));
            }
        }
    }

    // bishops
    {
        u64 bishops = (side == WHITE) ? board.whiteBishops : board.blackBishops;

        u64 bb = bishops;
        while (bb) {
            int from = popLeastSigBit(bb);
            u64 attacks = bishopAttacksMagic(from, board.occupancyAll) & enemy;

            u64 atk = attacks;
            while (atk) {
                int to = popLeastSigBit(atk);
                captures.add(Move(from, to, FLAG_CAPTURE));
            }
        }
    }

    // queens
    {
        u64 queens = (side == WHITE) ? board.whiteQueens : board.blackQueens;

        u64 bb = queens;
        while (bb) {
            int from = popLeastSigBit(bb);
            u64 attacks = (rookAttacksMagic(from, board.occupancyAll) | 
                           bishopAttacksMagic(from, board.occupancyAll)) & enemy;

            u64 atk = attacks;
            while (atk) {
                int to = popLeastSigBit(atk);
                captures.add(Move(from, to, FLAG_CAPTURE));
            }
        }
    }
}

void generateKingCaptures(const Board& board, int side, MoveList& captures) {
    u64 king = (side == WHITE) ? board.whiteKing : board.blackKing;
    u64 enemy = (side == WHITE) ? board.occupancyBlack : board.occupancyWhite;

    int from = __builtin_ctzll(king);
    u64 attacks = kingAttacks[from] & enemy;

    u64 bb = attacks;
    while (bb) {
        int to = popLeastSigBit(bb);
        captures.add(Move(from, to, FLAG_CAPTURE));
    }
}

void generateCaptures(const Board& board, MoveList& captures) {
    int side = board.getTurn();

    generatePawnCaptures(board, side, captures);
    generateKnightCaptures(board, side, captures);
    generateSlidingCaptures(board, side, captures);
    generateKingCaptures(board, side, captures);
}

void generateEvasions(Board& board, MoveList& evasions) {
    generateLegalMoves(board, evasions);
}

}