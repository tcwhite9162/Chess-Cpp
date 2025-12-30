#pragma once
#include <array>
#include <cstdint>

extern std::array<uint64_t, 64> knightAttacks;
extern std::array<uint64_t, 64> kingAttacks;
extern std::array<uint64_t, 64> pawnAttacksWhite;
extern std::array<uint64_t, 64> pawnAttacksBlack;

void initAttackTables();