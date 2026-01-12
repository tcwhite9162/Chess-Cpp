#pragma once

#include <string>

#include "core/board.hpp"

namespace Tests{

    long long perft(Board &board, int depth);
    void testAllPerft(Board &board, int depth);
    void testSinglePerft(Board &board, int depth);
    long long perftDivide(Board &board, int depth);
    inline std::string squareToString(int sq);
    std::string moveToString(const Move &m);
    long long perft_debug(Board &board, int depth);
    
    void perftTestSuite(Board& board, int depth);
    
    void debugGeneratePawnMoves(const Board &board, int color);

    void timeSearch(Board& board, int depth);
    void timeSearchFen(const std::string& fen, int depth);
}