#pragma once

#include <cstdint>

#include "core/move.hpp"

using u64 = uint64_t;

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

    u64 prevOccupancyWhite;
    u64 prevOccupancyBlack;
    u64 prevOccupancyAll;

    u64 prevZobrist;

};