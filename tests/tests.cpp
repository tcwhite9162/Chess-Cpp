#include <iostream>
#include <string>

#include "move.hpp"
#include "utils.hpp"
#include "board.hpp"
#include "moveGen.hpp"

long long perft(Board &board, int depth) {
    if (depth == 0) return 1;
    long long nodes = 0;
    MoveList movesList;
    generateLegalMoves(board, movesList);
    if (depth == 1) return movesList.count;
    for (int i = 0; i < movesList.count; i++) {
        Move move = movesList.moves[i];
        board.makeMove(move, false);
        nodes += perft(board, depth - 1);
        board.unmakeMove(move, false);
    }
    return nodes;
}

void testPerft(Board &board, int depth) {
    for (int d = 1; d <= depth; d++) {
        long long n = perft(board, d);
        std::cout << "perft(" << d << ") = " << n << std::endl;
    }
}

void testSinglePerft(Board &board, int depth) {
    long long n = perft(board, depth);
    std::cout << "perft(" << depth << ") = " << n << std::endl;
}

long long perft_divide(Board &board, int depth) {
    MoveList movesList;
    generateLegalMoves(board, movesList);

    long long total = 0;
    for (int i = 0; i < movesList.count; i++) {
        Move m = movesList.moves[i];
        board.makeMove(m, false);
        long long nodes = perft(board, depth - 1);
        board.unmakeMove(m, false);
        std::cout << m.to_string() << ": " << nodes << "\n";
        total += nodes;
    }
    std::cout << "Total: " << total << "\n";
    return total;
}


// convert square index to algebraic notation (0=a8, 63=h1)
std::string squareToStr(int sq) {
    int r = sq / 8;
    int f = sq % 8;
    char fileChar = 'a' + f;
    char rankChar = '8' - r;
    return std::string{fileChar, rankChar};
}

long long perft_debug(Board &board, int depth) {
    if (depth == 0) return 1;
    long long nodes = 0;
    MoveList movesList;
    generateLegalMoves(board, movesList);

    for (int i = 0; i < movesList.count; i++) {
        Move move = movesList.moves[i];
        board.makeMove(move, false);

        // check legality after the move
        if (board.isInCheck(board.getTurn())) {
            std::cout << "Illegal move (king in check): " << move.to_string() 
                      << " at depth " << depth << "\n";
            board.unmakeMove(move, false);
            continue;
        }

        nodes += perft_debug(board, depth - 1);
        board.unmakeMove(move, false);
    }
    return nodes;
}


void debugGeneratePawnMoves(const Board &board, int color) {
    MoveList moves;
    generatePawnMoves(board, color, moves);

    std::cout << "count: " << moves.count << "\n";

    std::cout << (color == WHITE ? "White pawn moves:\n" : "Black pawn moves:\n");

    for (int i = 0; i < moves.count; i++) {
        Move m = moves.moves[i];
        std::string from = squareToStr(m.from_square());
        std::string to   = squareToStr(m.to_square());
        std::string flags;

        if (isCapture(m.flags())) flags += "x";
        if (isPromotion(m.flags())) {
            int promo = promotionPiece(m.flags(), color);
            flags += "=";
            flags += pieceToChar(promo);
        }
        if (m.flags() & FLAG_EN_PASSANT) flags += " e.p.";

        std::cout << from << to << flags << "\n";
    }

    std::cout << "Total pawn moves: " << moves.count << "\n";
}

void perftTestSuite(Board& board, int depth) {
    for (std::string fen : testFens) {
        std::cout << "input FEN --- " << fen << "\n";
        board.setupFromFen(fen);
        std::cout << "--start position--\n";
        board.printBoard();

        std::cout << "--results--\n";
        testPerft(board, depth);
        std::cout << "\n\n\n\n";
    }
}