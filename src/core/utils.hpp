#pragma once

#include <algorithm>

#include "constants.hpp"
#include "core/board.hpp"
#include "search/zobrist.hpp"
#include "bitboard/bitboard.hpp"

inline int rank(int square) { return square / 8; }
inline int file(int square) { return square % 8; }
inline bool isValidSquare(int square) {return square >= 0 && square < 64;}

inline bool isCapture(int flags)     { return flags & Flags::FLAG_CAPTURE; }
inline bool isPromotion(int flags)   { return flags & Flags::FLAG_PROMOTION; }

inline int promotionPiece(int flags, int turn) {
    if (flags & Flags::FLAG_PROMO_N) { return (turn == Constants::Piece::WHITE) ? Constants::Piece::W_KNIGHT : Constants::Piece::B_KNIGHT; }
    if (flags & Flags::FLAG_PROMO_B) { return (turn == Constants::Piece::WHITE) ? Constants::Piece::W_BISHOP : Constants::Piece::B_BISHOP; }
    if (flags & Flags::FLAG_PROMO_R) { return (turn == Constants::Piece::WHITE) ? Constants::Piece::W_ROOK   : Constants::Piece::B_ROOK; }
    if (flags & Flags::FLAG_PROMO_Q) { return (turn == Constants::Piece::WHITE) ? Constants::Piece::W_QUEEN  : Constants::Piece::B_QUEEN; }
    return Constants::Piece::EMPTY;
}

inline bool isOpponent(int piece, int targetPiece) {
    return (piece > 0 && targetPiece < 0) || (piece < 0 && targetPiece > 0);
}

inline constexpr PieceType pieceToPieceType(int piece) {
    return static_cast<PieceType>( (piece > 0) ? piece - 1 : 6 + (-piece - 1) );
}

inline void trim(std::string& s) {
  // left trim
  while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) {
    s.erase(s.begin());
  }
  // right trim
  while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) {
    s.pop_back();
  }
}

inline bool isDigitsOnly(const std::string& str) {
    if (str.empty()) { return false; }
    return std::all_of(str.begin(), str.end(), [](unsigned char c){
            return std::isdigit(c);
            });
    return true;
}

inline int pieceToIndex(int piece) {
    switch (piece) {
        case Constants::Piece::W_PAWN:   return 0;
        case Constants::Piece::W_KNIGHT: return 1;
        case Constants::Piece::W_BISHOP: return 2;
        case Constants::Piece::W_ROOK:   return 3;
        case Constants::Piece::W_QUEEN:  return 4;
        case Constants::Piece::W_KING:   return 5;

        case Constants::Piece::B_PAWN:   return 6;
        case Constants::Piece::B_KNIGHT: return 7;
        case Constants::Piece::B_BISHOP: return 8;
        case Constants::Piece::B_ROOK:   return 9;
        case Constants::Piece::B_QUEEN:  return 10;
        case Constants::Piece::B_KING:   return 11;

        default: return 0;
    }
}

static int getPieceValue(int piece) {
    piece = std::abs(piece);

    switch (piece) {
        case Constants::Piece::PAWN:   return Constants::Eval::PAWN_VAL;
        case Constants::Piece::KNIGHT: return Constants::Eval::KNIGHT_VAL;
        case Constants::Piece::BISHOP: return Constants::Eval::BISHOP_VAL;
        case Constants::Piece::ROOK:   return Constants::Eval::ROOK_VAL;
        case Constants::Piece::QUEEN:  return Constants::Eval::QUEEN_VAL;
        default:     return 0;
    }
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
        case Constants::Piece::W_PAWN:   return u8"♟";
        case Constants::Piece::W_KNIGHT: return u8"♞";
        case Constants::Piece::W_BISHOP: return u8"♝";
        case Constants::Piece::W_ROOK:   return u8"♜";
        case Constants::Piece::W_QUEEN:  return u8"♛";
        case Constants::Piece::W_KING:   return u8"♚";

        case Constants::Piece::B_PAWN:   return u8"♙";
        case Constants::Piece::B_KNIGHT: return u8"♘";
        case Constants::Piece::B_BISHOP: return u8"♗";
        case Constants::Piece::B_ROOK:   return u8"♖";
        case Constants::Piece::B_QUEEN:  return u8"♕";
        case Constants::Piece::B_KING:   return u8"♔";

        case Constants::Piece::EMPTY:    return u8" ";
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

// bitboard helpers
inline int file_of(int sq) { return sq & 7; }
inline int rank_of(int sq) { return sq >> 3; }

inline int popcount(u64 bb) {
    return __builtin_popcountll(bb);
}

inline int lsb(u64 bb) {
    return bb ? __builtin_ctzll(bb) : -1;
}

inline u64 between(int sq1, int sq2) {
    u64 mask = 0ULL;

    if (file_of(sq1) == file_of(sq2)) {
        const int step = (sq2 > sq1) ? 8 : -8;
        for (int sq = sq1 + step; sq != sq2; sq += step) {
            mask |= (1ULL << sq);
        }
    }

    else if (rank_of(sq1) == rank_of(sq2)) {
        const int step = (sq2 > sq1) ? 1 : -1;
        for (int sq = sq1 + step; sq != sq2; sq += step) {
            mask |= (1ULL << sq);
        }
    }

    else if (abs(file_of(sq1) - file_of(sq2)) == abs(rank_of(sq1) - rank_of(sq2))) {
        const int fileStep = (file_of(sq2) > file_of(sq1)) ? 1 : -1;
        const int rankStep = (rank_of(sq2) > rank_of(sq1)) ? 1 : -1;
        int sq = sq1 + fileStep + 8 * rankStep;

        while (sq != sq2) {
            mask |= (1ULL << sq);
            sq += (fileStep + 8 * rankStep);
        }
    }

    return mask;
}
