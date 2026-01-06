#include "interface.hpp"
#include "board.hpp"
#include "tests.hpp"

#include <iostream>
#include <sstream>
#include <string>

namespace Interface {
  inline void trim(std::string& s) {
    while (!s.empty() && std::isspace(s[0])) {
      s.erase(0, 1);
    }
  }

  inline int strToSquare(const std::string& s) {
    int file = s[0] - 'a';
    int rank = s[1] - '1';
    return rank * 8 + file;
  }

  inline void clearScreen() {
    std::cout << "\033[2J\033[1;1H" << std::flush;
  }
 
  void inputLoop(Board& board) {
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

    if (line.rfind("position", 0) == 0) {
      handlePosition(line, board);
      return;
    }

    if (line.rfind("moves", 0) == 0) {
      handleMoves(line, board);
      return;
    }

    if (line.rfind("move", 0) == 0) {
      handleMove(line, board);
      return;
    }

    if (line.rfind("perft-d", 0) == 0) {
      handlePerft(line, board, true);
      return;
    }

    if (line.rfind("perft-s", 0) == 0) {
      handlePerft(line, board, false, true);
      return;
    }

    if (line.rfind("perft-a", 0) == 0) {
      handlePerft(line, board, false, false, true);
      return;
    }

    if (line.rfind("perft", 0) == 0) {
      handlePerft(line, board);
      return;
    }

    if (line == "d") {
      showState(board);
      return;
    }

    if (line == "h") {
      showCommands();
      return;
    }

    if (line == "clear" || line == "c") {
      clearScreen();
      return;
    }

    if (line == "") {
      return;
    }

    std::cout << "invalid command\n enter \'h\' to list commands\n";
  }

  void handlePosition(const std::string& line, Board& board) {
    std::stringstream ss(line);
    std::string token;
    ss >> token; // 'position'

    ss >> token;
    if (token == "startpos") {
      board.setupStartPosition();

      ss >> token;
      if (token == "moves") {
        std::string moveStr;
        while (ss >> moveStr) {
          Move m = parseMoveStr(moveStr, board);
          board.makeMove(m, true);
        }
      }
    }

    else if (token == "fen") {
      std::string fenStr;
      std::getline(ss, fenStr);

      trim(fenStr);
      
      board.setupFromFen(fenStr);
    }
  }

  void handleMove(const std::string& line, Board& board) {
    std::stringstream ss(line);
    std::string cmd, moveStr;
    ss >> cmd >> moveStr;

    Move m = parseMoveStr(moveStr, board);
    board.makeMove(m, true);
  }

  void handleMoves(const std::string& line, Board& board) {
    std::stringstream ss(line);
    std::string cmd, moveStr;
    ss >> cmd; // "moves"

    while (ss >> moveStr) {
      Move m = parseMoveStr(moveStr, board);
      board.makeMove(m, true);
    }
  }

  void handlePerft(const std::string& line, Board& board, bool divide, bool suite, bool all) {
    std::stringstream ss(line);
    std::string cmd, d;
    ss >> cmd;

    if (!(ss >> d)) {
      std::cout << "error: perft requires a depth\n";
      return;
    }

    int depth = std::stoi(d);

    if (divide) {
      perft_divide(board, depth);
    }
    else if (suite) {
      perftTestSuite(board, depth);
    }
    else if (all) {
      testPerft(board, depth);
    }
    else {
      testSinglePerft(board, depth);
    }
  }

  void showState(Board& board) {
    std::cout << "\n--Current Position--\n";
    board.printBoard();
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

  void showCommands() {
    std::cout << "\n--Commands--\n\n";
    
    std::cout << "'position startpos'      - reset board to start position\n";
    std::cout << "'position fen <fen-str>' - set position to <fen-str>\n\n";

    std::cout << "'moves <move1, move2, ...>' - apply moves to current position\n";
    std::cout << "'move  <move>' - apply move to current position\n\n";

    std::cout << "'perft   <depth>' - perft test current position to <depth>\n";
    std::cout << "'perft-d <depth>' - perft divide test current position to <depth>\n";
    std::cout << "'perft-a <depth>' - perft test current position from depth = 1 to <depth>\n";
    std::cout << "'perft-s <depth>' - run perft suite test to <depth>\n";
    std::cout << "positions taken from: https://www.chessprogramming.org/Perft_Results\n\n";

    std::cout << "'clear' or 'c' - clear screen\n\n";

    std::cout << "'q' or 'quit' - quit program\n";
    std::cout << "'h' - list commands\n\n";
  }

  Move parseMoveStr(const std::string& moveStr, Board& board) {
    int from = strToSquare(moveStr.substr(0, 2));
    int to = strToSquare(moveStr.substr(2, 2));
    int flags = FLAG_NONE;

    if (moveStr.length() == 5) {
      char promoPiece = moveStr[4];
      switch (promoPiece) {
        case 'q': flags |= (FLAG_PROMO_Q | FLAG_PROMOTION); break;
        case 'r': flags |= (FLAG_PROMO_R | FLAG_PROMOTION); break;
        case 'b': flags |= (FLAG_PROMO_B | FLAG_PROMOTION); break;
        case 'n': flags |= (FLAG_PROMO_N | FLAG_PROMOTION); break;
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
