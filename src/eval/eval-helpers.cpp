#include "core/utils.hpp"
#include "eval/eval-helpers.hpp"

int countMobility(const Board& board, int side) {
    int count = 0;

    for (int square = 0; square < 64; square++) {
        int piece = board.getPiece(square);
        if (piece == Constants::Piece::EMPTY) continue;
        if ((side == Constants::Piece::WHITE && piece < 0) || (side == Constants::Piece::BLACK && piece > 0)) continue;

        switch (std::abs(piece)) {
            case Constants::Piece::PAWN:   count += countPawnMobility(board,   square, piece); break;
            case Constants::Piece::KNIGHT: count += countKnightMobility(board, square, piece); break;
            case Constants::Piece::BISHOP: count += countBishopMobility(board, square, piece); break;
            case Constants::Piece::ROOK:   count += countRookMobility(board,   square, piece); break;
            case Constants::Piece::QUEEN:  count += countQueenMobility(board,  square, piece); break;
            case Constants::Piece::KING:   count += countKingMobility(board,   square, piece); break;

            default: break;
        }
    }

    return count;
}

int countPawnMobility(const Board& board, int square, int piece) {
    int count = 0;
    const int direction = (piece == Constants::Piece::W_PAWN ? Constants::Squares::UP : Constants::Squares::DOWN);

    const int forwardOne = square + direction;
    if (forwardOne >= 0 && forwardOne < 64 && board.getPiece(forwardOne) == Constants::Piece::EMPTY) {
        count++;

        const int rank = square / 8;
        const int startRank = (piece == Constants::Piece::W_PAWN ? Constants::Moves::WHITE_PAWN_START : Constants::Moves::BLACK_PAWN_START);
        if (rank == startRank) {
            const int forwardTwo = forwardOne + direction;
            if (forwardTwo >= 0 && forwardTwo < 64 && board.getPiece(forwardTwo) == Constants::Piece::EMPTY) {
                count++;
            }
        }
    }

    const int captureOffsets[2] = {direction - 1, direction + 1};
    for (const int offset : captureOffsets) {
        const int captureSquare = square + offset;
        if (captureSquare < 0 || captureSquare >= 64) continue;

        const int file = square % 8;
        const int captureFile = captureSquare % 8;
        if (captureFile != file + 1 && captureFile != captureFile - 1) continue;

        const int target = board.getPiece(captureSquare);
        if ((target != Constants::Piece::EMPTY && isOpponent(piece, target)) ||
            (captureSquare == board.getEnPassant() && board.enPassantAvailable())) {
            count++;
        }
    }

    return count;
}

int countKnightMobility(const Board& board, int square, int piece) {
    int count = 0;

    for (const int off : Constants::Moves::KNIGHT_DIRS) {
        const int target = square + off;

        const int fromFile = square % 8;
        const int toFile = target % 8;
        if (std::abs(fromFile - toFile) > 2) continue;

        if (target >= 0 && target < 64) {
            const int targetPiece = board.getPiece(target);
            if (targetPiece == Constants::Piece::EMPTY || isOpponent(piece, targetPiece)) {
                count++;
            }
        }
    }

    return count;
}

int countBishopMobility(const Board& board, int square, int piece) {
    int count = 0;

    for (const int dir : Constants::Moves::BISHOP_DIRS) {
        int target = square;
        while (true) {
            const int prev = target;
            target += dir;
            if (target < 0 || target >= 64) break;

            // Diagonal wrap guard
            const int prevFile = prev % 8;
            const int toFile = target % 8;
            if (std::abs(toFile - prevFile) != 1) break;

            const int targetPiece = board.getPiece(target);
            if (targetPiece == Constants::Piece::EMPTY) {
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

    for (const int dir : Constants::Moves::ROOK_DIRS) {
        int target = square;
        while (true) {
            const int prev = target;
            target += dir;
            if (target < 0 || target >= 64) break;

            // Horizontal wrap guard
            if (dir == -1 || dir == 1) {
                const int prevFile = prev % 8;
                const int toFile = target % 8;
                if (std::abs(toFile - prevFile) != 1) break;
            }

            const int targetPiece = board.getPiece(target);
            if (targetPiece == Constants::Piece::EMPTY) {
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

    for (const int off : Constants::Moves::KING_DIRS) {
        const int target = square + off;

        const int fromFile = square % 8;
        const int toFile = target % 8;
        if (std::abs(fromFile - toFile) > 1) continue;

        if (target >= 0 && target < 64) {
            const int targetPiece = board.getPiece(target);
            if (targetPiece == Constants::Piece::EMPTY || isOpponent(piece, targetPiece)) {
                count++;
            }
        }
    }

    // too expensive to count castling

    return count;
}