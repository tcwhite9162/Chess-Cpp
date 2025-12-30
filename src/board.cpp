#include <cstdint>
#include <iostream>
#include <cstdlib>

#include "../data/constants.hpp"
#include "moveGen.hpp"
#include "board.hpp"
#include "moveGen.hpp"
#include "zobrist.hpp"
#include "utils.hpp"

Board::Board() {
    squares.fill(EMPTY);
    turn = WHITE;
    castling = 0;
    en_passant = -1;
    halfmove = 0;
    fullmove = 1;
    whiteKingPosition = WHITE_KING_START;
    blackKingPosition = BLACK_KING_START;
    lastIrreversiblePly = 0;

    zobristKey = computeZobrist();
    repetitionHistory.clear();
    repetitionHistory.push_back(zobristKey);
}

void Board::setPiece(int square, int piece) {
    if (square >= 0 && square < 64) {
        squares[square] = piece;
    }
}

int Board::getPiece(int square) const {
    if (square >= 0 && square < 64) {
        return squares[square];
    }
    return EMPTY;
}

void Board::flipTurn() {
    turn = -turn;
}

void Board::printBoard() const {
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            std::cout << pieceToChar(squares[square]) << "  ";
        }
        std::cout << std::endl;
    }
}

void Board::setupStartPosition() {
    // pawns
    for (int i = 8; i < 16; i++) {
        squares[i] = B_PAWN;
        squares[i + 40] = W_PAWN;
    }
    //rooks
    squares[0] = B_ROOK;    squares[7] = B_ROOK;
    squares[56] = W_ROOK;   squares[63] = W_ROOK;

    //knights
    squares[1] = B_KNIGHT;  squares[6] = B_KNIGHT;
    squares[57] = W_KNIGHT; squares[62] = W_KNIGHT;

    // bishops
    squares[2] = B_BISHOP;  squares[5] = B_BISHOP;
    squares[58] = W_BISHOP; squares[61] = W_BISHOP;
    
    // queens
    squares[3] = B_QUEEN;   squares[59] = W_QUEEN;

    // kings
    squares[4] = B_KING;    squares[60] = W_KING;

    // reset state
    turn = WHITE;
    castling = CASTLE_WK | CASTLE_WQ | CASTLE_BK | CASTLE_BQ;
    en_passant = -1;
    halfmove = 0;
    fullmove = 1;
    whiteKingPosition = WHITE_KING_START;
    blackKingPosition = BLACK_KING_START;

    // reset hash
    zobristKey = computeZobrist();
    repetitionHistory.clear();
    repetitionHistory.push_back(zobristKey);

}

bool Board::isInCheck(int color) const {
    int kingPosition;
    kingPosition = (color == 1) ? whiteKingPosition : blackKingPosition;

    return isSquareAttacked(kingPosition, color * -1);
}

bool Board::isSquareAttacked(int square, int attackingColor) const {
    int squareRank = rank(square);
    int squareFile = file(square);
    
    // pawns
    if (attackingColor == 1) { // white attacking
        int offsets[2] = {DOWN_LEFT, DOWN_RIGHT};
        for (int offset : offsets) {
            int target = square + offset;
            if (isValidSquare(target)) {
                int targetFile = file(target);
                int dFile = targetFile - squareFile;

                if (abs(dFile) == 1 && squares[target] == W_PAWN) {
                    return true;
                }
            }
        }
    }
    else { // black attacking
        int offsets[2] = {UP_LEFT, UP_RIGHT};
        for (int offset : offsets) {
            int target = square + offset;
            if (isValidSquare(target)) {
                int targetFile = file(target);
                
                int dFile = abs(targetFile - squareFile);
                if (dFile == 1 && squares[target] == B_PAWN) {
                    return true;
                }
            }
        }
    }


    // knights
    for (int offset : KNIGHT_DIRS) {
        int target = square + offset;
        if (!(isValidSquare(target))) {
            continue;
        }
        int targetFile = file(target);
        
        int dFile = abs(targetFile - squareFile);
        if (dFile <= 2) {
            if (attackingColor == 1  && squares[target] == W_KNIGHT) { return true; }
            if (attackingColor == -1 && squares[target] == B_KNIGHT) { return true; }
        }
    }

    // bishops / queens (diagonal)
    for (int direction : BISHOP_DIRS) {
        int currentSquare = square + direction;
        while (isValidSquare(currentSquare)) {
            int currentFile = file(currentSquare);
            int prevFile    = file(currentSquare - direction);

            // file difference should always be one unless we wrap
            int dFile = abs(currentFile - prevFile);
            if (dFile != 1) { break; }

            int currentPiece = squares[currentSquare];
            if (currentPiece != EMPTY) {
                if      (attackingColor ==  1 && ((currentPiece == W_BISHOP) || (currentPiece == W_QUEEN))) { return true; }
                else if (attackingColor == -1 && ((currentPiece == B_BISHOP) || (currentPiece == B_QUEEN))) { return true; }
                else { break; } // break if we hit another piece
            }
            currentSquare += direction;
        }
    }

    // rooks / queens (orthogonal)
    for (int direction : ROOK_DIRS) {
        int currentSquare = square + direction;
        while (isValidSquare(currentSquare)) {
            int currentFile = file(currentSquare);

            if ((direction == LEFT)  && (currentFile == 7)) { break; } // moving left, break if we wrap from file 0 to 7
            if ((direction == RIGHT) && (currentFile == 0)) { break; } // same for moving right

            int currentPiece = squares[currentSquare];
            if (currentPiece != EMPTY) {
                if      (attackingColor ==  1 && ((currentPiece == W_ROOK) || (currentPiece == W_QUEEN))) { return true; }
                else if (attackingColor == -1 && ((currentPiece == B_ROOK) || (currentPiece == B_QUEEN))) { return true; }
                else { break; }
            }
            currentSquare += direction;
        }
    }

    // kings
    for (int direction : KING_DIRS) {
        int targetSquare = square + direction;
        if (!(isValidSquare(targetSquare))) { continue; }

        int targetFile = file(targetSquare);
        int dFile = abs(targetFile - squareFile);
        if (dFile <= 1) {
            int currentPiece = squares[targetSquare];
            if      (attackingColor ==  1 && currentPiece == W_KING) { return true; }
            else if (attackingColor == -1 && currentPiece == B_KING) { return true; }
        }
    }
    
    return false;
}

bool Board::enPassantAvailable() const{
    if (en_passant == -1) return false;

    int enPassantRank = en_passant / 8;
    int enPassantFile = en_passant % 8;

    int fileOffsets[2] = {1, -1};
    for (int dFile : fileOffsets) {
        int adjacentFile = enPassantFile + dFile;
        if (adjacentFile < 0 || adjacentFile > 7) continue;

        int pawnRank = (turn == WHITE) ? enPassantRank + 1 : enPassantRank - 1;
        if (pawnRank < 0 || pawnRank > 7) continue;

        int square = pawnRank * 8 + adjacentFile;
        int pawn = (turn == WHITE) ? W_PAWN : B_PAWN;

        if (squares[square] == pawn) return true;
    }

    return false;
}

bool Board::hasLegalMoves() {
    MoveList moves;
    generateLegalMoves(*this, moves);
    return (moves.count > 0);
}

bool Board::isCheckmate() {
    return (isInCheck(turn) && !hasLegalMoves());
}

bool Board::isDraw() {
    return (isStalemate() ||
            isThreefoldRepetition() ||
            isInsuffucientMaterial() ||
            halfmove >= 100);
}

bool Board::isStalemate() {
    return (!isInCheck(turn) && !hasLegalMoves());
}

bool Board::isThreefoldRepetition() {
    uint64_t current = zobristKey;
    int count = 0;

    for (int i = lastIrreversiblePly; i < (int)repetitionHistory.size(); i++) {
        if (repetitionHistory[i] == current) {
            count++;
            if (count >= 3) return true;
        }
    }
    return false;
}

bool Board::isInsuffucientMaterial() {
    int whiteBishops = 0; int blackBishops = 0;
    int whiteKnights = 0; int blackKnights = 0;
    bool whiteBishopLight = false; bool whiteBishopDark = false; 
    bool blackBishopLight = false; bool blackBishopDark = false; 

    for (int sq = 0; sq < 64; sq++) {
        int piece = squares[sq];
        if (piece == EMPTY) continue;

        switch (piece) {
            // pawns/rooks/queens always have possible mate
            case W_PAWN: case B_PAWN:
            case W_ROOK: case B_ROOK:
            case W_QUEEN: case B_QUEEN:
                return false;

            case W_BISHOP:
                whiteBishops++;
                // if row + col % 2 == 0, bishop is light
                if ((sq / 8 + sq % 8) % 2 == 0) whiteBishopLight = true;
                else whiteBishopDark = true;
                break;
            
            case B_BISHOP:
                blackBishops++;
                if ((sq / 8 + sq % 8) % 2 == 0) blackBishopLight = true;
                else blackBishopDark = true;
                break;

            case W_KNIGHT: whiteKnights++; break;
            case B_KNIGHT: blackKnights++; break;

            default: break; // kings dont matter
        }
    }

    int totalBishops = whiteBishops + blackBishops;
    int totalKnights = whiteKnights + blackKnights;

    // king vs king
    if (totalBishops == 0 && totalKnights == 0) return true;

    // king + bishop vs king
    if (totalBishops == 1 && totalKnights == 0) return true;

    // king + knight vs king
    if (totalBishops == 0 && totalKnights == 1) return true;

    // king + bishop vs king + bishop (same color bishops)
    if (totalBishops == 2 && totalKnights == 0) {
        if ((whiteBishopLight && blackBishopLight) ||
            (whiteBishopDark && blackBishopDark)) {
            return true;
        }
    }

    return false;
}

bool Board::isTerminal() {
    return (isDraw() || isCheckmate());
}

void Board::updateCastlingRights(int fromSquare, int toSquare, int movedPiece, int capturedPiece) {
    // clear castling if king moves
    if (movedPiece == W_KING)      { castling &= ~(CASTLE_WK | CASTLE_WQ); }
    else if (movedPiece == B_KING) { castling &= ~(CASTLE_BK | CASTLE_BQ); }

    // if rook moves
    if (fromSquare == W_ROOK_H1) { castling &= ~CASTLE_WK; }
    if (fromSquare == W_ROOK_A1) { castling &= ~CASTLE_WQ; }
    if (fromSquare == B_ROOK_H8) { castling &= ~CASTLE_BK; }
    if (fromSquare == B_ROOK_A8) { castling &= ~CASTLE_BQ; }

    // rook captured on starting square
    if (capturedPiece == W_ROOK) {
        if (toSquare == W_ROOK_H1) castling &= ~CASTLE_WK;
        if (toSquare == W_ROOK_A1) castling &= ~CASTLE_WQ;
    }
    else if (capturedPiece == B_ROOK) {
        if (toSquare == B_ROOK_H8) castling &= ~CASTLE_BK;
        if (toSquare == B_ROOK_A8) castling &= ~CASTLE_BQ;
    }
}

void Board::makeMove(Move move, bool updateHash) {
    int fromSquare = move.from_square();
    int toSquare   = move.to_square();
    int flags      = move.flags();

    int piece    = squares[fromSquare];
    int captured = squares[toSquare];

    undoInfo ui {
        move,
        piece,
        captured,
        castling,
        en_passant,
        halfmove,
        fullmove,
        whiteKingPosition,
        blackKingPosition
    };
    history.push_back(ui);

    squares[fromSquare] = EMPTY;

    if (isPromotion(flags)) {
        squares[toSquare] = promotionPiece(flags, turn);}
    else {
        squares[toSquare] = piece;
    }

    // default en passant before checking flags
    en_passant = -1;
    if (flags & FLAG_EN_PASSANT && !isPromotion(flags)) {
        squares[toSquare] = piece;
        squares[(turn == WHITE) ? toSquare + DOWN : toSquare + UP] = EMPTY;
    }

    if (flags & FLAG_DOUBLE_PUSH && !isPromotion(flags)) {
        squares[toSquare] = piece;
        en_passant = (turn == WHITE) ? toSquare + DOWN : toSquare + UP;
    }
    
    if (flags & FLAG_CASTLE_WK && !isPromotion(flags)) {
        squares[W_ROOK_H1] = EMPTY;
        squares[W_ROOK_F1_END] = W_ROOK;
        whiteKingPosition = toSquare;
    }

    if (flags & FLAG_CASTLE_WQ && !isPromotion(flags)) {
        squares[W_ROOK_A1] = EMPTY;
        squares[W_ROOK_D1_END] = W_ROOK;
        whiteKingPosition = toSquare;
    }

    if (flags & FLAG_CASTLE_BK && !isPromotion(flags)) {
        squares[B_ROOK_H8] = EMPTY;
        squares[B_ROOK_F8_END] = B_ROOK;
        blackKingPosition = toSquare;
    }

    if (flags & FLAG_CASTLE_BQ && !isPromotion(flags)) {
        squares[B_ROOK_A8] = EMPTY;
        squares[B_ROOK_D8_END] = B_ROOK;
        blackKingPosition = toSquare;
    }

    if (piece == W_KING) whiteKingPosition = toSquare;
    if (piece == B_KING) blackKingPosition = toSquare;

    halfmove = (piece == W_PAWN || piece == B_PAWN || captured != EMPTY) ? 0 : halfmove + 1;
    if (turn == BLACK) fullmove++;

    updateCastlingRights(fromSquare, toSquare, piece, captured);

    flipTurn();

    if (piece == W_PAWN || piece == B_PAWN || captured != EMPTY) {
        lastIrreversiblePly = repetitionHistory.size();
    }

    if (updateHash) {
        // remove piece from fromSquare
        zobristKey ^= Zobrist::piece[pieceToIndex(piece)][fromSquare];

        // remove captured piece
        if (captured != EMPTY) {
            zobristKey ^= Zobrist::piece[pieceToIndex(captured)][toSquare];
        }

        // add moved piece to toSquare
        int newPiece = squares[toSquare];
        zobristKey ^= Zobrist::piece[pieceToIndex(newPiece)][toSquare];

        // castling rights
        zobristKey ^= Zobrist::castling[ui.castlingRights];
        zobristKey ^= Zobrist::castling[castling];

        // en passant
        if (ui.enPassantSquare != -1) {
            zobristKey ^= Zobrist::enpassant[ui.enPassantSquare % 8];
        }

        if (en_passant != -1) {
            zobristKey ^= Zobrist::enpassant[en_passant % 8];
        }

        // side to move
        zobristKey ^= Zobrist::side;

        repetitionHistory.push_back(zobristKey);
    }
}

void Board::unmakeMove(Move move, bool updateHash) {
    if (history.empty()) return;

    undoInfo ui = history.back();
    history.pop_back();

    int flags      = ui.move.flags();
    int fromSquare = move.from_square();
    int toSquare   = move.to_square();
    int piece      = squares[toSquare];
    
    // flip turn before undoing en passant
    flipTurn(); // now 'turn' is the side that just moved
    if (flags & FLAG_EN_PASSANT) {
        int capSq = (turn == WHITE) ? toSquare + DOWN : toSquare + UP;
        squares[capSq] = (turn == WHITE ? B_PAWN : W_PAWN);
    }


    if (isPromotion(flags)) {
        squares[fromSquare] = (turn == WHITE ? W_PAWN : B_PAWN);}
    else {
        squares[fromSquare] = piece;
    }

    if (isCapture(flags)) {
        squares[toSquare] = ui.capturedPiece;}
    else {
        squares[toSquare] = EMPTY;
    }

    if (flags & FLAG_CASTLE_WK) { squares[W_ROOK_H1] = W_ROOK; squares[W_ROOK_F1_END] = EMPTY; }
    if (flags & FLAG_CASTLE_WQ) { squares[W_ROOK_A1] = W_ROOK; squares[W_ROOK_D1_END] = EMPTY; }
    if (flags & FLAG_CASTLE_BK) { squares[B_ROOK_H8] = B_ROOK; squares[B_ROOK_F8_END] = EMPTY; }
    if (flags & FLAG_CASTLE_BQ) { squares[B_ROOK_A8] = B_ROOK; squares[B_ROOK_D8_END] = EMPTY; }

    // restore state
    castling   = ui.castlingRights;
    en_passant = ui.enPassantSquare;
    halfmove   = ui.halfmoveClock;
    fullmove   = ui.fullmoveNumber;
    
    whiteKingPosition = ui.whiteKingPos;
    blackKingPosition = ui.blackKingPos;

    if (updateHash) {
        // side to move
        zobristKey ^= Zobrist::side;

        // en passant
        if (en_passant != -1) {
            zobristKey ^= Zobrist::enpassant[en_passant];
        }
        if (ui.enPassantSquare != -1) {
            zobristKey ^= Zobrist::enpassant[ui.enPassantSquare];
        }

        // castling
        zobristKey ^= Zobrist::castling[castling];
        zobristKey ^= Zobrist::castling[ui.castlingRights];

        // remove piece from fromSquare
        int piece = squares[fromSquare];
        zobristKey ^= Zobrist::piece[pieceToIndex(piece)][fromSquare];

        // add piece back to toSquare
        if (ui.capturedPiece != EMPTY) {
            zobristKey ^= Zobrist::piece[pieceToIndex(ui.capturedPiece)][toSquare];
        }
        zobristKey ^= Zobrist::piece[pieceToIndex(ui.movedPiece)][toSquare];

        // pop
        repetitionHistory.pop_back();

    }
}

uint64_t Board::computeZobrist() const {
    uint64_t key = 0;

    // pieces
    for (int sq = 0; sq < 64; sq++) {
        int piece = squares[sq];
        if (piece != EMPTY) {
            int index = pieceToIndex(piece);
            key ^= Zobrist::piece[index][sq];
        }
    }

    // castling
    key ^= Zobrist::castling[castling];

    // en passant file
    if (en_passant != -1) {
        int file = en_passant % 8;
        key ^= Zobrist::enpassant[file];
    }

    // side to move
    if (turn == BLACK) {
        key ^= Zobrist::side;
    }

    return key;
}