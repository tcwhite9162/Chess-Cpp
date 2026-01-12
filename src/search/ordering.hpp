#pragma once

#include "core/board.hpp"
#include "movegen/moveGen.hpp"

namespace Search::Ordering {

    void scoreMoves(MoveList& moves, const Board& board);

}