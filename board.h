#ifndef BOARD_H
#define BOARD_H

#include <array>
#include "constants.h"

class Board {
private:
    std::array<int, 64> squares;
    int turn;
    int castling;
    int en_passant;
    int halfmove;
    int fullmove;
    int blackKingPosition;
    int whiteKingPosition;

public:

    int getTurn() const {return turn;}
    int getCastlingRights() const {return castling;}
    int getEnPassant() const {return en_passant;}

    void setTurn(int t) {turn = t;}
    void setCastlingRights(int c) {castling = c;}
    void setEnPassant(int sq) {en_passant = sq;}

    bool isInCheck(int color);
    bool isSquareAttacked(int square, int attackingColor);

    Board();
    void setPiece(int square, int piece);
    int getPiece(int square) const;
    void flipTurn();
    void printBoard() const;
    void setupStartPosition();
};


#endif