#include <vector>
#include <iostream>

#include "moveGen.h"
#include "board.h"

int main() {
    Board board = Board();
    
    board.setupStartPosition();
    board.printBoard();

    std::vector<Move> moves = generateAllMoves(board);
    std::cout << moves.size() << std::endl;

    std::cout << board.isSquareAttacked(42, 1) << std::endl;

    return 0;
}