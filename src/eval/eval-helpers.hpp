#pragma once

#include "core/board.hpp"

int countMobility(const Board& board, int side);
int countPawnMobility(const Board& board, int square, int piece);
int countKnightMobility(const Board& board, int square, int piece);
int countBishopMobility(const Board& board, int square, int piece);
int countRookMobility(const Board& board, int square, int piece);
int countQueenMobility(const Board& board, int square, int piece);
int countKingMobility(const Board& board, int square, int piece);
