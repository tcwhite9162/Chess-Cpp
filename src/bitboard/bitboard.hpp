#pragma once

#include <cstdint>

#include "bitboard/magic.hpp"

using u64 = uint64_t;

extern u64 knightAttacks[64];
extern u64 kingAttacks[64];
extern u64 pawnAttacksWhite[64];
extern u64 pawnAttacksBlack[64];

u64 bishopAttacks(int square, u64 occupancy);
u64 rookAttacks(int square, u64 occupancy);
u64 queenAttacks(int square, u64 occupancy);

extern u64 rookMasks[64];
extern u64 bishopMasks[64];

extern int rookRelevantBits[64];
extern int bishopRelevantBits[64];

extern u64 rookAttackTable[64][4096];
extern u64 bishopAttackTable[64][1024];

inline u64 rookAttacksMagic(const int sq, u64 occ) {
    occ &= rookMasks[sq];
    occ *= rookMagic[sq];
    occ >>= (64 - rookRelevantBits[sq]);
    return rookAttackTable[sq][occ];
}

inline u64 bishopAttacksMagic(const int sq, u64 occ) {
    occ &= bishopMasks[sq];
    occ *= bishopMagic[sq];
    occ >>= (64 - bishopRelevantBits[sq]);
    return bishopAttackTable[sq][occ];
}

void initAttackTables();