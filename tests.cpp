#include <vector>
#include <iostream>

#include "board.h"
#include "moveGen.h"

long long perft(Board &board, int depth) {
    if (depth == 0) return 1;

    long long nodes = 0;
    std::vector<Move> moves = generateLegalMoves(board);

    for (const Move &move : moves) {
        board.makeMove(move);
        nodes += perft(board, depth - 1);
        board.unmakeMove(move);
    }

    return nodes;
}

void testPerft(Board &board, int depth) {
    for (int d = 1; d <= depth; d++) {
        long long n = perft(board, d);
        std::cout << "perft(" << d << ") = " << n << std::endl;
    }
}

long long perft_divide(Board &board, int depth) {
    auto moves = generateLegalMoves(board);
    long long total = 0;
    for (const Move &m : moves) {
        board.makeMove(m);
        long long nodes = perft(board, depth - 1);
        board.unmakeMove(m);
        std::cout << m.to_string() << ": " << nodes << "\n";
        total += nodes;
    }
    std::cout << "Total: " << total << "\n";
    return total;
}






