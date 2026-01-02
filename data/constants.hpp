#pragma once


#include <cstdint>
#include <limits>
#include <string>

using u64 = uint64_t;

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

// piece values
constexpr int PAWN_VAL   = 100;
constexpr int KNIGHT_VAL = 320;
constexpr int BISHOP_VAL = 330;
constexpr int ROOK_VAL   = 500;
constexpr int QUEEN_VAL  = 900;
constexpr int KING_VAL   = 20000;

// PST values //
constexpr int PAWN_PST[64] = {
   0,   0,   0,   0,   0,   0,   0,   0,
  50,  50,  50,  50,  50,  50,  50,  50,
  10,  10,  20,  30,  30,  20,  10,  10,
   5,   5,  10,  25,  25,  10,   5,   5,
   0,   0,   0,  20,  20,   0,   0,   0,
   5, - 5, -10,   0,   0, -10, - 5,   5,
   5,  10,  10, -20, -20,  10,  10,   5,
   0,   0,   0,   0,   0,   0,   0,   0
};
constexpr int KNIGHT_PST[64] = {
 -50, -40, -30, -30, -30, -30, -40, -50,
 -40, -20,   0,   0,   0,   0, -20, -40,
 -30,   0,  10,  15,  15,  10,   0, -30,
 -30,   5,  15,  20,  20,  15,   5, -30,
 -30,   0,  15,  20,  20,  15,   0, -30,
 -30,   5,  10,  15,  15,  10,   5, -30,
 -40, -20,   0,   5,   5,   0, -20, -40,
 -50, -40, -30, -30, -30, -30, -40, -50
};
constexpr int BISHOP_PST[64] = {
 -20, -10, -10, -10, -10, -10, -10, -20,
 -10,   0,   0,   0,   0,   0,   0, -10,
 -10,   0,   5,  10,  10,   5,   0, -10,
 -10,   5,   5,  10,  10,   5,   5, -10,
 -10,   0,  10,  10,  10,  10,   0, -10,
 -10,  10,  10,  10,  10,  10,  10, -10,
 -10,   5,   0,   0,   0,   0,   5, -10,
 -20, -10, -10, -10, -10, -10, -10, -20
};
constexpr int ROOK_PST[64] = {
   0,   0,   0,   0,   0,   0,   0,   0,
   5,  10,  10,  10,  10,  10,  10,   5,
  -5,   0,   0,   0,   0,   0,   0,  -5,
  -5,   0,   0,   0,   0,   0,   0,  -5,
  -5,   0,   0,   0,   0,   0,   0,  -5,
  -5,   0,   0,   0,   0,   0,   0,  -5,
  -5,   0,   0,   0,   0,   0,   0,  -5,
   0,   0,   0,   5,   5,   0,   0,   0
};
constexpr int QUEEN_PST[64] = {
 -20, -10, -10,  -5,  -5, -10, -10, -20,
 -10,   0,   0,   0,   0,   0,   0, -10,
 -10,   0,   5,   5,   5,   5,   0, -10,
  -5,   0,   5,   5,   5,   5,   0,  -5,
   0,   0,   5,   5,   5,   5,   0,  -5,
 -10,   5,   5,   5,   5,   5,   0, -10,
 -10,   0,   5,   0,   0,   0,   0, -10,
 -20, -10, -10,  -5,  -5, -10, -10, -20
};
constexpr int KING_PST[64] = {
 -30, -40, -40, -50, -50, -40, -40, -30,
 -30, -40, -40, -50, -50, -40, -40, -30,
 -30, -40, -40, -50, -50, -40, -40, -30,
 -30, -40, -40, -50, -50, -40, -40, -30,
 -20, -30, -30, -40, -40, -30, -30, -20,
 -10, -20, -20, -20, -20, -20, -20, -10,
  20,  20,   0,   0,   0,   0,  20,  20,
  20,  30,  10,   0,   0,  10,  30,  20
};

// evaluation factors
constexpr int MOBILITY_FACTOR = 10;
constexpr int PAWN_PENALTY    = 25;

static constexpr int INF = std::numeric_limits<int>::max(); // arbitrary large number for search

// bitboard stuff //

// file masks
constexpr u64 FILE_A = 0x0101010101010101ULL;
constexpr u64 FILE_B = FILE_A << 1;
constexpr u64 FILE_C = FILE_A << 2;
constexpr u64 FILE_D = FILE_A << 3;
constexpr u64 FILE_E = FILE_A << 4;
constexpr u64 FILE_F = FILE_A << 5;
constexpr u64 FILE_G = FILE_A << 6;
constexpr u64 FILE_H = FILE_A << 7;

constexpr u64 NOT_FILE_A = ~FILE_A;
constexpr u64 NOT_FILE_H = ~FILE_H;

// rank masks
constexpr u64 RANK_8 = 0x00000000000000FFULL;      // a8–h8 (top)
constexpr u64 RANK_7 = RANK_8 << 8;               // a7–h7
constexpr u64 RANK_6 = RANK_8 << 16;
constexpr u64 RANK_5 = RANK_8 << 24;
constexpr u64 RANK_4 = RANK_8 << 32;
constexpr u64 RANK_3 = RANK_8 << 40;
constexpr u64 RANK_2 = RANK_8 << 48;
constexpr u64 RANK_1 = RANK_8 << 56;              // a1–h1 (bottom)

// fen's
const std::string startFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

const std::string testFen_1 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
const std::string testFen_2 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
const std::string testFen_3 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
const std::string testFen_4 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
const std::string testFen_5 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";

const std::string testFens[] = {testFen_1, testFen_2, testFen_3, testFen_4, testFen_5};