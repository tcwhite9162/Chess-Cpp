#pragma once

#include <cstdint>

using u64 = uint64_t;

namespace Zobrist {
    extern u64 piece[12][64];
    extern u64 castling[16];
    extern u64 enpassant[8];
    extern u64 side;

    void initZobrist();
}