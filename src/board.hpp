#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <array>

#include "../data/constants.hpp"
#include "move.hpp"
#include "undoInfo.hpp"

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

    std::vector<undoInfo> history;

    void updateCastlingRights(int fromSquare, int toSquare, int piece, int capturedPiece);

public:

    int getTurn() const {return turn;}
    int getCastlingRights() const {return castling;}
    int getEnPassant() const {return en_passant;}

    bool canCastleWhiteKingside()  const { return castling & CASTLE_WK; }
    bool canCastleWhiteQueenside() const { return castling & CASTLE_WQ; }
    bool canCastleBlackKingside()  const { return castling & CASTLE_BK; }
    bool canCastleBlackQueenside() const { return castling & CASTLE_BQ; }

    void setTurn(int t) {turn = t;}
    void setCastlingRights(int c) {castling = c;}
    void setEnPassant(int sq) {en_passant = sq;}

    bool isInCheck(int color) const;
    bool isSquareAttacked(int square, int attackingColor) const;
    bool enPassantAvailable() const;

    Board();
    void setPiece(int square, int piece);
    int getPiece(int square) const;
    void flipTurn();
    void printBoard() const;
    void setupStartPosition();

    void makeMove(Move move);
    void unmakeMove(Move move);
};


#endif