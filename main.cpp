#include <vector>
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

            std::vector<Move> moves = generateLegalMoves(board);
            std::cout << moves.size() << std::endl;
        }
        if (arg == "-p") {
            testPerft(board, 5);
        }
    }



    return 0;
}