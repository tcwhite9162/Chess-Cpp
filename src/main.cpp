#include <iostream>
#include <string>

#include "board.hpp"
#include "moveGen.hpp"
#include "zobrist.hpp"
#include "evaluate.hpp"
#include "../tests/tests.hpp"
#include "bitboard/bitboard.hpp"

int main(int argc, char* argv[]) {
    Zobrist::initZobrist();
    initAttackTables();

    Board board = Board();
    board.setupStartPosition();

    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];

        // if (arg == "-t") {
        // }
        
        if (arg == "-d") { 
            board.printBoard();
            std::cout << "wk bk ep t: " << board.getWhiteKingPos() << "   " << board.getBlackKingPos() << "   " << board.getEnPassant() << "   " << board.getTurn() << "\n";
            std::cout << "casltling: " << board.canCastleBlackKingside() << board.canCastleBlackQueenside() << board.canCastleWhiteKingside() << board.canCastleWhiteQueenside() << "\n";
            std::cout << "flags: " << Move(60, 62, FLAG_CASTLE_WK).flags() << "\n";
            
            MoveList ms;
            generateLegalMoves(board, ms);
            std::cout << "move count: "<< ms.count << std::endl;

            for (int i = 0; i < ms.count; i++) {
                Move m = ms.moves[i];
                std::cout << m.to_string() << std::endl;
            }
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
        if (arg == "-f") {
            int depth = std::stoi(argv[i+1]);
            perftTestSuite(board, depth);
        }
    }



    return 0;
}