#pragma once
#include <cstdint>

using u64 = uint64_t;

extern u64 knightAttacks[64];
extern u64 kingAttacks[64];
extern u64 pawnAttacks[2][64]; // [color][square] 0 = white, 1 = black

void initAttackTables();

u64 bishopAttacks(int square, u64 occupancy);
u64 rookAttacks(int square, u64 occupancy);
u64 queenAttacks(int square, u64 occupancy);