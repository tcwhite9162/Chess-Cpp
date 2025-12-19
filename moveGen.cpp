#include "moveGen.h"
#include "constants.h"
#include <vector>

inline bool isOpponent(int piece, int targetPiece) {
    return (piece > 0 && targetPiece < 0) || (piece < 0 && targetPiece > 0);
}


std::vector<Move> generatePromotionMoves(const Board &board, int fromSquare, int toSquare, bool isCapture=false) {
    std::vector<Move> moves;

    if (isCapture) {
        moves.emplace_back(fromSquare, toSquare, FLAG_PROMOTION | FLAG_PROMO_Q | FLAG_CAPTURE);
        moves.emplace_back(fromSquare, toSquare, FLAG_PROMOTION | FLAG_PROMO_R | FLAG_CAPTURE);
        moves.emplace_back(fromSquare, toSquare, FLAG_PROMOTION | FLAG_PROMO_B | FLAG_CAPTURE);
        moves.emplace_back(fromSquare, toSquare, FLAG_PROMOTION | FLAG_PROMO_N | FLAG_CAPTURE);
    }
    else {
        moves.emplace_back(fromSquare, toSquare, FLAG_PROMOTION | FLAG_PROMO_Q);
        moves.emplace_back(fromSquare, toSquare, FLAG_PROMOTION | FLAG_PROMO_R);
        moves.emplace_back(fromSquare, toSquare, FLAG_PROMOTION | FLAG_PROMO_B);
        moves.emplace_back(fromSquare, toSquare, FLAG_PROMOTION | FLAG_PROMO_N);
    }
    return moves;
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
            std::vector<Move> promotionMoves = generatePromotionMoves(board, square, forwardOne);
            moves.insert(moves.end(), promotionMoves.begin(), promotionMoves.end());
        }
        else {
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
            int captureRank = captureSquare / 8;
            
            if (( (board.getTurn() == 1 ) && ( captureRank == WHITE_PROMOTION_RANK ))
             || ( (board.getTurn() == -1 ) && (captureRank == BLACK_PROMOTION_RANK ))) {
                std::vector<Move> promoCaptureMoves = generatePromotionMoves(board, square, captureSquare, true);
                moves.insert(moves.end(), promoCaptureMoves.begin(), promoCaptureMoves.end());
            }
            else {
                moves.emplace_back(square, captureSquare, FLAG_CAPTURE);
            }

        }
        else if ((captureSquare == board.getEnPassant()) && board.enPassantAvailable()) {
            moves.emplace_back(square, captureSquare, FLAG_CAPTURE | FLAG_EN_PASSANT);
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
            if (targetPiece == EMPTY) {
                moves.emplace_back(square, target, FLAG_NONE);
            } else if ((piece < 0 && targetPiece > 0) || (piece > 0 && targetPiece < 0)) {
                moves.emplace_back(square, target, FLAG_CAPTURE);
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
            if (targetPiece == EMPTY) {
                moves.emplace_back(square, target, FLAG_NONE);
            } else if ((piece < 0 && targetPiece > 0) || (piece > 0 && targetPiece < 0)) {
                moves.emplace_back(square, target, FLAG_CAPTURE);
            }
        }
    }

    // white kingside castle
    if (piece == W_KING && board.getTurn() == WHITE && board.canCastleWhiteKingside()) {
        bool empty = true;
        for (int sq : WHITE_KINGSIDE_EMPTY) {
            if (board.getPiece(sq) != EMPTY) { empty = false; break; }
        }
        bool safe = true;
        for (int sq : WHITE_KINGSIDE_PATH) {
            if (board.isSquareAttacked(sq, BLACK)) { safe = false; break; }
        }
        if (empty && safe) {
            moves.emplace_back(square, WHITE_KINGSIDE_END, FLAG_CASTLE_WK);
        }
    }

    // white queenside castle
    if (piece == W_KING && board.getTurn() == WHITE && board.canCastleWhiteQueenside()) {
        bool empty = true;
        for (int sq : WHITE_QUEENSIDE_EMPTY) {
            if (board.getPiece(sq) != EMPTY) { empty = false; break; }
        }
        bool safe = true;
        for (int sq : WHITE_QUEENSIDE_PATH) {
            if (board.isSquareAttacked(sq, BLACK)) { safe = false; break; }
        }
        if (empty && safe) {
            moves.emplace_back(square, WHITE_QUEENSIDE_END, FLAG_CASTLE_WQ);
        }
    }

    // black kingside castle
    if (piece == B_KING && board.getTurn() == BLACK && board.canCastleBlackKingside()) {
        bool empty = true;
        for (int sq : BLACK_KINGSIDE_EMPTY) {
            if (board.getPiece(sq) != EMPTY) { empty = false; break; }
        }
        bool safe = true;
        for (int sq : BLACK_KINGSIDE_PATH) {
            if (board.isSquareAttacked(sq, WHITE)) { safe = false; break; }
        }
        if (empty && safe) {
            moves.emplace_back(square, BLACK_KINGSIDE_END, FLAG_CASTLE_BK);
        }
    }

    // black queenside castle
    if (piece == B_KING && board.getTurn() == BLACK && board.canCastleBlackQueenside()) {
        bool empty = true;
        for (int sq : BLACK_QUEENSIDE_EMPTY) {
            if (board.getPiece(sq) != EMPTY) { empty = false; break; }
        }
        bool safe = true;
        for (int sq : BLACK_QUEENSIDE_PATH) {
            if (board.isSquareAttacked(sq, WHITE)) { safe = false; break; }
        }
        if (empty && safe) {
            moves.emplace_back(square, BLACK_QUEENSIDE_END, FLAG_CASTLE_BQ);
        }
    }

    return moves;
}

std::vector<Move> generateSlidingMoves(const Board &board, int square) {
    std::vector<Move> moves;
    int piece = board.getPiece(square);

    // Rook-like (orthogonal)
    if (piece == W_ROOK || piece == B_ROOK || piece == W_QUEEN || piece == B_QUEEN){
        int directions[] = {-8, -1, 1, 8}; // up, left, right, down
        for (int dir : directions) {
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
                    moves.emplace_back(square, target, FLAG_NONE);
                } else {
                    if (isOpponent(piece, targetPiece)) {
                        moves.emplace_back(square, target, FLAG_CAPTURE);
                    }
                    break; // stop ray at first blocker
                }
            }
        }
    }

    // Bishop-like (diagonal)
    if (piece == W_BISHOP || piece == B_BISHOP || piece == W_QUEEN || piece == B_QUEEN){
        int directions[] = {-9, -7, 7, 9};
        for (int dir : directions) {
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
                    moves.emplace_back(square, target, FLAG_NONE);
                } else {
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
        int turn = board.getTurn();

        if (piece == EMPTY) continue;
        if ((turn == WHITE && piece < 0) || (turn == BLACK && piece > 0)) continue;

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

std::vector<Move> generateLegalMoves(Board &board) {
    std::vector<Move> legalMoves;
    std::vector<Move> pseudoLegalMoves = generateAllMoves(board);

    for (const Move &move : pseudoLegalMoves) {
        int side = board.getTurn();
        board.makeMove(move);

        if (!board.isInCheck(side)) {
            legalMoves.push_back(move);
        }
        board.unmakeMove(move);
    }    
    return legalMoves;
}