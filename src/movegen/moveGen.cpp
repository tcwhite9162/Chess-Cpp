#include "bitboard/bitboard.hpp"
#include "constants.hpp"
#include "core/utils.hpp"
#include "movegen/moveGen.hpp"

namespace MoveGen {

void generatePawnMoves(const Board &board, int color, MoveList& moves) {
    const u64 empty = ~board.getOccupancyAll();

    // white pawns
    if (color == Data::Piece::WHITE){
        const u64 pawns = board.getBitboard(PieceType::WP);

        // single pushes
        const u64 singlePush = (pawns >> 8) & empty;

        const u64 promoSingle = singlePush & Data::Board::RANK_8;
        const u64 quietSingle = singlePush & ~Data::Board::RANK_8;

        u64 bb = quietSingle;
        while (bb) {
            const int to = popLeastSigBit(bb);
            const int from = to + 8;
            moves.add(Move(from, to, Flags::FLAG_NONE));
        }

        bb = promoSingle;
        while (bb) {
            const int to = popLeastSigBit(bb);
            const int from = to + 8;
            moves.add(Move(from, to, Flags::FLAG_PROMO_N | Flags::FLAG_PROMOTION));
            moves.add(Move(from, to, Flags::FLAG_PROMO_B | Flags::FLAG_PROMOTION));
            moves.add(Move(from, to, Flags::FLAG_PROMO_R | Flags::FLAG_PROMOTION));
            moves.add(Move(from, to, Flags::FLAG_PROMO_Q | Flags::FLAG_PROMOTION));
        }

        // double pushes
        const u64 doublePush = ((quietSingle >> 8) & empty) & Data::Board::RANK_4;

        bb = doublePush;
        while (bb) {
            const int to = popLeastSigBit(bb);
            const int from = to + 16;
            moves.add(Move(from, to, Flags::FLAG_DOUBLE_PUSH));
        }

        // // captures
        generatePawnCaptures(board, color, moves);
    }

    // black pawns
    else{
        const u64 pawns = board.getBitboard(PieceType::BP);

        // single pushes
        const u64 singlePush = (pawns << 8) & empty;

        // promotions
        const u64 promoSingle = singlePush & Data::Board::RANK_1;
        const u64 quietSingle = singlePush & ~Data::Board::RANK_1;

        // quiet single pushes
        u64 bb = quietSingle;
        while (bb) {
            const int to = popLeastSigBit(bb);
            const int from = to - 8;
            moves.add(Move(from, to, Flags::FLAG_NONE));
        }

        // promotion single pushes
        bb = promoSingle;
        while(bb) {
            const int to = popLeastSigBit(bb);
            const int from = to - 8;
            moves.add(Move(from, to, Flags::FLAG_PROMO_N | Flags::FLAG_PROMOTION));
            moves.add(Move(from, to, Flags::FLAG_PROMO_B | Flags::FLAG_PROMOTION));
            moves.add(Move(from, to, Flags::FLAG_PROMO_R | Flags::FLAG_PROMOTION));
            moves.add(Move(from, to, Flags::FLAG_PROMO_Q | Flags::FLAG_PROMOTION));
        }

        // double pushes
        const u64  doublePush = ((quietSingle << 8) & empty) & Data::Board::RANK_5;

        bb = doublePush;
        while (bb) {
            const int to = popLeastSigBit(bb);
            const int from = to - 16;
            moves.add(Move(from, to, Flags::FLAG_DOUBLE_PUSH));
        }

        // captures
        generatePawnCaptures(board, color, moves);
    }
}

void generateKnightMoves(const Board &board, int color, MoveList& moves) {
    const u64 knights = (color == Data::Piece::WHITE) ? board.getBitboard(PieceType::WN) : board.getBitboard(PieceType::BN);
    const u64 ownPieces = (color == Data::Piece::WHITE) ? board.getOccupancyWhite() : board.getOccupancyBlack();

    u64 bb = knights;
    while (bb) {
        const int from = popLeastSigBit(bb);

        const u64 attacks = knightAttacks[from] & ~ownPieces;

        u64 atk = attacks;
        while (atk) {
            const int to = popLeastSigBit(atk);

            if (board.getPiece(to) == Data::Piece::EMPTY) {
                moves.add(Move(from, to, Flags::FLAG_NONE));
            }else {
                moves.add(Move(from, to, Flags::FLAG_CAPTURE));
            }
        }
    }

}

void generateKingMoves(const Board &board, int color, MoveList& moves) {
    const u64 king = (color == Data::Piece::WHITE) ? board.getBitboard(PieceType::WK) : board.getBitboard(PieceType::BK);
    const u64 ownPieces = (color == Data::Piece::WHITE) ? board.getOccupancyWhite() : board.getOccupancyBlack();

    // finds the king
    const int from = __builtin_ctzll(king);

    const u64 attacks = kingAttacks[from] & ~ownPieces;

    u64 bb = attacks;
    while (bb) {
        int to = popLeastSigBit(bb);

        if (board.getPiece(to) == Data::Piece::EMPTY) {
            moves.add(Move(from, to, Flags::FLAG_NONE));
        } else {
            moves.add(Move(from, to, Flags::FLAG_CAPTURE));
        }
    }

    // castling
    if (color == Data::Piece::WHITE) {
        // white kingside
        if (board.canCastleWhiteKingside()) {
            const bool empty = (board.getPiece(61) == Data::Piece::EMPTY &&
                          board.getPiece(62) == Data::Piece::EMPTY);

            const bool safe = (!board.isSquareAttacked(60, Data::Piece::BLACK) &&
                         !board.isSquareAttacked(61, Data::Piece::BLACK) &&
                         !board.isSquareAttacked(62, Data::Piece::BLACK));

            if (empty && safe)
                moves.add(Move(from, 62, Flags::FLAG_CASTLE_WK));
        }

        // white queenside
        if (board.canCastleWhiteQueenside()) {
            const bool empty = (board.getPiece(59) == Data::Piece::EMPTY &&
                          board.getPiece(58) == Data::Piece::EMPTY &&
                          board.getPiece(57) == Data::Piece::EMPTY);

            const bool safe = (!board.isSquareAttacked(60, Data::Piece::BLACK) &&
                         !board.isSquareAttacked(59, Data::Piece::BLACK) &&
                         !board.isSquareAttacked(58, Data::Piece::BLACK));

            if (empty && safe)
                moves.add(Move(from, 58, Flags::FLAG_CASTLE_WQ));
        }
    } else {
        // black kingside
        if (board.canCastleBlackKingside()) {
            const bool empty = (board.getPiece(5) == Data::Piece::EMPTY &&
                          board.getPiece(6) == Data::Piece::EMPTY);

            const bool safe = (!board.isSquareAttacked(4, Data::Piece::WHITE) &&
                         !board.isSquareAttacked(5, Data::Piece::WHITE) &&
                         !board.isSquareAttacked(6, Data::Piece::WHITE));

            if (empty && safe)
                moves.add(Move(from, 6, Flags::FLAG_CASTLE_BK));
        }

        // black queenside
        if (board.canCastleBlackQueenside()) {
            const bool empty = (board.getPiece(3) == Data::Piece::EMPTY &&
                          board.getPiece(2) == Data::Piece::EMPTY &&
                          board.getPiece(1) == Data::Piece::EMPTY);

            const bool safe = (!board.isSquareAttacked(4, Data::Piece::WHITE) &&
                         !board.isSquareAttacked(3, Data::Piece::WHITE) &&
                         !board.isSquareAttacked(2, Data::Piece::WHITE));

            if (empty && safe)
                moves.add(Move(from, 2, Flags::FLAG_CASTLE_BQ));
        }
    }


}

void generateSlidingMoves(const Board &board, int color, MoveList& moves) {
    const u64 own   = (color == Data::Piece::WHITE) ? board.getOccupancyWhite() : board.getOccupancyBlack();
    const u64 enemy = (color == Data::Piece::WHITE) ? board.getOccupancyBlack() : board.getOccupancyWhite();

    // rooks
    {
        const u64 rooks = (color == Data::Piece::WHITE) ? board.getBitboard(PieceType::WR) : board.getBitboard(PieceType::BR);

        u64 bb = rooks;
        while (bb) {
            const int from = popLeastSigBit(bb);

            // u64 attacks = rookAttacksMagic(from, board.getOccupancyAll()) & ~own;
            const u64 attacks = rookAttacksMagic(from, board.getOccupancyAll()) & ~own;

            u64 atk = attacks;
            while (atk) {
                const int to = popLeastSigBit(atk);
                if (enemy & (1ULL << to)) {
                    moves.add(Move(from, to, Flags::FLAG_CAPTURE));
                } else {
                    moves.add(Move(from, to, Flags::FLAG_NONE));
                }
            }
        }
    }

    // bishops
    {
        const u64 bishops = (color == Data::Piece::WHITE) ? board.getBitboard(PieceType::WB) : board.getBitboard(PieceType::BB);

        u64 bb = bishops;
        while (bb) {
            const int from = popLeastSigBit(bb);

            // u64 attacks = bishopAttacksMagic(from, board.getOccupancyAll()) & ~own;
            u64 attacks = bishopAttacksMagic(from, board.getOccupancyAll()) & ~own;

            u64 atk = attacks;
            while (atk) {
                const int to = popLeastSigBit(atk);
                if (enemy & (1ULL << to)) {
                    moves.add(Move(from, to, Flags::FLAG_CAPTURE));
                } else {
                    moves.add(Move(from, to, Flags::FLAG_NONE));
                }
            }
        }
    }

    // queens
    {
        const u64 queens = (color == Data::Piece::WHITE) ? board.getBitboard(PieceType::WQ) : board.getBitboard(PieceType::BQ);

        u64 bb = queens;
        while (bb) {
            const int from = popLeastSigBit(bb);

            const u64 attacks = (rookAttacksMagic(from, board.getOccupancyAll()) |
                           bishopAttacksMagic(from, board.getOccupancyAll())) & ~own;

            u64 atk = attacks;
            while (atk) {
                const int to = popLeastSigBit(atk);
                if (enemy & (1ULL << to)) {
                    moves.add(Move(from, to, Flags::FLAG_CAPTURE));
                } else {
                    moves.add(Move(from, to, Flags::FLAG_NONE));
                }
            }
        }
    }
}

void generatePseudoLegalMoves(const Board &board, MoveList& moves) {
    const int side = board.getTurn();

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
        const int side = board.getTurn();

        board.makeMove(m, false);
        if (!board.isInCheck(side)) { legalMoves.add(m); }
        board.unmakeMove(m, false);
    }
}

void generatePawnCaptures(const Board& board, int side, MoveList& captures) {
    const u64 pawns = (side == Data::Piece::WHITE) ? board.getBitboard(PieceType::WP) : board.getBitboard(PieceType::BP);
    const u64 enemy = (side == Data::Piece::WHITE) ? board.getOccupancyBlack() : board.getOccupancyWhite();

    if (side == Data::Piece::WHITE) {

        const u64 left  = ((pawns & Data::Board::NOT_FILE_A) >> 9) & enemy;
        const u64 right = ((pawns & Data::Board::NOT_FILE_H) >> 7) & enemy;

        // promotion captures
        const u64 promoLeft  = left  & Data::Board::RANK_8;
        const u64 promoRight = right & Data::Board::RANK_8;

        // normal captures
        const u64 quietLeft  = left  & ~Data::Board::RANK_8;
        const u64 quietRight = right & ~Data::Board::RANK_8;

        // normal left captures
        u64 bb = quietLeft;
        while (bb) {
            const int to = popLeastSigBit(bb);
            const int from = to + 9;
            captures.add(Move(from, to, Flags::FLAG_CAPTURE));
        }

        // normal right captures
        bb = quietRight;
        while (bb) {
            const int to = popLeastSigBit(bb);
            const int from = to + 7;
            captures.add(Move(from, to, Flags::FLAG_CAPTURE));
        }

        // left promotion captures
        bb = promoLeft;
        while (bb) {
            const int to = popLeastSigBit(bb);
            const int from = to + 9;
            captures.add(Move(from, to, Flags::FLAG_PROMO_N | Flags::FLAG_PROMOTION | Flags::FLAG_CAPTURE));
            captures.add(Move(from, to, Flags::FLAG_PROMO_B | Flags::FLAG_PROMOTION | Flags::FLAG_CAPTURE));
            captures.add(Move(from, to, Flags::FLAG_PROMO_R | Flags::FLAG_PROMOTION | Flags::FLAG_CAPTURE));
            captures.add(Move(from, to, Flags::FLAG_PROMO_Q | Flags::FLAG_PROMOTION | Flags::FLAG_CAPTURE));
        }

        // right promotion captures
        bb = promoRight;
        while (bb) {
            const int to = popLeastSigBit(bb);
            const int from = to + 7;
            captures.add(Move(from, to, Flags::FLAG_PROMO_N | Flags::FLAG_PROMOTION | Flags::FLAG_CAPTURE));
            captures.add(Move(from, to, Flags::FLAG_PROMO_B | Flags::FLAG_PROMOTION | Flags::FLAG_CAPTURE));
            captures.add(Move(from, to, Flags::FLAG_PROMO_R | Flags::FLAG_PROMOTION | Flags::FLAG_CAPTURE));
            captures.add(Move(from, to, Flags::FLAG_PROMO_Q | Flags::FLAG_PROMOTION | Flags::FLAG_CAPTURE));
        }

        // en passant
        int ep = board.getEnPassant();
        if (ep != -1) {
            u64 attackers = pawnAttacks[0][ep] & pawns; // white pawns attacking ep
            while (attackers) {
                const int from = popLeastSigBit(attackers);
                captures.add(Move(from, ep, Flags::FLAG_CAPTURE | Flags::FLAG_EN_PASSANT));
            }
        }
    }

    // black
    else {

        const u64 left  = ((pawns & Data::Board::NOT_FILE_A) << 7) & enemy;
        const u64 right = ((pawns & Data::Board::NOT_FILE_H) << 9) & enemy;

        // promotion captures
        const u64 promoLeft  = left  & Data::Board::RANK_1;
        const u64 promoRight = right & Data::Board::RANK_1;

        // normal captures
        const u64 quietLeft  = left  & ~Data::Board::RANK_1;
        const u64 quietRight = right & ~Data::Board::RANK_1;

        // normal left captures
        u64 bb = quietLeft;
        while (bb) {
            const int to = popLeastSigBit(bb);
            const int from = to - 7;
            captures.add(Move(from, to, Flags::FLAG_CAPTURE));
        }

        // normal right captures
        bb = quietRight;
        while (bb) {
            const int to = popLeastSigBit(bb);
            const int from = to - 9;
            captures.add(Move(from, to, Flags::FLAG_CAPTURE));
        }

        // promotions captures left
        bb = promoLeft;
        while (bb) {
            const int to = popLeastSigBit(bb);
            const int from = to - 7;
            captures.add(Move(from, to, Flags::FLAG_PROMO_N | Flags::FLAG_PROMOTION | Flags::FLAG_CAPTURE));
            captures.add(Move(from, to, Flags::FLAG_PROMO_B | Flags::FLAG_PROMOTION | Flags::FLAG_CAPTURE));
            captures.add(Move(from, to, Flags::FLAG_PROMO_R | Flags::FLAG_PROMOTION | Flags::FLAG_CAPTURE));
            captures.add(Move(from, to, Flags::FLAG_PROMO_Q | Flags::FLAG_PROMOTION | Flags::FLAG_CAPTURE));
        }

        // promotion captures right
        bb = promoRight;
        while (bb) {
            const int to = popLeastSigBit(bb);
            const int from = to - 9;
            captures.add(Move(from, to, Flags::FLAG_PROMO_N | Flags::FLAG_PROMOTION | Flags::FLAG_CAPTURE));
            captures.add(Move(from, to, Flags::FLAG_PROMO_B | Flags::FLAG_PROMOTION | Flags::FLAG_CAPTURE));
            captures.add(Move(from, to, Flags::FLAG_PROMO_R | Flags::FLAG_PROMOTION | Flags::FLAG_CAPTURE));
            captures.add(Move(from, to, Flags::FLAG_PROMO_Q | Flags::FLAG_PROMOTION | Flags::FLAG_CAPTURE));

        }

        // en passant
        const int ep = board.getEnPassant();
        if (ep != -1) {
            u64 attackers = pawnAttacks[1][ep] & pawns; // black pawns attacking ep
            while (attackers) {
                const int from = popLeastSigBit(attackers);
                captures.add(Move(from, ep, Flags::FLAG_CAPTURE | Flags::FLAG_EN_PASSANT));
            }
        }
    }
}

void generateKnightCaptures(const Board& board, int side, MoveList& captures) {
    const u64 knights = (side == Data::Piece::WHITE) ? board.getBitboard(PieceType::WN) : board.getBitboard(PieceType::BN);
    const u64 enemy   = (side == Data::Piece::WHITE) ? board.getOccupancyBlack() : board.getOccupancyWhite();

    u64 bb = knights;
    while (bb) {
        const int from = popLeastSigBit(bb);
        const u64 attacks = knightAttacks[from] & enemy;

        u64 atk = attacks;
        while (atk) {
            const int to = popLeastSigBit(atk);
            captures.add(Move(from, to, Flags::FLAG_CAPTURE));
        }
    }
}

void generateSlidingCaptures(const Board& board, int side, MoveList& captures) {
    const u64 enemy = (side == Data::Piece::WHITE) ? board.getOccupancyBlack() : board.getOccupancyWhite();

    // rooks
    {
        const u64 rooks = (side == Data::Piece::WHITE) ? board.getBitboard(PieceType::WR) : board.getBitboard(PieceType::BR);

        u64 bb = rooks;
        while (bb) {
            const int from = popLeastSigBit(bb);
            const u64 attacks = rookAttacksMagic(from, board.getOccupancyAll()) & enemy;

            u64 atk = attacks;
            while (atk) {
                const int to = popLeastSigBit(atk);
                captures.add(Move(from, to, Flags::FLAG_CAPTURE));
            }
        }
    }

    // bishops
    {
        const u64 bishops = (side == Data::Piece::WHITE) ? board.getBitboard(PieceType::WB) : board.getBitboard(PieceType::BB);

        u64 bb = bishops;
        while (bb) {
            const int from = popLeastSigBit(bb);
            const u64 attacks = bishopAttacksMagic(from, board.getOccupancyAll()) & enemy;

            u64 atk = attacks;
            while (atk) {
                const int to = popLeastSigBit(atk);
                captures.add(Move(from, to, Flags::FLAG_CAPTURE));
            }
        }
    }

    // queens
    {
        const u64 queens = (side == Data::Piece::WHITE) ? board.getBitboard(PieceType::WQ) : board.getBitboard(PieceType::BQ);

        u64 bb = queens;
        while (bb) {
            const int from = popLeastSigBit(bb);
            const u64 attacks = (rookAttacksMagic(from, board.getOccupancyAll()) |
                           bishopAttacksMagic(from, board.getOccupancyAll())) & enemy;

            u64 atk = attacks;
            while (atk) {
                const int to = popLeastSigBit(atk);
                captures.add(Move(from, to, Flags::FLAG_CAPTURE));
            }
        }
    }
}

void generateKingCaptures(const Board& board, int side, MoveList& captures) {
    const u64 king = (side == Data::Piece::WHITE) ? board.getBitboard(PieceType::WK) : board.getBitboard(PieceType::BK);
    const u64 enemy = (side == Data::Piece::WHITE) ? board.getOccupancyBlack() : board.getOccupancyWhite();

    const int from = __builtin_ctzll(king);
    const u64 attacks = kingAttacks[from] & enemy;

    u64 bb = attacks;
    while (bb) {
        const int to = popLeastSigBit(bb);
        captures.add(Move(from, to, Flags::FLAG_CAPTURE));
    }
}

void generateCaptures(const Board& board, MoveList& captures) {
    const int side = board.getTurn();

    generatePawnCaptures(board, side, captures);
    generateKnightCaptures(board, side, captures);
    generateSlidingCaptures(board, side, captures);
    generateKingCaptures(board, side, captures);
}

void generateEvasions(Board& board, MoveList& evasions) {
    generateLegalMoves(board, evasions);
}

}