#include "moveGen.hpp"
#include "../data/constants.hpp"
#include "move.hpp"
#include "utils.hpp"

void generatePawnMoves(const Board &board, int square, MoveList& moves) {
    int piece = board.getPiece(square);
    if (piece != W_PAWN && piece != B_PAWN) return;

    int direction = (piece == W_PAWN ? UP : DOWN);
    int startRank = (piece == W_PAWN ? WHITE_PAWN_START : BLACK_PAWN_START);

    int rank = square / 8;
    int forwardOne = square + direction;

    if (forwardOne >= 0 && forwardOne < 64 && board.getPiece(forwardOne) == EMPTY) {
        // check for promotion
        int toRank = forwardOne / 8;
        if ((piece > 0 && toRank == WHITE_PROMOTION_RANK) || 
            (piece < 0 && toRank == BLACK_PROMOTION_RANK)) {
                moves.add(Move(square, forwardOne, FLAG_PROMO_N | FLAG_PROMOTION));
                moves.add(Move(square, forwardOne, FLAG_PROMO_B | FLAG_PROMOTION));
                moves.add(Move(square, forwardOne, FLAG_PROMO_R | FLAG_PROMOTION));
                moves.add(Move(square, forwardOne, FLAG_PROMO_Q | FLAG_PROMOTION));
            
        }
        else {
            moves.add(Move(square, forwardOne, FLAG_NONE));
        }

        if (rank == startRank) {
            int forwardTwo = forwardOne + direction;
            if (forwardTwo >= 0 && forwardTwo < 64 && board.getPiece(forwardTwo) == EMPTY) {
                moves.add(Move(square, forwardTwo, FLAG_DOUBLE_PUSH));
            }
        }
    }

    const int captureOffsets[2] = {direction + 1, direction - 1};
    for (int offset : captureOffsets) {
        int captureSquare = square + offset;
        if (captureSquare < 0 || captureSquare >= 64) continue;
        
        int file = square % 8;
        int captureFile = captureSquare % 8;
        if (captureFile != file + 1 && captureFile != file - 1) continue;

        int target = board.getPiece(captureSquare);
        if (target != EMPTY && isOpponent(piece, target)) {
            
            int captureRank = captureSquare / 8;
            if (( piece > 0 && captureRank == WHITE_PROMOTION_RANK ) ||
                ( piece < 0 && captureRank == BLACK_PROMOTION_RANK )){
                    moves.add(Move(square, captureSquare, FLAG_PROMO_N | FLAG_PROMOTION | FLAG_CAPTURE));
                    moves.add(Move(square, captureSquare, FLAG_PROMO_B | FLAG_PROMOTION | FLAG_CAPTURE));
                    moves.add(Move(square, captureSquare, FLAG_PROMO_R | FLAG_PROMOTION | FLAG_CAPTURE));
                    moves.add(Move(square, captureSquare, FLAG_PROMO_Q | FLAG_PROMOTION | FLAG_CAPTURE));
            }else {
                moves.add(Move(square, captureSquare, FLAG_CAPTURE));
            }

        }
        else if ((captureSquare == board.getEnPassant()) && board.enPassantAvailable()) {
            moves.add(Move(square, captureSquare, FLAG_CAPTURE | FLAG_EN_PASSANT));
        }
    }
}

void generateKnightMoves(const Board &board, int square, MoveList& moves) {
    int piece = board.getPiece(square);

    for (int off : KNIGHT_DIRS) {
        int target = square + off;

        int fromFile = square % 8;
        int toFile   = target % 8;
        if (std::abs(fromFile - toFile) > 2) continue;

        if (target >= 0 && target < 64) {
            int targetPiece = board.getPiece(target);
            if (targetPiece == EMPTY) {
                moves.add(Move(square, target, FLAG_NONE));
            } else if ((piece < 0 && targetPiece > 0) || (piece > 0 && targetPiece < 0)) {
                moves.add(Move(square, target, FLAG_CAPTURE));
            }
        }
    }
}

void generateKingMoves(const Board &board, int square, MoveList& moves) {
    int piece = board.getPiece(square);

    for (int off : KING_DIRS) {
        int target = square + off;

        int fromFile = square % 8;
        int toFile   = target % 8;
        if (std::abs(fromFile - toFile) > 1) continue;

        if (target >= 0 && target < 64) {
            int targetPiece = board.getPiece(target);
            if (targetPiece == EMPTY) {
                moves.add(Move(square, target, FLAG_NONE));
            } else if ((piece < 0 && targetPiece > 0) || (piece > 0 && targetPiece < 0)) {
                moves.add(Move(square, target, FLAG_CAPTURE));
            }
        }
    }

    // white kingside castle
    if (board.canCastleWhiteKingside()) {
        bool empty = true;
        for (int sq : WHITE_KINGSIDE_EMPTY) {
            if (board.getPiece(sq) != EMPTY) { empty = false; break; }
        }
        bool safe = true;
        for (int sq : WHITE_KINGSIDE_PATH) {
            if (board.isSquareAttacked(sq, BLACK)) { safe = false; break; }
        }
        if (empty && safe) {
            moves.add(Move(square, WHITE_KINGSIDE_END, FLAG_CASTLE_WK));
        }
    }

    // white queenside castle
    if (board.canCastleWhiteQueenside()) {
        bool empty = true;
        for (int sq : WHITE_QUEENSIDE_EMPTY) {
            if (board.getPiece(sq) != EMPTY) { empty = false; break; }
        }
        bool safe = true;
        for (int sq : WHITE_QUEENSIDE_PATH) {
            if (board.isSquareAttacked(sq, BLACK)) { safe = false; break; }
        }
        if (empty && safe) {
            moves.add(Move(square, WHITE_QUEENSIDE_END, FLAG_CASTLE_WQ));
        }
    }

    // black kingside castle
    if (board.canCastleBlackKingside()) {
        bool empty = true;
        for (int sq : BLACK_KINGSIDE_EMPTY) {
            if (board.getPiece(sq) != EMPTY) { empty = false; break; }
        }
        bool safe = true;
        for (int sq : BLACK_KINGSIDE_PATH) {
            if (board.isSquareAttacked(sq, WHITE)) { safe = false; break; }
        }
        if (empty && safe) {
            moves.add(Move(square, BLACK_KINGSIDE_END, FLAG_CASTLE_BK));
        }
    }

    // black queenside castle
    if (board.canCastleBlackQueenside()) {
        bool empty = true;
        for (int sq : BLACK_QUEENSIDE_EMPTY) {
            if (board.getPiece(sq) != EMPTY) { empty = false; break; }
        }
        bool safe = true;
        for (int sq : BLACK_QUEENSIDE_PATH) {
            if (board.isSquareAttacked(sq, WHITE)) { safe = false; break; }
        }
        if (empty && safe) {
            moves.add(Move(square, BLACK_QUEENSIDE_END, FLAG_CASTLE_BQ));
        }
    }
}

void generateSlidingMoves(const Board &board, int square, MoveList& moves) {
    int piece = board.getPiece(square);

    // Rook-like (orthogonal)
    if (piece == W_ROOK || piece == B_ROOK || piece == W_QUEEN || piece == B_QUEEN){
        for (int dir : ROOK_DIRS) {
            int target = square;
            while (true) {
                int prev = target;
                target += dir;
                if (target < 0 || target >= 64) break;

                // Horizontal wrap guard: for left/right, file must change by exactly 1
                if (dir == -1 || dir == 1) {
                    int prevFile = prev % 8;
                    int toFile   = target % 8;
                    if (std::abs(toFile - prevFile) != 1) break;
                }

                int targetPiece = board.getPiece(target);
                if (targetPiece == EMPTY) {
                    moves.add(Move(square, target, FLAG_NONE));
                } else {
                    if (isOpponent(piece, targetPiece)) {
                        moves.add(Move(square, target, FLAG_CAPTURE));
                    }
                    break; // stop ray at first blocker
                }
            }
        }
    }

    // Bishop-like (diagonal)
    if (piece == W_BISHOP || piece == B_BISHOP || piece == W_QUEEN || piece == B_QUEEN){
        for (int dir : BISHOP_DIRS) {
            int target = square;
            while (true) {
                int prev = target;
                target += dir;
                if (target < 0 || target >= 64) break;

                // Diagonal wrap guard: file must change by exactly 1 each step
                int prevFile = prev % 8;
                int toFile   = target % 8;
                if (std::abs(toFile - prevFile) != 1) break;

                int targetPiece = board.getPiece(target);
                if (targetPiece == EMPTY) {
                    moves.add(Move(square, target, FLAG_NONE));
                } else {
                    if (isOpponent(piece, targetPiece)) {
                        moves.add(Move(square, target, FLAG_CAPTURE));
                    }
                    break;
                }
            }
        }
    }
}


void generatePseudoLegalMoves(const Board &board, MoveList& moves) {
    for (int square = 0; square < 64; square++) {
        int piece = board.getPiece(square);
        if (piece == EMPTY) continue;
        if (board.getTurn() == WHITE && piece < 0) continue;
        if (board.getTurn() == BLACK && piece > 0) continue;

        switch(std::abs(piece)) {
            case PAWN: generatePawnMoves(board, square, moves); break;
            case KNIGHT: generateKnightMoves(board, square, moves); break;
            case KING: generateKingMoves(board, square, moves); break;
            default: generateSlidingMoves(board, square, moves); break;
        }
    }

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

int countMobility(const Board& board, int side) {
    int count = 0;

    for (int square = 0; square < 64; square++) {
        int piece = board.getPiece(square);
        if (piece == EMPTY) continue;
        if ((side == WHITE && piece < 0) || (side == BLACK && piece > 0)) continue;

        switch (std::abs(piece)) {
            case PAWN:   count += countPawnMobility(board,   square, piece); break;
            case KNIGHT: count += countKnightMobility(board, square, piece); break;
            case BISHOP: count += countBishopMobility(board, square, piece); break;
            case ROOK:   count += countRookMobility(board,   square, piece); break;
            case QUEEN:  count += countQueenMobility(board,  square, piece); break;
            case KING:   count += countKingMobility(board,   square, piece); break;
        }
    }

    return count;
}

int countPawnMobility(const Board& board, int square, int piece) {
    int count = 0;
    int direction = (piece == W_PAWN ? UP : DOWN);

    int forwardOne = square + direction;
    if (forwardOne >= 0 && forwardOne < 64 && board.getPiece(forwardOne) == EMPTY) {
        count++;

        int rank = square / 8;
        int startRank = (piece == W_PAWN ? WHITE_PAWN_START : BLACK_PAWN_START);
        if (rank == startRank) {
            int forwardTwo = forwardOne + direction;
            if (forwardTwo >= 0 && forwardTwo < 64 && board.getPiece(forwardTwo) == EMPTY) {
                count++;
            }
        }
    }

    const int captureOffsets[2] = {direction - 1, direction + 1};
    for (int offset : captureOffsets) {
        int captureSquare = square + offset;
        if (captureSquare < 0 || captureSquare >= 64) continue;

        int file = square % 8;
        int captureFile = captureSquare % 8;
        if (captureFile != file + 1 && captureFile != captureFile - 1) continue;

        int target = board.getPiece(captureSquare);
        if ((target != EMPTY && isOpponent(piece, target)) || 
            (captureSquare == board.getEnPassant() && board.enPassantAvailable())) {
            count++;
        }
    }

    return count;
}

int countKnightMobility(const Board& board, int square, int piece) {
    int count = 0;

    for (int off : KNIGHT_DIRS) {
        int target = square + off;
        
        int fromFile = square % 8;
        int toFile = target % 8;
        if (std::abs(fromFile - toFile) > 2) continue;
        
        if (target >= 0 && target < 64) {
            int targetPiece = board.getPiece(target);
            if (targetPiece == EMPTY || isOpponent(piece, targetPiece)) {
                count++;
            }
        }
    }

    return count;
}

int countBishopMobility(const Board& board, int square, int piece) {
    int count = 0;
    
    for (int dir : BISHOP_DIRS) {
        int target = square;
        while (true) {
            int prev = target;
            target += dir;
            if (target < 0 || target >= 64) break;
            
            // Diagonal wrap guard
            int prevFile = prev % 8;
            int toFile = target % 8;
            if (std::abs(toFile - prevFile) != 1) break;
            
            int targetPiece = board.getPiece(target);
            if (targetPiece == EMPTY) {
                count++;
            } else {
                if (isOpponent(piece, targetPiece)) {
                    count++;
                }
                break;
            }
        }
    }
    
    return count;
}

int countRookMobility(const Board& board, int square, int piece) {
    int count = 0;
    
    for (int dir : ROOK_DIRS) {
        int target = square;
        while (true) {
            int prev = target;
            target += dir;
            if (target < 0 || target >= 64) break;
            
            // Horizontal wrap guard
            if (dir == -1 || dir == 1) {
                int prevFile = prev % 8;
                int toFile = target % 8;
                if (std::abs(toFile - prevFile) != 1) break;
            }
            
            int targetPiece = board.getPiece(target);
            if (targetPiece == EMPTY) {
                count++;
            } else {
                if (isOpponent(piece, targetPiece)) {
                    count++;
                }
                break;
            }
        }
    }
    
    return count;
}

int countQueenMobility(const Board& board, int square, int piece) {
    // Queen is just rook + bishop 
    return countRookMobility(board, square, piece) + 
           countBishopMobility(board, square, piece);
}

int countKingMobility(const Board& board, int square, int piece) {
    int count = 0;
    
    for (int off : KING_DIRS) {
        int target = square + off;
        
        int fromFile = square % 8;
        int toFile = target % 8;
        if (std::abs(fromFile - toFile) > 1) continue;
        
        if (target >= 0 && target < 64) {
            int targetPiece = board.getPiece(target);
            if (targetPiece == EMPTY || isOpponent(piece, targetPiece)) {
                count++;
            }
        }
    }
    
    // too expensive to count castling
    
    return count;
}