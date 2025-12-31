#ifndef UNDOINFO_H
#define UNDOINFO_H

#include "move.hpp"

struct undoInfo {
    Move move;
    int movedPiece;
    int capturedPiece;
    int capturedSquare;
    int castlingRights;
    int enPassantSquare;
    int halfmoveClock;
    int fullmoveNumber;
    int whiteKingPos;
    int blackKingPos;
};

#endif