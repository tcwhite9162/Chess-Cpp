#include <iostream>
#include <string>

#include "moveGen.h"
#include "board.h"
#include "tests.h"

int main(int argc, char* argv[]) {
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
            testPerft(board, 6);
        }
    }



    return 0;
}