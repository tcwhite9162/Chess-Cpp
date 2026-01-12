#include <string>
#include <cstdlib>
#include <sstream>
#include <optional>
#include <iostream>
#include <functional>
#include <unordered_map>

#include "eval/evaluate.hpp"
#include "tests.hpp"
#include "core/move.hpp"
#include "core/board.hpp"
#include "core/utils.hpp"
#include "interface/interface.hpp"

namespace Interface {

  using CommandHandler = std::function<void(std::string&, Board&)>;

  // command map to handler lambdas
  static const std::unordered_map<std::string, CommandHandler> commandTable = {
    { "position", [](std::string& line, Board& board) {
      handlePosition(line, board);
      }
    },
    { "move", [](std::string& line, Board& board) {
        handleMove(line, board);
      }
    },
    { "moves", [](std::string& line, Board& board) {
        handleMoves(line, board);
      }
    },
    { "perft", [](std::string& line, Board& board) {
        handlePerft(line, board, PerftMode::Single);
      }
    },
    { "perft-divide", [](std::string& line, Board& board) {
        handlePerft(line, board, PerftMode::Divide);
      }
    },
    { "perft-suite", [](std::string& line, Board& board) {
        handlePerft(line, board, PerftMode::Suite);
      }
    },
    { "perft-all", [](std::string& line, Board& board) {
        handlePerft(line, board, PerftMode::All);
      }
    },
    { "d", [](std::string& line, Board& board) {
        showState(board, true);
      }
    },
    { "p", [](std::string& line, Board& board) {
        showState(board, false);
      }
    },
    { "h", [](std::string& line, Board& board) {
        showCommands();
      }
    },
    { "help", [](std::string& line, Board& board) {
        showCommands();
      }
    },
    { "clear", [](std::string& line, Board& board) {
        clearScreen();
      }
    },
    { "c", [](std::string& line, Board& board) {
        clearScreen();
      }
    },
    { "search", [](std::string& line, Board& board) {
      handleSearch(line, board);
      }
    },
    { "search-range", [](std::string& line, Board& board) {
      handleSearch(line, board, true);
      }
    }
  };

  inline std::optional<int> strToSquare(const std::string& s) {
    if (s.size() != 2) {
      std::cout << "error: invalid square: '" << s << "'\n";
      return std::nullopt;
    }

    if (s[0] < 'a' || s[0] > 'h') {
          std::cout << "error: invalid square: '" << s << "'\n";
          return std::nullopt;
    }

    if (s[1] < '1' || s[1] > '8') {
          std::cout << "error: invalid square: '" << s << "'\n";
          return std::nullopt;
    }

    int file = s[0] - 'a';
    int rank = s[1] - '1';

    return (7 - rank) * 8 + file;
  }

  inline void clearScreen() {
    std::cout << "\033[2J\033[1;1H" << std::flush;
  }

  std::string getCommandWord(const std::string& line) {
    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;
    return cmd;
  }
 
  void userLoop(Board& board) {
    std::string line;
    std::cout << "👺> ";
    while (std::getline(std::cin, line)) {
      trim(line);
      if (line == "q" || line == "quit") {
        break;
      }
      handleCommand(line, board);
      std::cout << "👺> ";
    }
  }

  void handleCommand(std::string& line, Board& board) {
    trim(line);
    if (line.empty()) {
      return;
    }

    std::string cmd = getCommandWord(line);

    auto command = commandTable.find(cmd);
    if (command != commandTable.end()) {
      command->second(line, board);
      return;
    }

    std::cout << "invalid command\n enter \'h\' to list commands\n";
  }

  void handlePosition(const std::string& line, Board& board) {
    std::stringstream ss(line);
    std::string token;
    ss >> token; // 'position'

    std::string arg;
    if (!(ss >> arg)) {
      std::cout << "error: 'position' requires 'startpos' or 'fen'\n";
      return;
    }

    if (arg == "startpos") {
      board.setupStartPosition();

      if (ss >> token && token == "moves") {
        std::string moveStr;
        while (ss >> moveStr) {
          auto m = parseMoveStr(moveStr, board);
          if (!m) {
            return;
          }
          board.makeMove(*m, true);
        }
      }
    }

    else if (arg == "fen") {
      std::string fenStr;
      std::getline(ss, fenStr);

      trim(fenStr);
      
      board.setupFromFen(fenStr);
    }
    else {
      std::cout << "error: unknown position argument '" << arg << "'\nposition requires 'startpos' or 'fen'\n";
    }
  }

  void handleMove(const std::string& line, Board& board) {
    std::stringstream ss(line);
    std::string cmd, moveStr;
    ss >> cmd;

    if (!(ss >> moveStr)) {
      std::cout << "error: 'move' requires valid move string, eg: 'a2a4' or 'c7c8q'\n";
      return;
    }

    auto m = parseMoveStr(moveStr, board);
    if (!m) {
      return;
    }

    board.makeMove(*m, true);
  }

  void handleMoves(const std::string& line, Board& board) {
    std::stringstream ss(line);
    std::string cmd, moveStr;
    ss >> cmd; // "moves"

    bool hasMoves = false;
    while (ss >> moveStr) {
      hasMoves = true;
      auto move = parseMoveStr(moveStr, board);

      if (!move) {
        return;
      }

      board.makeMove(*move, true);
    }

   if (!hasMoves) {
    std::cout << "error: 'moves' requires at least one move\n";
   }

  }

  void handlePerft(const std::string& line, Board& board, PerftMode mode) {
    std::stringstream ss(line);
    std::string cmd, d;
    ss >> cmd;

    if (!(ss >> d)) {
      std::cout << "error: perft requires a depth\n";
      return;
    }

    int depth = std::stoi(d);

    switch (mode) {

      case PerftMode::Divide:
        Tests::perftDivide(board, depth);
        break;

      case PerftMode::Suite:
        Tests::perftTestSuite(board, depth);
        break;

      case PerftMode::All:
        Tests::testAllPerft(board, depth);
        break;

      case PerftMode::Single:

      default:
        Tests::testSinglePerft(board, depth);
        break;
    }
  }

  void handleSearch(const std::string& line, Board& board, bool range) {
    std::stringstream ss(line);
    std::string cmd, d;
    ss >> cmd;

    if (!(ss >> d)) {
      std::cout << "error: search requires a depth\n";
      return;
    }

    int depth = std::stoi(d);

    if (range) {
      for (int i = 1; i <= depth; i++) {
        Tests::timeSearch(board, i);
      }
    } else {
      Tests::timeSearch(board, depth);
    }
  }

  void showState(Board& board, bool verbose) {
    std::string toMove = (board.getTurn() == WHITE) ? "White to move\n" : "Black to move\n";

    std::cout << "\n--Current Position--\n";
    std::cout << toMove << "\n";
    std::cout << "Current eval -> " << Eval::evaluate(board )<< "\n";
    board.printBoard();

    if (verbose) {
      std::cout << "\n--Current Board State--\n";

      std::cout << "White king pos -> " << board.getWhiteKingPos() << ", Black king pos -> " << board.getBlackKingPos() << "\n";

      std::cout << "White castling rights\n";
      std::cout << "Kingside -> "  << board.canCastleWhiteKingside()  << ", Queenside -> " << board.canCastleWhiteQueenside() << "\n";
      std::cout << "Black caslting rights\n";
      std::cout << "Kingside -> "  << board.canCastleBlackKingside()  << ", Queenside -> " << board.canCastleBlackQueenside() << "\n\n";

      std::cout << "En passant square -> " << board.getEnPassant() << "\n";

      std::cout << "Fullmove num -> " << board.getFullmove() << "\n";
      std::cout << "Halfmove num -> " << board.getHalfmove() << "\n\n";
    }
  }

  void showCommands() {
    std::cout << "\n--Commands--\n\n";
    
    std::cout << "'position startpos'      - reset board to start position\n";
    std::cout << "'position fen <fen-string>' - set position to <fen-string>\n\n";

    std::cout << "'moves <move1, move2, ...>' - apply moves to current position\n";
    std::cout << "'move  <move>' - apply move to current position\n\n";

    std::cout << "'perft <depth>' - perft test current position to <depth>\n";
    std::cout << "'perft-all <depth>' - perft test current position from depth = 1 to <depth>\n";
    std::cout << "'perft-suite <depth>' - run perft suite test to <depth>\n";
    std::cout << "'perft-divide <depth>' - perft divide test current position to <depth>\n";
    std::cout << "positions taken from: https://www.chessprogramming.org/Perft_Results\n\n";

    std::cout << "'c' or 'clear' - clear screen\n\n";

    std::cout << "'q' or 'quit' - quit program\n";
    std::cout << "'h' or 'help' - list commands\n\n";
  }

  std::optional<Move> parseMoveStr(const std::string& moveStr, Board& board) {
    if (moveStr.size() < 4 || moveStr.size() > 5) {
      std::cout << "error: invalid move string: '" << moveStr << "'\n";
      return std::nullopt;
    }

    auto fromOpt = strToSquare(moveStr.substr(0, 2));
    auto toOpt = strToSquare(moveStr.substr(2, 2));
    
    if (!fromOpt) { return std::nullopt; }
    if (!toOpt)   { return std::nullopt; }

    int from  = *fromOpt;
    int to    = *toOpt;
    int flags = FLAG_NONE;

    if (moveStr.length() == 5) {
      char promoPiece = moveStr[4];
      switch (promoPiece) {
        case 'q': flags |= (FLAG_PROMO_Q | FLAG_PROMOTION); break;
        case 'r': flags |= (FLAG_PROMO_R | FLAG_PROMOTION); break;
        case 'b': flags |= (FLAG_PROMO_B | FLAG_PROMOTION); break;
        case 'n': flags |= (FLAG_PROMO_N | FLAG_PROMOTION); break;
        default:
          std::cout << "error: invalid promotion piece\n";
          return std::nullopt;
      }
    }

    int currKing = (board.getTurn() == WHITE) ? W_KING : B_KING;
    if (board.getPiece(from) == currKing) {
      if (from == WHITE_KING_START && to == WHITE_KINGSIDE_END ) flags |= FLAG_CASTLE_WK;
      if (from == WHITE_KING_START && to == WHITE_QUEENSIDE_END) flags |= FLAG_CASTLE_WQ;
      if (from == BLACK_KING_START && to == BLACK_KINGSIDE_END ) flags |= FLAG_CASTLE_BK;
      if (from == BLACK_KING_START && to == BLACK_QUEENSIDE_END) flags |= FLAG_CASTLE_BQ;
    }

    int currPawn = (board.getTurn() == WHITE) ? W_PAWN : B_PAWN;
    if (board.getPiece(from) == currPawn) {
      if (std::abs(from - to) == 16) {
        flags |= FLAG_DOUBLE_PUSH;
      }
    }

    if (to == board.getEnPassant() && board.getPiece(from) == currPawn) {
      flags |= (FLAG_EN_PASSANT | FLAG_CAPTURE);
    } 

    if (board.getPiece(to) != EMPTY) {
      flags |= FLAG_CAPTURE;
    }

    return Move(from, to, flags);
  }
}