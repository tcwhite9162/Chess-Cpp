#pragma once

#include "core/move.hpp"

struct undoInfo {
    Move move;
    int movedPiece{};
    int capturedPiece{};
    int capturedSquare{};
    int castlingRights{};
    int enPassantSquare{};
    int halfmoveClock{};
    int fullmoveNumber{};
    int whiteKingPos{};
    int blackKingPos{};
};
