#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <cstdint>

namespace Zobrist {
    extern uint64_t piece[12][64];
    extern uint64_t castling[16];
    extern uint64_t enpassant[8];
    extern uint64_t side;

    void initZobrist();
}
#endif