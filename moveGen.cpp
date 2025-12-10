#include "moveGen.h"
#include "constants.h"
#include <vector>

inline bool isOpponent(int piece, int targetPiece) {
    return (piece > 0 && targetPiece < 0) || (piece < 0 && targetPiece > 0);
}

std::vector<Move> generatePawnMoves(const Board &board, int square) {
    std::vector<Move> moves;
    int piece = board.getPiece(square);
    int direction, startRank;

    if (piece == W_PAWN) {direction = UP; startRank = WHITE_PAWN_START;}
    else if (piece == B_PAWN) {direction = DOWN; startRank = BLACK_PAWN_START;}
    else return moves;

    int forwardOne = square + direction;
    if (forwardOne >= 0 && forwardOne < 64 && board.getPiece(forwardOne) == EMPTY) {
        // check for promotion
        int rank = square / 8;
        if ((piece > 0 && rank == WHITE_PROMOTION_RANK) || (piece < 0 && rank == BLACK_PROMOTION_RANK)) {
            // TODO: add promotion flag
            moves.emplace_back(square, forwardOne, FLAG_PROMO_Q);
            moves.emplace_back(square, forwardOne, FLAG_PROMO_R);
            moves.emplace_back(square, forwardOne, FLAG_PROMO_B);
            moves.emplace_back(square, forwardOne, FLAG_PROMO_N);
        }else {
            moves.emplace_back(square, forwardOne, FLAG_NONE);
        }
        if (rank == startRank) {
            int forwardTwo = forwardOne + direction;
            if (forwardTwo >= 0 && forwardTwo < 64 && board.getPiece(forwardTwo) == EMPTY) {
                moves.emplace_back(square, forwardTwo, FLAG_DOUBLE_PUSH);
                // TODO: update en passant square in Board
            }
        }
    }

    int captureOffsets[] = {direction + 1, direction - 1};
    for (int offset : captureOffsets) {
        int captureSquare = square + offset;
        int file = square % 8;

        if (captureSquare < 0 || captureSquare >= 64) continue;

        int captureFile = captureSquare % 8;
        if (captureFile != file + 1 && captureFile != file - 1) continue;

        int target = board.getPiece(captureSquare);
        if (target != EMPTY && isOpponent(piece, target)) {
            //TODO add promotion check here
            moves.emplace_back(square, captureSquare, FLAG_CAPTURE);
        }
        else if (captureSquare == board.getEnPassant()) {
            // TODO add board.en_passant_available method
            moves.emplace_back(square, captureSquare, FLAG_EN_PASSANT);
        }
    }
    return moves;
}

std::vector<Move> generateKnightMoves(const Board &board, int square) {
    std::vector<Move> moves;

    int piece = board.getPiece(square);
    if (piece != W_KNIGHT && piece != B_KNIGHT) return moves;

    int offsets[8] = {-17, -15, -10, -6, 6, 10, 15, 17};
    for (int off : offsets) {
        int target = square + off;

        int fromFile = square % 8;
        int toFile   = target % 8;
        if (std::abs(fromFile - toFile) > 2) continue;

        if (target >= 0 && target < 64) {
            int targetPiece = board.getPiece(target);
            if (targetPiece == EMPTY || (piece < 0 && targetPiece > 0) || (piece > 0 && targetPiece < 0)) {
                moves.emplace_back(square, target, FLAG_NONE);
            }
        }
    }
    return moves;
}

std::vector<Move> generateKingMoves(const Board &board, int square) {
    std::vector<Move> moves;

    int piece = board.getPiece(square);
    if (piece != W_KING && piece != B_KING) return moves;

    int offsets[8] = {-9, -8, -7, -1, 1, 7, 8, 9};
    for (int off : offsets) {
        int target = square + off;

        int fromFile = square % 8;
        int toFile   = target % 8;
        if (std::abs(fromFile - toFile) > 1) continue;

        if (target >= 0 && target < 64) {
            int targetPiece = board.getPiece(target);
            if (targetPiece == EMPTY || (piece < 0 && targetPiece > 0) || (piece > 0 && targetPiece < 0)) {
                moves.emplace_back(square, target, FLAG_NONE);
            }
        }
    }
    return moves;
}

std::vector<Move> generateSlidingMoves(const Board &board, int square) {
    std::vector<Move> moves;

    int piece = board.getPiece(square);
    
    if (piece == W_ROOK || piece == B_ROOK || piece == W_QUEEN || piece == B_QUEEN){
        int directions[] = {-8, -1, 1, 8};
        
        for (int dir : directions) {
            int target = square;
            while (true) {
                target += dir;
                if (target < 0 || target >= 64) break;
                
                int targetPiece = board.getPiece(target);
                if (targetPiece == EMPTY) {
                    moves.emplace_back(square, target, FLAG_NONE);
                }
                else {
                    if (isOpponent(piece, targetPiece)) {
                        moves.emplace_back(square, target, FLAG_CAPTURE);
                    }
                    break;
                }
            }
        }
    }
    if (piece == W_BISHOP || piece == B_BISHOP || piece == W_QUEEN || piece == B_QUEEN){
        int directions[] = {-9, -7, 7, 9};

        for (int dir : directions) {
            int target = square;
            while (true) {
                target += dir;
                if (target < 0 || target >= 64) break;

                int targetPiece = board.getPiece(target);
                if (targetPiece == EMPTY) {
                    moves.emplace_back(square, target, FLAG_NONE);
                }
                else {
                    if (isOpponent(piece, targetPiece)) {
                        moves.emplace_back(square, target, FLAG_CAPTURE);
                    }
                    break;
                }
            }
        }
    }
    return moves;
}

std::vector<Move> generateAllMoves(const Board &board) {
    std::vector<Move> moves;

    for (int square = 0; square < 64; square++) {
        int piece = board.getPiece(square);
        if (piece == EMPTY) continue;
        if (piece == W_PAWN || piece == B_PAWN) {
            std::vector<Move> pawnMoves = generatePawnMoves(board, square);
            moves.insert(moves.end(), pawnMoves.begin(), pawnMoves.end());
        }
        else if (piece == W_KNIGHT || piece == B_KNIGHT) {
            std::vector<Move> knightMoves = generateKnightMoves(board, square);
            moves.insert(moves.end(), knightMoves.begin(), knightMoves.end());
        }
        else if (piece == W_KING || piece == B_KING) {
            std::vector<Move> kingMoves = generateKingMoves(board, square);
            moves.insert(moves.end(), kingMoves.begin(), kingMoves.end());
        }
        else {
            std::vector<Move> slidingMoves = generateSlidingMoves(board, square);
            moves.insert(moves.end(), slidingMoves.begin(), slidingMoves.end());
        }
    }
    return moves;
}

std::vector<Move> generateLegalMoves(const Board &board) {
    std::vector<Move> moves;

    




    return moves;
}