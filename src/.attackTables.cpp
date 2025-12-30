#include "attackTables.hpp"
#include "../data/constants.hpp"
#include <cstdlib>

std::array<uint64_t, 64> knightAttacks;
std::array<uint64_t, 64> kingAttacks;
std::array<uint64_t, 64> pawnAttacksWhite;
std::array<uint64_t, 64> pawnAttacksBlack;

static bool onBoard(int sq) { return sq >= 0 && sq < 64; }

void initAttackTables() {
    for (int sq = 0; sq < 64; sq++) {
        int r = sq / 8;
        int f = sq % 8;

        // knight
        knightAttacks[sq] = 0;
        for (int off : KNIGHT_DIRS) {
            int t = sq + off;
            if (!onBoard(t)) continue;
            int tf = t % 8;
            if (std::abs(tf - f) > 2) continue;
            knightAttacks[sq] |= (1ULL << t);
        }

        // king
        kingAttacks[sq] = 0;
        for (int off : KING_DIRS) {
            int t = sq + off;
            if (!onBoard(t)) continue;
            int tf = t % 8;
            if (std::abs(tf - f) > 1) continue;
            kingAttacks[sq] |= (1ULL << t);
        }

        // pawn attacks
        pawnAttacksWhite[sq] = 0;
        pawnAttacksBlack[sq] = 0;

        if (r < 7) {
            if (f > 0) pawnAttacksWhite[sq] |= (1ULL << (sq + 7));
            if (f < 7) pawnAttacksWhite[sq] |= (1ULL << (sq + 9));
        }
        if (r > 0) {
            if (f > 0) pawnAttacksBlack[sq] |= (1ULL << (sq - 9));
            if (f < 7) pawnAttacksBlack[sq] |= (1ULL << (sq - 7));
        }
    }
}
