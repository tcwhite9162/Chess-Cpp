#include "board.hpp"
#include "zobrist.hpp"
#include "interface/interface.hpp"
#include "bitboard/bitboard.hpp"

int main(int argc, char* argv[]) {
    Zobrist::initZobrist();
    initAttackTables();

    Board board;

    Interface::inputLoop(board);

    return 0;
}
