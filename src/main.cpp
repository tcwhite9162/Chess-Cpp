#include <iostream>
#include <string>

#include "moveGen.hpp"
#include "board.hpp"
#include "evaluate.hpp"
#include "zobrist.hpp"
#include "../tests/tests.h"

int main(int argc, char* argv[]) {
    Zobrist::initZobrist();
    Board board = Board();
    board.setupStartPosition();
    
    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-d") { 
            board.printBoard();

            MoveList moves;
            generateLegalMoves(board, moves);
            std::cout << moves.count << std::endl;
        }
        if (arg == "-p") {
            int depth = std::stoi(argv[i+1]);
            testSinglePerft(board, depth);
            return 0;
        }
        if (arg == "-e") {
            int eval = evaluate(board);
            std::cout << "eval: " << eval << std::endl;
            board.printBoard();

        }
    }



    return 0;
}