#pragma once

#include "movegen/moveGen.hpp"
#include "core/board.hpp"

namespace Search::Ordering {

    void scoreMoves(MoveList& moves, const Board& board);

}