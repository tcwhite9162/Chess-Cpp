#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <vector>
#include "board.h"
#include "move.h"

std::vector<Move> generatePawnMoves(const Board& board, int square);
std::vector<Move> generateKnightMoves(const Board& board, int square);
std::vector<Move> generateSlidingMoves(const Board& board, int square);
std::vector<Move> generateKingMoves(const Board& board, int square);
std::vector<Move> generateAllMoves(const Board& board);

std::vector<Move> generateLegalMoves(const Board& board);

#endif