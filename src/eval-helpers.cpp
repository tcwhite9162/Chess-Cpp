#include "eval-helpers.hpp"

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