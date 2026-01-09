#include "uci.hpp"
#include "interface.hpp"
#include "core/utils.hpp"
#include "core/move.hpp"
#include "search/search.hpp"

#include <iostream>
#include <string>

namespace UCI {

void uciLoop(Board& board) {
    std::string line;

    while (std::getline(std::cin, line)) {
        trim(line);
        std::string cmd = Interface::getCommandWord(line);
        
        if (cmd == "uci") {
            std::cout << "id name MyEngine\n";
            std::cout << "id author Thomas\n";
            std::cout << "uciok\n";
        }
        
        else if (cmd == "isready") {
            std::cout << "readyok\n";
        }
        
        else if (cmd == "position") {
            Interface::handlePosition(line, board);
        }

        else if (cmd == "go") {
            // search here
            std::cout << "bestmove <move>\n";
        }

        else if (cmd == "quit") {
            return;
        }
    }
}

}