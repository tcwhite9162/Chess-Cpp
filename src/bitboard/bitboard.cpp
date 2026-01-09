#include "bitboard.hpp"
#include "core/utils.hpp"
#include "magic.hpp"

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

        if (r < 7 && f > 0) white |= 1ULL << (sq + 7);  // down-left
        if (r < 7 && f < 7) white |= 1ULL << (sq + 9);  // down-right

        if (r > 0 && f < 7) black |= 1ULL << (sq - 7);  // up-right
        if (r > 0 && f > 0) black |= 1ULL << (sq - 9);  // up-left

        pawnAttacks[0][sq] = white;
        pawnAttacks[1][sq] = black;
    }
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

u64 rookMasks[64];
u64 bishopMasks[64];

u64 rookMask(int sq) {
    u64 mask = 0ULL;
    int r = sq / 8;
    int f = sq % 8;

    // up
    for (int rr = r - 1; rr > 0; --rr) {
        mask |= 1ULL << (rr * 8 + f);
    }

    // down
    for (int rr = r + 1; rr < 7; ++rr) {
        mask |= 1ULL << (rr * 8 + f);
    }

    // left
    for (int ff = f - 1; ff > 0; --ff) {
        mask |= 1ULL << (r * 8 + ff);
    }

    // right
    for (int ff = f + 1; ff < 7; ++ff) {
        mask |= 1ULL << (r * 8 + ff);
    }

    return mask;
}

u64 bishopMask(int sq) {
    u64 mask = 0ULL;
    int r = sq / 8;
    int f = sq % 8;

    // up left
    for (int rr = r - 1, ff = f - 1; rr > 0 && ff > 0; --rr, --ff) {
        mask |= 1ULL << (rr * 8 + ff);
    }

    // up right
    for (int rr = r - 1, ff = f + 1; rr > 0 && ff < 7; --rr, ++ff) {
        mask |= 1ULL << (rr * 8 + ff);
    }

    // down left
    for (int rr = r + 1, ff = f - 1; rr < 7 && ff > 0; ++rr, --ff) {
        mask |= 1ULL << (rr * 8 + ff);
    }

    // down right
    for (int rr = r + 1, ff = f + 1; rr < 7 && ff < 7; ++rr, ++ff) {
        mask |= 1ULL << (rr * 8 + ff);
    }

    return mask;
}

void initMasks() {
    for (int sq = 0; sq < 64; sq++) {
        rookMasks[sq]   = rookMask(sq);
        bishopMasks[sq] = bishopMask(sq);
    }
}

int rookRelevantBits[64];
int bishopRelevantBits[64];

int rookMaskSquares[64][16];
int bishopMaskSquares[64][16];

void initRelevantBits() {
    for (int sq = 0; sq < 64; sq++) {
        // rooks
        u64 mask = rookMasks[sq];
        int count = 0;
        while (mask) {
            int bit = popLeastSigBit(mask);
            rookMaskSquares[sq][count++] = bit;
        }
        rookRelevantBits[sq] = count;
        
        // bishops
        mask = bishopMasks[sq];
        count = 0;
        while (mask) {
            int bit = popLeastSigBit(mask);
            bishopMaskSquares[sq][count++] = bit;
        }
        bishopRelevantBits[sq] = count;
    }
}

u64 occupancyFromIndex(int sq, int index, bool isRook) {
    u64 occ = 0ULL;

    int count    = isRook ? rookRelevantBits[sq] : bishopRelevantBits[sq];
    int* squares = isRook ? rookMaskSquares[sq] : bishopMaskSquares[sq];

    for (int i = 0; i < count; i++) {
        if (index & (1ULL << i)) {
            occ |= 1ULL << squares[i];
        }
    }

    return occ;
}

u64 rookAttackTable[64][4096];
u64 bishopAttackTable[64][1024];

void initSlidingAttackTables() {
    for (int sq = 0; sq < 64; sq++) {
        
        // rooks
        int rookBits = rookRelevantBits[sq];
        int rookSubsets = 1 << rookBits;

        for (int i = 0; i < rookSubsets; i++) {
            u64 occ = occupancyFromIndex(sq, i, true);

            u64 index = (occ * rookMagic[sq]) >> (64 - rookBits);
            rookAttackTable[sq][index] = rookAttacks(sq, occ);
        }
        
        int bishopBits = bishopRelevantBits[sq];
        int bishopSubsets = 1 << bishopBits;

        // bishops
        for (int j = 0; j < bishopSubsets; j++) {
            u64 occ = occupancyFromIndex(sq, j, false);

            u64 index = (occ * bishopMagic[sq]) >> (64 - bishopBits);
            bishopAttackTable[sq][index] = bishopAttacks(sq, occ);
        }
    }
}

void initAttackTables() {
    initKnightAttacks();
    initKingAttacks();
    initPawnAttacks();
    
    initMasks();
    initRelevantBits();
    initSlidingAttackTables();
}