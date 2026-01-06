#include "board.hpp"
#include "zobrist.hpp"
#include "interface.hpp"
#include "bitboard.hpp"

int main(int argc, char* argv[]) {
    Zobrist::initZobrist();
    initAttackTables();

    Board board;

    Interface::inputLoop(board);

    return 0;
}
