#include <random>
#include <cstdint>
#include "zobrist.hpp"

namespace Zobrist {
    uint64_t piece[12][64];      // 6 white + 6 black pieces
    uint64_t castling[16];       // 4 bits → 16 possible castling states
    uint64_t enpassant[8];       // file 0–7
    uint64_t side;               // side to move

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