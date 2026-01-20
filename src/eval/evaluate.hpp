#pragma once

#include "core/board.hpp"

namespace Evaluate {

    int pstValue(int piece, int square);
    int kingSafety(const Board& board);

    int evaluate(const Board& board);
}