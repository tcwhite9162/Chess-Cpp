#include <iostream>
#include <sstream>
#include <string>

#include "core/utils.hpp"
#include "interface/interface.hpp"
#include "interface/uci.hpp"
#include "search/search.hpp"

namespace UCI {

void uciLoop(Board& board) {
    std::string line;
    std::cout << "uciok\n";

    while (std::getline(std::cin, line)) {
        trim(line);

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "uci") {
            std::cout << "uciok";
        }

        else if (cmd == "isready") {
            std::cout << "readyok\n";
        }

        else if (cmd == "position") {
            Interface::handlePosition(line, board);
        }

        else if (cmd == "go") {
            std::string arg;
            iss >> arg;

            if (arg == "depth") {
                std::string d;
                if (!(iss >> d)) {
                    std::cout << "'go depth' requires a depth\n";
                }

                int depth           = std::stoi(d);
                SearchResult search = Search::searchBestMove(board, depth);
                std::cout << "bestmove " << search.bestMove.to_string() << "\n";
            }
            else if (arg == "movetime") {
                std::string t;
                if (!(iss >> t)) {
                    std::cout << "'go movetime' requires a time\n";
                }

                int ms = std::stoi(t);
                SearchResult search = Search::searchBestMovetime(board, ms);
                std::cout << "bestmove " << search.bestMove.to_string() << "\n";
            }
            else {
                std::cout << "'go' requires arg\n";
            }
        }

        else if (cmd == "quit") {
            return;
        }
        
        else {
            std::cout << "? how\n";
        }
    }
}

} // namespace UCI
