#include <iostream>
#include "board.h"

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

struct Move {
    unsigned int data;

    Move(int from_sq, int to_sq, int flags = 0) {
        data = from_sq | (to_sq << 6) | (flags << 12);
    }
    
    static unsigned int encode(int from_sq, int to_sq, int flags = 0) {
        return from_sq | (to_sq << 6) | (flags << 12);
    }

    int from_square() const {return data & 0x3f;}
    int to_square() const {return (data >> 6) & 0x3f;}
    int flags() const {return (data >> 12) & 0x3f;}

    std::string to_string() {
        auto squareToString = [](int sq) {
            char file = 'a' + (sq % 8);
            char rank = '8' - (sq / 8);
            return std::string{file} + rank;
        };
        return squareToString(from_square()) + squareToString(to_square());
    }

    void print() const {
        std::cout << "Move(" << from_square() << " -> " << to_square()
                  << ", flags= " << flags() << ")" << std::endl; 
    }

};

Board::Board() {
    squares.fill(EMPTY);
    turn = WHITE;
    castling = 0;
    en_passant = -1;
    halfmove = 0;
    fullmove = 1;
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
