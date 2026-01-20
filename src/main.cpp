#include "core/board.hpp"
#include "interface/interface.hpp"
#include "interface/uci.hpp"
#include "core/utils.hpp"

#include <iostream>
#include <string>

int main() {
    initMatch();
    Board board;
    board.setupStartPosition();

    std::string first;
    if (!std::getline(std::cin, first))
        return 0; // no input exit

    trim(first);

    if (first == "uci")
        UCI::uciLoop(board);
    else
        Interface::userLoop(board);

    return 0;
}
