#ifndef UNDOINFO_H

#include "move.hpp"

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