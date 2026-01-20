#include <random>
#include <cstdint>

#include "search/zobrist.hpp"

using u64 = uint64_t;

namespace Zobrist {
    u64 piece[12][64];      // 6 white + 6 black pieces
    u64 castling[16];       // 4 bits → 16 possible castling states
    u64 enpassant[8];       // file 0–7
    u64 side;               // side to move

    void initZobrist() {
        std::mt19937_64 rng(0xABCDEF123456789ULL);

        for (int p = 0; p < 12; ++p)
            for (int sq = 0; sq < 64; ++sq)
                Zobrist::piece[p][sq] = rng();

        for (int i = 0; i < 16; ++i)
            Zobrist::castling[i] = rng();

        for (int f = 0; f < 8; ++f)
            Zobrist::enpassant[f] = rng();

        Zobrist::side = rng();
    }
}