#pragma once

#include "constants.hpp"
#include "bitboard.hpp"
#include "board.hpp"
#include "zobrist.hpp"

inline int rank(int square) { return square / 8; }
inline int file(int square) { return square % 8; }
inline bool isValidSquare(int square) {return square >= 0 && square < 64;}

inline bool isCapture(int flags)     { return flags & FLAG_CAPTURE; }
inline bool isPromotion(int flags)   { return flags & FLAG_PROMOTION; }

inline int promotionPiece(int flags, int turn) {
    if (flags & FLAG_PROMO_N) { return (turn == WHITE) ? W_KNIGHT : B_KNIGHT; }
    if (flags & FLAG_PROMO_B) { return (turn == WHITE) ? W_BISHOP : B_BISHOP; }
    if (flags & FLAG_PROMO_R) { return (turn == WHITE) ? W_ROOK : B_ROOK; }
    if (flags & FLAG_PROMO_Q) { return (turn == WHITE) ? W_QUEEN : B_QUEEN; }
    return EMPTY;
}

inline bool isOpponent(int piece, int targetPiece) {
    return (piece > 0 && targetPiece < 0) || (piece < 0 && targetPiece > 0);
}

inline int pieceToIndex(int piece) {
    switch (piece) {
        case W_PAWN:   return 0;
        case W_KNIGHT: return 1;
        case W_BISHOP: return 2;
        case W_ROOK:   return 3;
        case W_QUEEN:  return 4;
        case W_KING:   return 5;

        case B_PAWN:   return 6;
        case B_KNIGHT: return 7;
        case B_BISHOP: return 8;
        case B_ROOK:   return 9;
        case B_QUEEN:  return 10;
        case B_KING:   return 11;
    }
    return -1; // should never happen
}

inline int popLeastSigBit(u64 &bitboard) {
    int sq = __builtin_ctzll(bitboard); // gcc
    bitboard &= bitboard - 1;
    return sq;
}

inline bool hasBits(u64 bitboard) { return bitboard != 0; }
inline int bitCount(u64 bitboard) { return __builtin_popcountll(bitboard);}


inline std::string pieceToChar(int piece) {
    switch (piece) {
        case W_PAWN:   return u8"♟";
        case W_KNIGHT: return u8"♞";
        case W_BISHOP: return u8"♝";
        case W_ROOK:   return u8"♜";
        case W_QUEEN:  return u8"♛";
        case W_KING:   return u8"♚";

        case B_PAWN:   return u8"♙";
        case B_KNIGHT: return u8"♘";
        case B_BISHOP: return u8"♗";
        case B_ROOK:   return u8"♖";
        case B_QUEEN:  return u8"♕";
        case B_KING:   return u8"♔";

        case EMPTY:    return u8" ";
        default:       return u8"?";
    }
}


inline void initMatch() {
    Zobrist::initZobrist();
    initAttackTables();
}

inline u64 random64() {
    static u64 seed = 0x9e3779b97f4a7c15ULL; // any non-zero seed

    seed ^= seed >> 12;
    seed ^= seed << 25;
    seed ^= seed >> 27;
    return seed * 0x2545F4914F6CDD1DULL;
}

inline u64 randomMagic() { return random64() & random64() & random64(); }

void setFromFen(Board& board);