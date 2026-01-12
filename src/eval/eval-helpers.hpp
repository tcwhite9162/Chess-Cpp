#pragma once

#include "core/board.hpp"
#include "pst.hpp"

inline auto mirrorSquare = [](int x) { return 63 -x; };

inline void matchPstTable(const int* table, int piece) {
    switch(std::abs(piece)) {
        case PAWN:   table = PAWN_PST; break;
        case KNIGHT: table = KNIGHT_PST; break;
        case BISHOP: table = BISHOP_PST; break;
        case ROOK:   table = ROOK_PST; break;
        case QUEEN:  table = QUEEN_PST; break;
        case KING:   table = KING_PST; break;
        default: return;
    }
}

inline const int* getPiecePstTable(int piece) {
    const int* table = nullptr;

    switch(std::abs(piece)) {
        case PAWN:   table = PAWN_PST; break;
        case KNIGHT: table = KNIGHT_PST; break;
        case BISHOP: table = BISHOP_PST; break;
        case ROOK:   table = ROOK_PST; break;
        case QUEEN:  table = QUEEN_PST; break;
        case KING:   table = KING_PST; break;
        default: return 0;
    }
    return table;
}

int countMobility(const Board& board, int side);
int countPawnMobility(const Board& board, int square, int piece);
int countKnightMobility(const Board& board, int square, int piece);
int countBishopMobility(const Board& board, int square, int piece);
int countRookMobility(const Board& board, int square, int piece);
int countQueenMobility(const Board& board, int square, int piece);
int countKingMobility(const Board& board, int square, int piece);
