#pragma once

#include <cstdint>
#include <vector>

#include "core/move.hpp"

using u64 = uint64_t;
using u8 = uint8_t;

enum TTFlag : u8 {
    TT_EXACT = 0,
    TT_LOWER = 1,
    TT_UPPER = 2
};

struct TTEntry {
    u64 key = 0;                 // zobrist
    int depth = -1;              // search depth
    int score = 0;               // evaluation
    Move bestMove = Move();      // optional
    u8 flag = TTFlag::TT_EXACT;  // exact / lowerbound / upperbound
};

struct TranspositionTable {
    std::vector<TTEntry> table;

    explicit TranspositionTable(const size_t sizeMB) {
        const size_t entries = (sizeMB * 1024 * 1024) / sizeof(TTEntry);

        // round down to nearest power of 2
        size_t pow2 = 1;
        while (pow2 * 2 <= entries)
            pow2 *= 2;

        table.resize(pow2);
    }

    TTEntry& operator[](const u64 key) { return table[key & (table.size() - 1)]; }
};
