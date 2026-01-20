#pragma once

#include "core/board.hpp"
#include "pst.hpp"

inline auto mirrorSquare = [](int x) { return 63 -x; };

inline void matchPstTable(const int* table, const int piece) {
    switch(std::abs(piece)) {
        case Constants::Piece::PAWN:   table = Data::PST::PAWN_PST; break;
        case Constants::Piece::KNIGHT: table = Data::PST::KNIGHT_PST; break;
        case Constants::Piece::BISHOP: table = Data::PST::BISHOP_PST; break;
        case Constants::Piece::ROOK:   table = Data::PST::ROOK_PST; break;
        case Constants::Piece::QUEEN:  table = Data::PST::QUEEN_PST; break;
        case Constants::Piece::KING:   table = Data::PST::KING_PST; break;
        default: return;
    }
}

inline const int* getPiecePstTable(int piece) {
    const int* table = nullptr;

    switch(std::abs(piece)) {
        case Constants::Piece::PAWN:   table = Data::PST::PAWN_PST; break;
        case Constants::Piece::KNIGHT: table = Data::PST::KNIGHT_PST; break;
        case Constants::Piece::BISHOP: table = Data::PST::BISHOP_PST; break;
        case Constants::Piece::ROOK:   table = Data::PST::ROOK_PST; break;
        case Constants::Piece::QUEEN:  table = Data::PST::QUEEN_PST; break;
        case Constants::Piece::KING:   table = Data::PST::KING_PST; break;
        default: return nullptr;
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
