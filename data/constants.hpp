#ifndef constexprANTS_H
#define constexprANTS_H

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
constexpr int ROOK_DIRS[4]   = {-8, 8, -1, 1};
constexpr int BISHOP_DIRS[4] = {-9, 9, -7, 7};
constexpr int QUEEN_DIRS[8]  = {-9, 9, -8, 8, -7, 7, -1, 1};

constexpr int KING_DIRS[8]   = {-9, 9, -8, 8, -7, 7, -1, 1};
constexpr int KNIGHT_DIRS[8] = {-17, 17, -15, 15, -10, 10, -6, 6};

// pawn ranks
constexpr int WHITE_PAWN_START = 6;
constexpr int BLACK_PAWN_START = 1;
constexpr int WHITE_PROMOTION_RANK = 0;
constexpr int BLACK_PROMOTION_RANK = 7;


// piece values
constexpr int EMPTY  = 0;
constexpr int PAWN   = 1;
constexpr int KNIGHT = 2;
constexpr int BISHOP = 3;
constexpr int ROOK   = 4;
constexpr int QUEEN  = 5;
constexpr int KING   = 6;

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

// rook positions for castling
constexpr int W_ROOK_A1 = 56;
constexpr int W_ROOK_H1 = 63;

constexpr int B_ROOK_A8 = 0;
constexpr int B_ROOK_H8 = 7;


constexpr int W_ROOK_D1_END = 59;
constexpr int W_ROOK_F1_END = 61;
constexpr int B_ROOK_D8_END = 3;
constexpr int B_ROOK_F8_END = 5;

constexpr int WHITE_KING_START = 60;
constexpr int BLACK_KING_START = 4;

// squares passed through during castling
constexpr int WHITE_KINGSIDE_PATH[3]  = {60, 61, 62}; // E1, F1, G1
constexpr int WHITE_QUEENSIDE_PATH[3] = {60, 59, 58}; // E1, D1, C1
constexpr int BLACK_KINGSIDE_PATH[3]  = {4, 5, 6};    // E8, F8, G8
constexpr int BLACK_QUEENSIDE_PATH[3] = {4, 3, 2};    // E8, D8, C8

// squares that must be empty when castling
constexpr int WHITE_KINGSIDE_EMPTY[2]  = {61, 62};     // F1, G1
constexpr int WHITE_QUEENSIDE_EMPTY[3] = {59, 58, 57}; // D1, C1, B1
constexpr int BLACK_KINGSIDE_EMPTY[2]  = {5, 6};       // F8, G8
constexpr int BLACK_QUEENSIDE_EMPTY[3] = {3, 2, 1};    // D8, C8, B8

// king end positions when castling
constexpr int WHITE_KINGSIDE_END  = 62;
constexpr int WHITE_QUEENSIDE_END = 58;
constexpr int BLACK_KINGSIDE_END  = 6;
constexpr int BLACK_QUEENSIDE_END  = 2;

// move flags (bit masks)
constexpr int FLAG_NONE        = 0;       // normal move
constexpr int FLAG_CAPTURE     = 1 << 0;  // capture move
constexpr int FLAG_DOUBLE_PUSH = 1 << 1;  // pawn double push (sets en passant square)
constexpr int FLAG_EN_PASSANT  = 1 << 2;  // en passant capture

// promotions (OR with FLAG_CAPTURE if its a capture promotion)
constexpr int FLAG_PROMOTION   = 1 << 3;  // marker bit
constexpr int FLAG_PROMO_N     = 1 << 4;  // promote to knight
constexpr int FLAG_PROMO_B     = 1 << 5;  // promote to bishop
constexpr int FLAG_PROMO_R     = 1 << 6;  // promote to rook
constexpr int FLAG_PROMO_Q     = 1 << 7;  // promote to queen

// castling
constexpr int FLAG_CASTLE_WK   = 1 << 8;  // white king-side castle
constexpr int FLAG_CASTLE_WQ   = 1 << 9;  // white queen-side castle
constexpr int FLAG_CASTLE_BK   = 1 << 10; // black king-side castle
constexpr int FLAG_CASTLE_BQ   = 1 << 11; // black queen-side castle

#endif 
