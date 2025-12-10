#include <iostream>
#include "board.h"
#include "constants.h"
#include <cstdlib>

char pieceToChar(int piece) {
    switch (piece) {
        case W_PAWN:   return 'P';
        case W_KNIGHT: return 'N';
        case W_BISHOP: return 'B';
        case W_ROOK:   return 'R';
        case W_QUEEN:  return 'Q';
        case W_KING:   return 'K';

        case B_PAWN:   return 'p';
        case B_KNIGHT: return 'n';
        case B_BISHOP: return 'b';
        case B_ROOK:   return 'r';
        case B_QUEEN:  return 'q';
        case B_KING:   return 'k';

        case EMPTY:    return '.';
        default:       return '?'; // unknown
    }
}

inline int rank(int square) { return square / 8; }
inline int file(int square) { return square % 8; }
inline bool isValidSquare(int square) {return square >= 0 && square < 64;}


Board::Board() {
    squares.fill(EMPTY);
    turn = WHITE;
    castling = 0;
    en_passant = -1;
    halfmove = 0;
    fullmove = 1;
    whiteKingPosition = WHITE_KING_START;
    blackKingPosition = BLACK_KING_START;
}

void Board::setPiece(int square, int piece) {
    if (square >= 0 && square < 64) {
        squares[square] = piece;
    }
}

int Board::getPiece(int square) const {
    if (square >= 0 && square < 64) {
        return squares[square];
    }
    return EMPTY;
}
void Board::flipTurn() {
    turn = -turn;
}

void Board::printBoard() const {
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            std::cout << pieceToChar(squares[square]) << "  ";
        }
        std::cout << std::endl;
    }
}
void Board::setupStartPosition() {
    // pawns
    for (int i = 8; i < 16; i++) {
        squares[i] = B_PAWN;
        squares[i + 40] = W_PAWN;
    }
    //rooks
    squares[0] = B_ROOK;    squares[7] = B_ROOK;
    squares[56] = W_ROOK;   squares[63] = W_ROOK;

    //knights
    squares[1] = B_KNIGHT;  squares[6] = B_KNIGHT;
    squares[57] = W_KNIGHT; squares[62] = W_KNIGHT;

    // bishops
    squares[2] = B_BISHOP;  squares[5] = B_BISHOP;
    squares[58] = W_BISHOP; squares[61] = W_BISHOP;
    
    // queens
    squares[3] = B_QUEEN;   squares[59] = W_QUEEN;

    // kings
    squares[4] = B_KING;    squares[60] = W_KING;

}

bool Board::isInCheck(int color) {
    int kingPosition;
    kingPosition = (color == 1) ? whiteKingPosition : blackKingPosition;

    return isSquareAttacked(kingPosition, color * -1);
}

bool Board::isSquareAttacked(int square, int attackingColor) {
    int squareRank = rank(square);
    int squareFile = file(square);
    
    // pawns
    if (attackingColor == 1) { // white attacking
        int offsets[2] = {DOWN_LEFT, DOWN_RIGHT};
        for (int offset : offsets) {
            int target = square + offset;
            if (isValidSquare(target)) {
                int targetFile = file(target);
                int dFile = targetFile - squareFile;

                if (abs(dFile) == 1 && squares[target] == W_PAWN) {
                    return true;
                }
            }
        }
    }
    else { // black attacking
        int offsets[2] = {UP_LEFT, UP_RIGHT};
        for (int offset : offsets) {
            int target = square + offset;
            if (isValidSquare(target)) {
                int targetFile = file(target);
                
                int dFile = abs(targetFile - squareFile);
                if (dFile == 1 && squares[target] == B_PAWN) {
                    return true;
                }
            }
        }
    }


    // knights
    for (int offset : KNIGHT_DIRS) {
        int target = square + offset;
        if (!(isValidSquare(target))) {
            continue;
        }
        int targetFile = file(target);
        
        int dFile = abs(targetFile - squareFile);
        if (dFile <= 2) {
            if (attackingColor == 1  && squares[target] == W_KNIGHT) { return true; }
            if (attackingColor == -1 && squares[target] == B_KNIGHT) { return true; }
        }
    }

    // bishops / queens (diagonal)
    for (int direction : BISHOP_DIRS) {
        int currentSquare = square + direction;
        while (isValidSquare(currentSquare)) {
            int currentFile = file(currentSquare);
            int prevFile    = file(currentSquare - direction);

            // file difference should always be one unless we wrap
            int dFile = abs(currentFile - prevFile);
            if (dFile != 1) { break; }

            int currentPiece = squares[currentSquare];
            if (currentPiece != EMPTY) {
                if      (attackingColor ==  1 && ((currentPiece == W_BISHOP) || (currentPiece == W_QUEEN))) { return true; }
                else if (attackingColor == -1 && ((currentPiece == B_BISHOP) || (currentPiece == B_QUEEN))) { return true; }
                else { break; } // break if we hit another piece
            }
            currentSquare += direction;
        }
    }

    // rooks / queens (orthogonal)
    for (int direction : ROOK_DIRS) {
        int currentSquare = square + direction;
        while (isValidSquare(currentSquare)) {
            int currentFile = file(currentSquare);

            if ((direction == LEFT)  && (currentFile == 7)) { break; } // moving left, break if we wrap from file 0 to 7
            if ((direction == RIGHT) && (currentFile == 0)) { break; } // same for moving right

            int currentPiece = squares[currentSquare];
            if (currentPiece != EMPTY) {
                if      (attackingColor ==  1 && ((currentPiece == W_ROOK) || (currentPiece == W_QUEEN))) { return true; }
                else if (attackingColor == -1 && ((currentPiece == B_ROOK) || (currentPiece == B_QUEEN))) { return true; }
                else { break; }
            }
            currentSquare += direction;
        }
    }

    // kings
    for (int direction : KING_DIRS) {
        int targetSquare = square + direction;
        if (!(isValidSquare(targetSquare))) { continue; }

        int targetFile = file(targetSquare);
        int dFile = abs(targetFile - squareFile);
        if (dFile <= 1) {
            int currentPiece = squares[targetSquare];
            if      (attackingColor ==  1 && currentPiece == W_KING) { return true; }
            else if (attackingColor == -1 && currentPiece == B_KING) { return true; }
        }
    }
    
    return false;
}