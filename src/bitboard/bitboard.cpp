#include "bitboard.hpp"

u64 knightAttacks[64];
u64 kingAttacks[64];
u64 pawnAttacks[2][64];

void initKnightAttacks() {
    for (int sq = 0; sq < 64; sq++) {
        u64 bb = 0ULL;
        int r = sq / 8;
        int f = sq % 8;
        const int dr[8] = { -2, -2, -1, -1, 1, 1, 2, 2 };
        const int df[8] = { -1, 1, -2, 2, -2, 2, -1, 1 };

        for (int i = 0; i < 8; i++) {
            int rr = r + dr[i];
            int ff = f + df[i];
            if (rr >= 0 && rr < 8 && ff >= 0 && ff < 8) {
                bb |= 1ULL << (rr * 8 + ff);
            }
        }
        knightAttacks[sq] = bb;
    }
}

void initKingAttacks() {
    for (int sq = 0; sq < 64; sq++) {
        u64 bb = 0ULL;
        int r = sq / 8;
        int f = sq % 8;
        for (int dr = -1; dr <= 1; dr++) {
            for (int df = -1; df <= 1; df++) {
                if (dr == 0 && df == 0) continue;

                int rr = r + dr;
                int ff = f + df;
                if (rr >= 0 && rr < 8 && ff >= 0 && ff < 8) {
                    bb |= 1ULL << (rr * 8 + ff);
                }
            }
        }
        kingAttacks[sq] = bb;
    }
}

void initPawnAttacks() {
    for (int sq = 0; sq < 64; sq++) {
        u64 white = 0ULL;
        u64 black = 0ULL;
        int r = sq / 8;
        int f = sq % 8;

        if (r > 0 && f > 0) white |= 1ULL << (sq + 9);
        if (r > 0 && f < 7) white |= 1ULL << (sq + 7);

        if (r < 7 && f < 7) black |= 1ULL << (sq - 9);
        if (r < 7 && f > 0) black |= 1ULL << (sq - 7);

        pawnAttacks[0][sq] = white;
        pawnAttacks[1][sq] = black;
    }

}

void initAttackTables() {
    initKnightAttacks();
    initKingAttacks();
    initPawnAttacks();
}

u64 bishopAttacks(int square, u64 occupancy) {
    u64 attacks = 0ULL;
    int r = square / 8;
    int f = square % 8;

    // up right
    for (int rr = r-1, ff = f+1; rr >= 0 && ff < 8; --rr, ++ff) {
        int sq = rr*8 + ff;
        attacks |= 1ULL << sq;
        if (occupancy & (1ULL << sq)) break;
    }
    // up left
    for (int rr = r-1, ff = f-1; rr >= 0 && ff >= 0; --rr, --ff) {
        int sq = rr*8 + ff;
        attacks |= 1ULL << sq;
        if (occupancy & (1ULL << sq)) break;
    }
    // down right
    for (int rr = r+1, ff = f+1; rr < 8 && ff < 8; ++rr, ++ff) {
        int sq = rr*8 + ff;
        attacks |= 1ULL << sq;
        if (occupancy & (1ULL << sq)) break;
    }
    // down left
    for (int rr = r+1, ff = f-1; rr < 8 && ff >= 0; ++rr, --ff) {
        int sq = rr*8 + ff;
        attacks |= 1ULL << sq;
        if (occupancy & (1ULL << sq)) break;
    }

    return attacks;
}

u64 rookAttacks(int square, u64 occupancy) {
    u64 attacks = 0ULL;
    int r = square / 8;
    int f = square % 8;

    // up
    for (int rr = r-1; rr >= 0; --rr) {
        int sq = rr*8 + f;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq)) break; 
    }
    // down
    for (int rr = r+1; rr < 8; ++rr) {
        int sq = rr*8 + f;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq)) break; 
    }
    // left
    for (int ff = f-1; ff >= 0; --ff) {
        int sq = r*8 + ff;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq)) break; 
    }
    // right
    for (int ff = f+1; ff < 8; ++ff) {
        int sq = r*8 + ff;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq)) break; 
    }

    return attacks;
}  

u64 queenAttacks(int square, u64 occupancy) {
    return bishopAttacks(square, occupancy) | rookAttacks(square, occupancy);
} 
