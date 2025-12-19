#ifndef UNDOINFO_H

#include "move.h"

struct undoInfo {
    Move move;
    int capturedPiece;
    int castlingRights;
    int enPassantSquare;
    int halfmoveClock;
    int fullmoveNumber;
    int whiteKingPos;
    int blackKingPos;
};

#endif