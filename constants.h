#ifndef CONSTANTS_H
#define CONSTANTS_H

// index offsets for orthogonal moves
const int UP    = -8;
const int DOWN  =  8;
const int LEFT  = -1;
const int RIGHT =  1;

// diagonal offsets
const int UP_LEFT    = -9;
const int UP_RIGHT   = -7;
const int DOWN_LEFT  =  7;
const int DOWN_RIGHT =  9;

// pawn ranks
const int WHITE_PAWN_START = 6;
const int BLACK_PAWN_START = 1;
const int WHITE_PROMOTION_RANK = 0;
const int BLACK_PROMOTION_RANK = 7;


// piece values
const int EMPTY    = 0;
const int W_PAWN   = 1;
const int W_KNIGHT = 2;
const int W_BISHOP = 3;
const int W_ROOK   = 4;
const int W_QUEEN  = 5;
const int W_KING   = 6;

const int B_PAWN   = -1;
const int B_KNIGHT = -2;
const int B_BISHOP = -3;
const int B_ROOK   = -4;
const int B_QUEEN  = -5;
const int B_KING   = -6;

// colors
const int WHITE = 1;
const int BLACK = -1;

// castling rights (bitmask)
const int CASTLE_NONE = 0;
const int CASTLE_WK   = 1 << 0;
const int CASTLE_WQ   = 1 << 1;
const int CASTLE_BK   = 1 << 2;
const int CASTLE_BQ   = 1 << 3;
const int CASTLE_ALL  = CASTLE_WK | CASTLE_WQ | CASTLE_BK | CASTLE_BQ;

// move flags 4-bit field
const int FLAG_NONE        = 0;  // normal move
const int FLAG_CAPTURE     = 1;  // capture move
const int FLAG_DOUBLE_PUSH = 2;  // pawn double push (sets en passant square)
const int FLAG_EN_PASSANT  = 3;  // en passant capture

// promotions (OR with FLAG_CAPTURE if its a capture promotion)
const int FLAG_PROMO_N     = 4;  // promote to knight
const int FLAG_PROMO_B     = 5;  // promote to bishop
const int FLAG_PROMO_R     = 6;  // promote to rook
const int FLAG_PROMO_Q     = 7;  // promote to queen

// castling
const int FLAG_CASTLE_WK   = 8;  // white king-side castle
const int FLAG_CASTLE_WQ   = 9;  // white queen-side castle
const int FLAG_CASTLE_BK   = 10; // black king-side castle
const int FLAG_CASTLE_BQ   = 11; // black queen-side castle

#endif 
