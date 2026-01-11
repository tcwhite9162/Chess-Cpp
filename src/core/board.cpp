#include <cctype>
#include <iostream>
#include <sstream>


#include "board.hpp"
#include "utils.hpp"
#include "movegen/moveGen.hpp"

Board::Board() {
    squares.fill(EMPTY);
    turn = WHITE;
    castling = 0;
    en_passant = -1;
    halfmove = 0;
    fullmove = 1;
    whiteKingPosition = 0;
    blackKingPosition = 0;
    lastIrreversiblePly = 0;

    whitePawns   = 0ULL;
    whiteKnights = 0ULL;
    whiteBishops = 0ULL;
    whiteRooks   = 0ULL;
    whiteQueens  = 0ULL;
    whiteKing    = 0ULL;

    blackPawns   = 0ULL;
    blackKnights = 0ULL;
    blackBishops = 0ULL;
    blackRooks   = 0ULL;
    blackQueens  = 0ULL;
    blackKing    = 0ULL;

    occupancyWhite = 0ULL;
    occupancyBlack = 0ULL;
    occupancyAll   = 0ULL;

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
    std::cout << "  ┌───┬───┬───┬───┬───┬───┬───┬───┐\n";
    
    for (int rank = 0; rank < 8; ++rank) {
        std::cout << 8 - rank << " ";

        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            std::cout << "| " << pieceToChar(squares[square]) << " ";
        }
        std::cout << "|\n";
        
        if (rank < 7) {
            std::cout << "  ├───┼───┼───┼───┼───┼───┼───┼───┤\n";
        } else {
            std::cout << "  └───┴───┴───┴───┴───┴───┴───┴───┘\n";
        }
    }
    std::cout << "   a   b   c   d   e   f   g   h\n";
}

void Board::setupStartPosition() {
    setupFromFen(startFen);
}

void Board::setupFromFen(const std::string& fen) {
    // clear everything
    squares.fill(EMPTY);

    // reset bitboards
    whitePawns   = blackPawns   = 0ULL;
    whiteKnights = blackKnights = 0ULL;
    whiteBishops = blackBishops = 0ULL;
    whiteRooks   = blackRooks   = 0ULL;
    whiteQueens  = blackQueens  = 0ULL;
    whiteKing    = blackKing    = 0ULL;

    occupancyWhite = occupancyBlack = occupancyAll = 0ULL;

    std::stringstream ss(fen);
    std::string boardPart, turnPart, castlingPart, epPart, halfmovePart, fullmovePart;

    ss >> boardPart >> turnPart >> castlingPart >> epPart >> halfmovePart >> fullmovePart;

    int squareIndex = 0;
    for (char c : boardPart) {
        if (c == '/') {
            continue;
        }
        else if (std::isdigit(c)) {
            squareIndex += (c - '0');
        }
        else {
            switch (c) {
                case 'p': setPieceAndBitboard(squareIndex, B_PAWN);   break;
                case 'n': setPieceAndBitboard(squareIndex, B_KNIGHT); break;
                case 'b': setPieceAndBitboard(squareIndex, B_BISHOP); break;
                case 'r': setPieceAndBitboard(squareIndex, B_ROOK);   break;
                case 'q': setPieceAndBitboard(squareIndex, B_QUEEN);  break;
                case 'k': setPieceAndBitboard(squareIndex, B_KING); blackKingPosition = squareIndex;   break;

                case 'P': setPieceAndBitboard(squareIndex, W_PAWN);   break;
                case 'N': setPieceAndBitboard(squareIndex, W_KNIGHT); break;
                case 'B': setPieceAndBitboard(squareIndex, W_BISHOP); break;
                case 'R': setPieceAndBitboard(squareIndex, W_ROOK);   break;
                case 'Q': setPieceAndBitboard(squareIndex, W_QUEEN);  break;
                case 'K': setPieceAndBitboard(squareIndex, W_KING); whiteKingPosition = squareIndex;   break;
            }
            squareIndex++;
        }
    }

    turn = (turnPart == "w") ? WHITE : BLACK;

    castling = 0;
    if (castlingPart.find('K') != std::string::npos) castling |= CASTLE_WK; 
    if (castlingPart.find('Q') != std::string::npos) castling |= CASTLE_WQ; 
    if (castlingPart.find('k') != std::string::npos) castling |= CASTLE_BK; 
    if (castlingPart.find('q') != std::string::npos) castling |= CASTLE_BQ; 

    if (epPart == "-") {
        en_passant = -1;
    }
    else {
        int file   = (epPart[0] - 'a');
        int rank   = (epPart[1] - '0');
        en_passant = rank * 8 + file;
    }

    halfmove = (halfmovePart == "") ? 0 : std::stoi(halfmovePart);
    fullmove = (fullmovePart == "") ? 1 : std::stoi(fullmovePart);

    updateOccupancy();

    zobristKey = computeZobrist();
    repetitionHistory.clear();
    repetitionHistory.push_back(zobristKey);
}

void Board::setPieceAndBitboard(int square, int piece) {
    squares[square] = piece;
    u64 mask = 1ULL << square;

    // clear square from all bitboards
    whitePawns   &= ~mask; blackPawns   &= ~mask;
    whiteKnights &= ~mask; blackKnights &= ~mask;
    whiteBishops &= ~mask; blackBishops &= ~mask;
    whiteRooks   &= ~mask; blackRooks   &= ~mask;
    whiteQueens  &= ~mask; blackQueens  &= ~mask;
    whiteKing    &= ~mask; blackKing    &= ~mask;

    // add to corresponding bitboard
    switch (piece) {
        case EMPTY: return;

        case W_PAWN:   whitePawns   |= mask; break;
        case W_KNIGHT: whiteKnights |= mask; break;
        case W_BISHOP: whiteBishops |= mask; break;
        case W_ROOK:   whiteRooks   |= mask; break;
        case W_QUEEN:  whiteQueens  |= mask; break;
        case W_KING:   whiteKing    |= mask; break;

        case B_PAWN:   blackPawns   |= mask; break;   
        case B_KNIGHT: blackKnights |= mask; break;
        case B_BISHOP: blackBishops |= mask; break;
        case B_ROOK:   blackRooks   |= mask; break;
        case B_QUEEN:  blackQueens  |= mask; break;
        case B_KING:   blackKing    |= mask; break;
    }
}

void Board::removePieceBB(int square, int piece) {
    u64 mask = ~(1ULL << square);

    switch (piece) {
        case W_PAWN:   whitePawns   &= mask; break;
        case W_KNIGHT: whiteKnights &= mask; break;
        case W_BISHOP: whiteBishops &= mask; break;
        case W_ROOK:   whiteRooks   &= mask; break;
        case W_QUEEN:  whiteQueens  &= mask; break;
        case W_KING:   whiteKing    &= mask; break;

        case B_PAWN:   blackPawns   &= mask; break;   
        case B_KNIGHT: blackKnights &= mask; break;
        case B_BISHOP: blackBishops &= mask; break;
        case B_ROOK:   blackRooks   &= mask; break;
        case B_QUEEN:  blackQueens  &= mask; break;
        case B_KING:   blackKing    &= mask; break;
    }
}

void Board::addPieceBB(int square, int piece) {
    u64 mask = 1ULL << square;

    switch (piece) {
        case W_PAWN:   whitePawns   |= mask; break;
        case W_KNIGHT: whiteKnights |= mask; break;
        case W_BISHOP: whiteBishops |= mask; break;
        case W_ROOK:   whiteRooks   |= mask; break;
        case W_QUEEN:  whiteQueens  |= mask; break;
        case W_KING:   whiteKing    |= mask; break;

        case B_PAWN:   blackPawns   |= mask; break;   
        case B_KNIGHT: blackKnights |= mask; break;
        case B_BISHOP: blackBishops |= mask; break;
        case B_ROOK:   blackRooks   |= mask; break;
        case B_QUEEN:  blackQueens  |= mask; break;
        case B_KING:   blackKing    |= mask; break;
    }

}

void Board::movePieceBB(int from, int to, int piece) {
    removePieceBB(from, piece);
    addPieceBB(to, piece);
}

void Board::removeCapturedPieceBB(int square, int piece) {
    removePieceBB(square, piece);
}

void Board::updateOccupancy() {
    occupancyWhite = whitePawns | whiteKnights | whiteBishops |whiteRooks | whiteQueens | whiteKing;
    occupancyBlack = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;

    occupancyAll = occupancyWhite | occupancyBlack;
}

bool Board::isInCheck(int color) const {
    int kingSq = (color == WHITE) ? whiteKingPosition : blackKingPosition;
    int enemy = (color == WHITE ? BLACK : WHITE);
    return isSquareAttacked(kingSq, enemy);
}

bool Board::isSquareAttacked(int square, int attackingColor) const {
    u64 occ = occupancyAll;

    int r = square / 8;
    int f = square % 8;


    // pawns
    if (attackingColor == WHITE) {
        u64 attackers = 0ULL;

        // White pawns attack from squares one rank *below* the target:
        // from = square + 7 (down-left) or square + 9 (down-right)
        if (r < 7 && f > 0) attackers |= 1ULL << (square + 7);  // from file-1, rank+1 (d2 -> e3)
        if (r < 7 && f < 7) attackers |= 1ULL << (square + 9);  // from file+1, rank+1 (f2 -> e3)

        if (whitePawns & attackers) return true;

    } else { // attackingColor == BLACK
        u64 attackers = 0ULL;

        // Black pawns attack from squares one rank *above* the target:
        // from = square - 7 (up-right) or square - 9 (up-left)
        if (r > 0 && f < 7) attackers |= 1ULL << (square - 7);  // from file+1, rank-1 (e4 -> d5)
        if (r > 0 && f > 0) attackers |= 1ULL << (square - 9);  // from file-1, rank-1 (c4 -> d5)

        if (blackPawns & attackers) return true;
    }

    // knights
    if (attackingColor == WHITE) {
        if (whiteKnights & knightAttacks[square]) return true;
    } else {
        if (blackKnights & knightAttacks[square]) return true;
    }

    // bishops + queens (diagonal)
    if (attackingColor == WHITE) {
        if (bishopAttacksMagic(square, occ) & (whiteBishops | whiteQueens)) return true;
    } else {
        if (bishopAttacksMagic(square, occ) & (blackBishops | blackQueens)) return true;
    }

    // rooks + queens (orthogonal)
    if (attackingColor == WHITE) {
        if (rookAttacksMagic(square, occ) & (whiteRooks | whiteQueens)) return true;
    } else {
        if (rookAttacksMagic(square, occ) & (blackRooks | blackQueens)) return true;
    }

    // kings
    if (attackingColor == WHITE) {
        if (whiteKing & kingAttacks[square]) return true;
    } else {
        if (blackKing & kingAttacks[square]) return true;
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

int Board::getEnPassantCaptureSquare() const {
    if (en_passant == -1)
        return -1;

    int capSq = (turn == WHITE) ? en_passant + DOWN : en_passant + UP;

    if (capSq < 0 || capSq >= 64)
        return -1;

    return capSq;
}

bool Board::hasLegalMoves() {
    MoveList moves;
    MoveGen::generateLegalMoves(*this, moves);
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

    int piece = squares[fromSquare];

    // compute en-passant capture square if this move is en-passant
    int capSq = -1;
    if (flags & FLAG_EN_PASSANT) {
        capSq = (turn == WHITE) ? toSquare + DOWN : toSquare + UP;
    }

    // captured piece: for en-passant the captured pawn is on capSq, otherwise on toSquare
    int captured = (flags & FLAG_EN_PASSANT) ? (isValidSquare(capSq) ? squares[capSq] : EMPTY) : squares[toSquare];

    // Save undo info (store previous castling/enpassant/halfmove/fullmove and king positions)
    undoInfo ui {
        move,
        piece,          // movedPiece
        captured,       // capturedPiece (may be EMPTY)
        capSq,          // capturedSquare (for en-passant or -1)
        castling,       // castlingRights (before move)
        en_passant,     // enPassantSquare (before move)
        halfmove,
        fullmove,
        whiteKingPosition,
        blackKingPosition
    };
    history.push_back(ui);

    // Clear mailbox from-square immediately (we'll set destination below)
    squares[fromSquare] = EMPTY;

    // Clear en_passant by default; will set if double push
    en_passant = -1;

    // Remove captured piece from mailbox/bitboards BEFORE placing/moving the moving piece
    if (flags & FLAG_EN_PASSANT) {
        if (isValidSquare(capSq) && captured != EMPTY) {
            // captured pawn is on capSq (behind the toSquare)
            int capPiece = (turn == WHITE) ? B_PAWN : W_PAWN;
            squares[capSq] = EMPTY;
            removePieceBB(capSq, capPiece);
        }
    } else if (captured != EMPTY) {
        // normal capture on toSquare
        squares[toSquare] = EMPTY; // will be overwritten below
        removePieceBB(toSquare, captured);
    }

    // Handle promotion
    if (isPromotion(flags)) {
        int promo = promotionPiece(flags, turn); // e.g. W_QUEEN or B_QUEEN
        // remove pawn from fromSquare bitboard
        removePieceBB(fromSquare, (turn == WHITE ? W_PAWN : B_PAWN));
        // place promoted piece on toSquare (mailbox + bitboard)
        squares[toSquare] = promo;
        addPieceBB(toSquare, promo);
    } else {
        // Normal move: move bitboard entry and mailbox
        movePieceBB(fromSquare, toSquare, piece);
        squares[toSquare] = piece;
    }

    // Handle double pawn push -> set en_passant square
    if (flags & FLAG_DOUBLE_PUSH) {
        en_passant = (turn == WHITE) ? toSquare + DOWN : toSquare + UP;
    }

    // Handle castling rook moves (mailbox + bitboards)
    if (flags & FLAG_CASTLE_WK) {
        // White king-side: rook from h1 -> f1
        squares[W_ROOK_H1] = EMPTY;
        squares[W_ROOK_F1_END] = W_ROOK;
        movePieceBB(W_ROOK_H1, W_ROOK_F1_END, W_ROOK);
        whiteKingPosition = toSquare;
    }
    if (flags & FLAG_CASTLE_WQ) {
        // White queen-side: rook from a1 -> d1
        squares[W_ROOK_A1] = EMPTY;
        squares[W_ROOK_D1_END] = W_ROOK;
        movePieceBB(W_ROOK_A1, W_ROOK_D1_END, W_ROOK);
        whiteKingPosition = toSquare;
    }
    if (flags & FLAG_CASTLE_BK) {
        // Black king-side: rook from h8 -> f8
        squares[B_ROOK_H8] = EMPTY;
        squares[B_ROOK_F8_END] = B_ROOK;
        movePieceBB(B_ROOK_H8, B_ROOK_F8_END, B_ROOK);
        blackKingPosition = toSquare;
    }
    if (flags & FLAG_CASTLE_BQ) {
        // Black queen-side: rook from a8 -> d8
        squares[B_ROOK_A8] = EMPTY;
        squares[B_ROOK_D8_END] = B_ROOK;
        movePieceBB(B_ROOK_A8, B_ROOK_D8_END, B_ROOK);
        blackKingPosition = toSquare;
    }

    // Update king position for non-castling king moves
    if (piece == W_KING && !(flags & (FLAG_CASTLE_WK | FLAG_CASTLE_WQ))) {
        whiteKingPosition = toSquare;
    }
    if (piece == B_KING && !(flags & (FLAG_CASTLE_BK | FLAG_CASTLE_BQ))) {
        blackKingPosition = toSquare;
    }

    // Rebuild occupancy
    updateOccupancy();

    // halfmove/fullmove bookkeeping
    halfmove = (piece == W_PAWN || piece == B_PAWN || captured != EMPTY) ? 0 : halfmove + 1;
    if (turn == BLACK) fullmove++;

    // update castling rights (uses moved piece and captured piece)
    updateCastlingRights(fromSquare, toSquare, piece, captured);

    // flip side to move
    flipTurn();

    // update last irreversible ply (for repetition)
    if (piece == W_PAWN || piece == B_PAWN || captured != EMPTY) {
        lastIrreversiblePly = repetitionHistory.size();
    }

    // update zobrist and repetition history
    if (updateHash) {
        // XOR out piece from fromSquare (use ui.movedPiece)
        zobristKey ^= Zobrist::piece[pieceToIndex(ui.movedPiece)][fromSquare];

        // XOR out captured piece (use ui.capturedPiece and ui.capturedSquare for en-passant)
        if (ui.capturedPiece != EMPTY) {
            if (ui.capturedSquare != -1 && (flags & FLAG_EN_PASSANT)) {
                zobristKey ^= Zobrist::piece[pieceToIndex(ui.capturedPiece)][ui.capturedSquare];
            } else {
                zobristKey ^= Zobrist::piece[pieceToIndex(ui.capturedPiece)][toSquare];
            }
        }

        // XOR in moved piece at toSquare (note promotions: squares[toSquare] already holds the new piece)
        // int newPiece = squares[toSquare];
        int newPiece = isPromotion(flags) ? promotionPiece(flags, turn) : piece;
        zobristKey ^= Zobrist::piece[pieceToIndex(newPiece)][toSquare];

        // castling rights: XOR old and new
        zobristKey ^= Zobrist::castling[ui.castlingRights];
        zobristKey ^= Zobrist::castling[castling];

        // en passant: XOR old and new (use file index)
        if (ui.enPassantSquare != -1) {
            zobristKey ^= Zobrist::enpassant[ui.enPassantSquare % 8];
        }
        if (en_passant != -1) {
            zobristKey ^= Zobrist::enpassant[en_passant % 8];
        }

        // side to move
        zobristKey ^= Zobrist::side;

        // push new key into repetition history
        repetitionHistory.push_back(zobristKey);
    }
}

void Board::unmakeMove(Move move, bool updateHash) {
    if (history.empty()) return;

    // Pop undo info
    undoInfo ui = history.back();
    history.pop_back();

    int flags      = ui.move.flags();
    int fromSquare = ui.move.from_square();
    int toSquare   = ui.move.to_square();
    int movedPiece = ui.movedPiece;
    int captured   = ui.capturedPiece;
    int capSq      = ui.capturedSquare;

    // flip side back
    flipTurn();

    // Undo castling rook moves first (bitboards + mailbox)
    if (flags & FLAG_CASTLE_WK) {
        movePieceBB(W_ROOK_F1_END, W_ROOK_H1, W_ROOK);
        squares[W_ROOK_H1] = W_ROOK;
        squares[W_ROOK_F1_END] = EMPTY;
    }
    if (flags & FLAG_CASTLE_WQ) {
        movePieceBB(W_ROOK_D1_END, W_ROOK_A1, W_ROOK);
        squares[W_ROOK_A1] = W_ROOK;
        squares[W_ROOK_D1_END] = EMPTY;
    }
    if (flags & FLAG_CASTLE_BK) {
        movePieceBB(B_ROOK_F8_END, B_ROOK_H8, B_ROOK);
        squares[B_ROOK_H8] = B_ROOK;
        squares[B_ROOK_F8_END] = EMPTY;
    }
    if (flags & FLAG_CASTLE_BQ) {
        movePieceBB(B_ROOK_D8_END, B_ROOK_A8, B_ROOK);
        squares[B_ROOK_A8] = B_ROOK;
        squares[B_ROOK_D8_END] = EMPTY;
    }

    // Undo promotion or normal move
    if (isPromotion(flags)) {
        // promoted piece is on toSquare; remove it
        int promo = promotionPiece(flags, (ui.movedPiece > 0) ? WHITE : BLACK);
        removePieceBB(toSquare, promo);
        squares[toSquare] = EMPTY;

        // restore pawn on fromSquare (movedPiece indicates color)
        int pawn = (ui.movedPiece > 0) ? W_PAWN : B_PAWN;
        addPieceBB(fromSquare, pawn);
        squares[fromSquare] = pawn;
    } else {
        // move piece back from toSquare to fromSquare
        movePieceBB(toSquare, fromSquare, movedPiece);
        squares[fromSquare] = movedPiece;
        squares[toSquare] = EMPTY;
    }

    // Restore captured piece
    if (captured != EMPTY) {
        if (flags & FLAG_EN_PASSANT) {
            // captured pawn was on capSq
            if (isValidSquare(capSq)) {
                addPieceBB(capSq, captured);
                squares[capSq] = captured;
            }
        } else {
            // normal capture on toSquare
            addPieceBB(toSquare, captured);
            squares[toSquare] = captured;
        }
    }

    // restore state values from undo info
    castling   = ui.castlingRights;
    en_passant = ui.enPassantSquare;
    halfmove   = ui.halfmoveClock;
    fullmove   = ui.fullmoveNumber;
    whiteKingPosition = ui.whiteKingPos;
    blackKingPosition = ui.blackKingPos;

    // rebuild occupancy
    updateOccupancy();

    // Zobrist: restore previous key by popping repetition history (safer than incremental XOR mistakes)
    if (updateHash) {
        if (!repetitionHistory.empty()) repetitionHistory.pop_back();
        if (!repetitionHistory.empty()) {
            zobristKey = repetitionHistory.back();
        } else {
            // fallback: recompute and push
            zobristKey = computeZobrist();
            repetitionHistory.push_back(zobristKey);
        }
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
