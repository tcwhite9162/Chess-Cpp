#pragma once

#include <limits>
#include <string>
#include <cstdint>

using u64 = uint64_t;

namespace Flags {

    // move flags (bit masks)
    constexpr int FLAG_NONE        = 0;       // normal move
    constexpr int FLAG_CAPTURE     = 1 << 0;  // capture move
    constexpr int FLAG_DOUBLE_PUSH = 1 << 1;  // pawn double push (sets en passant square)
    constexpr int FLAG_EN_PASSANT  = 1 << 2;  // en passant capture

    // promotions (OR'ed with FLAG_CAPTURE if capture promotion)
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
    constexpr int FLAG_CASTLE_ANY  = FLAG_CASTLE_WK | FLAG_CASTLE_WQ | FLAG_CASTLE_BK | FLAG_CASTLE_BQ;

}

// namespace Data {
namespace Constants {

namespace Squares {

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

    // bitboard stuff

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
    constexpr u64 RANK_8 = 0x00000000000000FFULL; // a8–h8
    constexpr u64 RANK_7 = RANK_8 << 8;           // a7–h7
    constexpr u64 RANK_6 = RANK_8 << 16;
    constexpr u64 RANK_5 = RANK_8 << 24;
    constexpr u64 RANK_4 = RANK_8 << 32;
    constexpr u64 RANK_3 = RANK_8 << 40;
    constexpr u64 RANK_2 = RANK_8 << 48;
    constexpr u64 RANK_1 = RANK_8 << 56; // a1–h1

}

namespace Piece {

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

}

namespace Moves {

    // piece move directions
    constexpr int ROOK_DIRS[4]   = {-8, 8, -1, 1}; // up, down, left, right
    constexpr int BISHOP_DIRS[4] = {-9, 9, -7, 7}; // up-left, down-right, up-right, down-left
    constexpr int QUEEN_DIRS[8]  = {-9, 9, -8, 8, -7, 7, -1, 1};

    constexpr int KING_DIRS[8]   = {-9, 9, -8, 8, -7, 7, -1, 1};
    constexpr int KNIGHT_DIRS[8] = {-17, 17, -15, 15, -10, 10, -6, 6};

    // pawn ranks
    constexpr int WHITE_PAWN_START = 6; // rank 2
    constexpr int BLACK_PAWN_START = 1; // rank 7
    constexpr int WHITE_PROMOTION_RANK = 0; // rank 1
    constexpr int BLACK_PROMOTION_RANK = 7; // rank 8

}

namespace Castling {

    // castling rights (bitmask)
    constexpr int CASTLE_NONE = 0;
    constexpr int CASTLE_WK   = 1 << 0; // not flags. used to track castling rights
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
    constexpr int WHITE_KINGSIDE_END  = 62; // G1
    constexpr int WHITE_QUEENSIDE_END = 58; // C1
    constexpr int BLACK_KINGSIDE_END  = 6;  // G8
    constexpr int BLACK_QUEENSIDE_END  = 2; // C8

}

namespace Eval {

    // piece values
    constexpr int PAWN_VAL   = 100;
    constexpr int KNIGHT_VAL = 320;
    constexpr int BISHOP_VAL = 330;
    constexpr int ROOK_VAL   = 500;
    constexpr int QUEEN_VAL  = 900;
    constexpr int KING_VAL   = 20000;

    // evaluation factors
    constexpr int MOBILITY_FACTOR = 10;
    constexpr int PAWN_PENALTY    = 25;

    // most valuable victim, least valuable attacker
    constexpr int MVV_VICTIM_MULT = 10;
    constexpr int MVV_ATTACKER_MULT = 1;

    // promotion bonuses
    constexpr int PROMO_QUEEN_BONUS = 9000;
    constexpr int PROMO_ROOK_BONUS = 5000;
    constexpr int PROMO_BISHOP_BONUS = 3250;
    constexpr int PROMO_KNIGHT_BONUS = 3000;

    // quiet move score
    constexpr int QUIET_BASE_SCORE = 0;

}

namespace Search {

    constexpr int TT_SIZE_MB = 64;

    constexpr int TT_MOVE_BONUS = 100000000; // 1e8

    constexpr int INF = std::numeric_limits<int>::max(); // arbitrary large number for search

}

namespace FEN {
    const std::string startFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    // for tests
    const std::string testFen_1 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    const std::string testFen_2 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
    const std::string testFen_3 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    const std::string testFen_4 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    const std::string testFen_5 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";

    const std::string testFens[5] = {testFen_1, testFen_2, testFen_3, testFen_4, testFen_5};
}

} // namespace Data
