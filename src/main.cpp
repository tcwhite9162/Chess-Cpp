#include <iostream>
#include <string>

#include "move.hpp"
#include "moveGen.hpp"
#include "board.hpp"
#include "evaluate.hpp"
#include "zobrist.hpp"
#include "bitboard/bitboard.hpp"
#include "../tests/tests.hpp"

int main(int argc, char* argv[]) {
    Zobrist::initZobrist();
    initAttackTables();

    Board board = Board();
    board.setupStartPosition();

    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-d") { 
            board.printBoard();

            MoveList ms;
            generateLegalMoves(board, ms);
            
            std::cout << "ep square: "<< board.getEnPassant() << "\n";
            std::cout << "move count: "<< ms.count << std::endl;

            for (int i = 0; i < ms.count; i++) {
                Move m = ms.moves[i];
                std::cout << m.to_string() << std::endl;
            }
        }
        if (arg == "-dd") {
            int depth = std::stoi(argv[i+1]);
            long long nodes = perft_debug(board, depth);
            std::cout << "nodes: " << nodes << "\n";
            int e1 = 4; // e1 square index
            bool attacked = board.isSquareAttacked(e1, WHITE);
            std::cout << "e1 attacked by black? " << attacked << "\n";

            return 0;
        }
        if (arg == "-p") {
            int depth = std::stoi(argv[i+1]);
            testSinglePerft(board, depth);
            return 0;
        }

        if (arg == "-pf") {
            int depth = std::stoi(argv[i+1]);
            testPerft(board, depth);
            return 0;
        }

        if (arg == "-pd") {
            int depth = std::stoi(argv[i+1]);
            perft_divide(board, depth);
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