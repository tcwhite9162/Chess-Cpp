#ifndef constexprANTS_H
#define constexprANTS_H
#include <array>

// index offsets for orthogonal moves
constexpr int UP    = -8;
constexpr int DOWN  =  8;
constexpr int LEFT  = -1;
constexpr int RIGHT =  1;

// diagonal offsets
constexpr int UP_LEFT    = -9;
constexpr int UP_RIGHT   = -7;
constexpr int DOWN_LEFT  =  7;
constexpr int DOWN_RIGHT =  9;

// piece move directions
constexpr std::array<int, 4> ROOK_DIRS   = {-8, 8, -1, 1};
constexpr std::array<int, 4> BISHOP_DIRS = {-9, 9, -7, 7};
constexpr std::array<int, 8> QUEEN_DIRS  = {-9, 9, -8, 8, -7, 7, -1, 1};

constexpr std::array<int, 8> KING_DIRS   = QUEEN_DIRS;
constexpr std::array<int, 8> KNIGHT_DIRS = {-17, 17, -15, 15, -10, 10, -6, 6};

// pawn ranks
constexpr int WHITE_PAWN_START = 6;
constexpr int BLACK_PAWN_START = 1;
constexpr int WHITE_PROMOTION_RANK = 0;
constexpr int BLACK_PROMOTION_RANK = 7;


// piece values
constexpr int EMPTY    = 0;
constexpr int W_PAWN   = 1;
constexpr int W_KNIGHT = 2;
constexpr int W_BISHOP = 3;
constexpr int W_ROOK   = 4;
constexpr int W_QUEEN  = 5;
constexpr int W_KING   = 6;

constexpr int B_PAWN   = -1;
constexpr int B_KNIGHT = -2;
constexpr int B_BISHOP = -3;
constexpr int B_ROOK   = -4;
constexpr int B_QUEEN  = -5;
constexpr int B_KING   = -6;

// colors
constexpr int WHITE = 1;
constexpr int BLACK = -1;

// castling rights (bitmask)
constexpr int CASTLE_NONE = 0;
constexpr int CASTLE_WK   = 1 << 0;
constexpr int CASTLE_WQ   = 1 << 1;
constexpr int CASTLE_BK   = 1 << 2;
constexpr int CASTLE_BQ   = 1 << 3;
constexpr int CASTLE_ALL  = CASTLE_WK | CASTLE_WQ | CASTLE_BK | CASTLE_BQ;

// move flags 4-bit field
constexpr int FLAG_NONE        = 0;  // normal move
constexpr int FLAG_CAPTURE     = 1;  // capture move
constexpr int FLAG_DOUBLE_PUSH = 2;  // pawn double push (sets en passant square)
constexpr int FLAG_EN_PASSANT  = 3;  // en passant capture

// promotions (OR with FLAG_CAPTURE if its a capture promotion)
constexpr int FLAG_PROMO_N     = 4;  // promote to knight
constexpr int FLAG_PROMO_B     = 5;  // promote to bishop
constexpr int FLAG_PROMO_R     = 6;  // promote to rook
constexpr int FLAG_PROMO_Q     = 7;  // promote to queen

// castling
constexpr int FLAG_CASTLE_WK   = 8;  // white king-side castle
constexpr int FLAG_CASTLE_WQ   = 9;  // white queen-side castle
constexpr int FLAG_CASTLE_BK   = 10; // black king-side castle
constexpr int FLAG_CASTLE_BQ   = 11; // black queen-side castle

constexpr int WHITE_KING_START = 60;
constexpr int BLACK_KING_START = 4;

#endif 
