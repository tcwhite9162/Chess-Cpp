#include <cctype>
#include <iostream>
#include <sstream>

#include "core/board.hpp"
#include "core/utils.hpp"
#include "movegen/moveGen.hpp"

Board::Board() {
    squares.fill(Data::Piece::EMPTY);
    turn = Data::Piece::WHITE;
    castling   = 0;
    en_passant = -1;
    halfmove   = 0;
    fullmove   = 1;
    whiteKingPosition   = 0;
    blackKingPosition   = 0;
    lastIrreversiblePly = 0;

    bitboards[PieceType::WP] = 0ULL;
    bitboards[PieceType::WN] = 0ULL;
    bitboards[PieceType::WB] = 0ULL;
    bitboards[PieceType::WR] = 0ULL;
    bitboards[PieceType::WQ] = 0ULL;
    bitboards[PieceType::WK] = 0ULL;

    bitboards[PieceType::BP] = 0ULL;
    bitboards[PieceType::BN] = 0ULL;
    bitboards[PieceType::BB] = 0ULL;
    bitboards[PieceType::BR] = 0ULL;
    bitboards[PieceType::BQ] = 0ULL;
    bitboards[PieceType::BK] = 0ULL;

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
    return Data::Piece::EMPTY;
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
    setupFromFen(Data::FEN::startFen);
}

void Board::setupFromFen(const std::string& fen) {
    // clear everything
    squares.fill(Data::Piece::EMPTY);

    // reset bitboards
    // whitePawns   = blackPawns   = 0ULL;
    // whiteKnights = blackKnights = 0ULL;
    // whiteBishops = blackBishops = 0ULL;
    // whiteRooks   = blackRooks   = 0ULL;
    // whiteQueens  = blackQueens  = 0ULL;
    // whiteKing    = blackKing    = 0ULL;

    bitboards[PieceType::WP]   = bitboards[PieceType::BP]   = 0ULL;
    bitboards[PieceType::WN] = bitboards[PieceType::BN]   = 0ULL;
    bitboards[PieceType::WB] = bitboards[PieceType::BB]   = 0ULL;
    bitboards[PieceType::WR]   = bitboards[PieceType::BR]   = 0ULL;
    bitboards[PieceType::WQ]  = bitboards[PieceType::BQ]   = 0ULL;
    bitboards[PieceType::WK]   = bitboards[PieceType::BK]   = 0ULL;

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
                case 'p': setPieceAndBitboard(squareIndex, Data::Piece::B_PAWN);   break;
                case 'n': setPieceAndBitboard(squareIndex, Data::Piece::B_KNIGHT); break;
                case 'b': setPieceAndBitboard(squareIndex, Data::Piece::B_BISHOP); break;
                case 'r': setPieceAndBitboard(squareIndex, Data::Piece::B_ROOK);   break;
                case 'q': setPieceAndBitboard(squareIndex, Data::Piece::B_QUEEN);  break;
                case 'k': setPieceAndBitboard(squareIndex, Data::Piece::B_KING); blackKingPosition = squareIndex;   break;

                case 'P': setPieceAndBitboard(squareIndex, Data::Piece::W_PAWN);   break;
                case 'N': setPieceAndBitboard(squareIndex, Data::Piece::W_KNIGHT); break;
                case 'B': setPieceAndBitboard(squareIndex, Data::Piece::W_BISHOP); break;
                case 'R': setPieceAndBitboard(squareIndex, Data::Piece::W_ROOK);   break;
                case 'Q': setPieceAndBitboard(squareIndex, Data::Piece::W_QUEEN);  break;
                case 'K': setPieceAndBitboard(squareIndex, Data::Piece::W_KING); whiteKingPosition = squareIndex;   break;

                default: return;
            }
            squareIndex++;
        }
    }

    turn = (turnPart == "w") ? Data::Piece::WHITE : Data::Piece::BLACK;

    castling = 0;
    if (castlingPart.find('K') != std::string::npos) castling |= Data::Castling::CASTLE_WK;
    if (castlingPart.find('Q') != std::string::npos) castling |= Data::Castling::CASTLE_WQ;
    if (castlingPart.find('k') != std::string::npos) castling |= Data::Castling::CASTLE_BK;
    if (castlingPart.find('q') != std::string::npos) castling |= Data::Castling::CASTLE_BQ;

    if (epPart == "-") {
        en_passant = -1;
    }
    else {
        int file   = (epPart[0] - 'a');
        int rank   = (epPart[1] - '0');
        en_passant = rank * 8 + file;
    }

    halfmove = (halfmovePart.empty()) ? 0 : std::stoi(halfmovePart);
    fullmove = (fullmovePart.empty()) ? 1 : std::stoi(fullmovePart);

    updateOccupancy();

    zobristKey = computeZobrist();
    repetitionHistory.clear();
    repetitionHistory.push_back(zobristKey);
}

void Board::setPieceAndBitboard(int square, int piece) {
    squares[square] = piece;
    u64 mask = 1ULL << square;

    // clear square from all bitboards
    bitboards[PieceType::WP]   &= ~mask; bitboards[PieceType::BP]    &= ~mask;
    bitboards[PieceType::WN] &= ~mask; bitboards[PieceType::BN]  &= ~mask;
    bitboards[PieceType::WB] &= ~mask; bitboards[PieceType::BB]  &= ~mask;
    bitboards[PieceType::WR]   &= ~mask; bitboards[PieceType::BR]    &= ~mask;
    bitboards[PieceType::WQ]  &= ~mask; bitboards[PieceType::BQ]   &= ~mask;
    bitboards[PieceType::WK]   &= ~mask; bitboards[PieceType::BK]    &= ~mask;

    // add to corresponding bitboard
    switch (piece) {
        case Data::Piece::EMPTY: return;

        case Data::Piece::W_PAWN:   bitboards[PieceType::WP]   |= mask; break;
        case Data::Piece::W_KNIGHT: bitboards[PieceType::WN] |= mask; break;
        case Data::Piece::W_BISHOP: bitboards[PieceType::WB] |= mask; break;
        case Data::Piece::W_ROOK:   bitboards[PieceType::WR]   |= mask; break;
        case Data::Piece::W_QUEEN:  bitboards[PieceType::WQ]  |= mask; break;
        case Data::Piece::W_KING:   bitboards[PieceType::WK]   |= mask; break;

        case Data::Piece::B_PAWN:   bitboards[PieceType::BP]   |= mask; break;
        case Data::Piece::B_KNIGHT: bitboards[PieceType::BN] |= mask; break;
        case Data::Piece::B_BISHOP: bitboards[PieceType::BB] |= mask; break;
        case Data::Piece::B_ROOK:   bitboards[PieceType::BR]   |= mask; break;
        case Data::Piece::B_QUEEN:  bitboards[PieceType::BQ]  |= mask; break;
        case Data::Piece::B_KING:   bitboards[PieceType::BK]   |= mask; break;

        default: return;
    }
}

void Board::removePieceBB(int square, int piece) {
    u64 mask = ~(1ULL << square);

    switch (piece) {
        case Data::Piece::W_PAWN:   bitboards[PieceType::WP]   &= mask; break;
        case Data::Piece::W_KNIGHT: bitboards[PieceType::WN] &= mask; break;
        case Data::Piece::W_BISHOP: bitboards[PieceType::WB] &= mask; break;
        case Data::Piece::W_ROOK:   bitboards[PieceType::WR]   &= mask; break;
        case Data::Piece::W_QUEEN:  bitboards[PieceType::WQ]  &= mask; break;
        case Data::Piece::W_KING:   bitboards[PieceType::WK]   &= mask; break;

        case Data::Piece::B_PAWN:   bitboards[PieceType::BP]   &= mask; break;
        case Data::Piece::B_KNIGHT: bitboards[PieceType::BN] &= mask; break;
        case Data::Piece::B_BISHOP: bitboards[PieceType::BB] &= mask; break;
        case Data::Piece::B_ROOK:   bitboards[PieceType::BR]   &= mask; break;
        case Data::Piece::B_QUEEN:  bitboards[PieceType::BQ]  &= mask; break;
        case Data::Piece::B_KING:   bitboards[PieceType::BK]   &= mask; break;

        default: return;
    }
}

void Board::addPieceBB(int square, int piece) {
    u64 mask = 1ULL << square;

    switch (piece) {
        case Data::Piece::W_PAWN:   bitboards[PieceType::WP]   |= mask; break;
        case Data::Piece::W_KNIGHT: bitboards[PieceType::WN] |= mask; break;
        case Data::Piece::W_BISHOP: bitboards[PieceType::WB] |= mask; break;
        case Data::Piece::W_ROOK:   bitboards[PieceType::WR]   |= mask; break;
        case Data::Piece::W_QUEEN:  bitboards[PieceType::WQ]  |= mask; break;
        case Data::Piece::W_KING:   bitboards[PieceType::WK]   |= mask; break;

        case Data::Piece::B_PAWN:   bitboards[PieceType::BP]   |= mask; break;
        case Data::Piece::B_KNIGHT: bitboards[PieceType::BN] |= mask; break;
        case Data::Piece::B_BISHOP: bitboards[PieceType::BB] |= mask; break;
        case Data::Piece::B_ROOK:   bitboards[PieceType::BR]   |= mask; break;
        case Data::Piece::B_QUEEN:  bitboards[PieceType::BQ]  |= mask; break;
        case Data::Piece::B_KING:   bitboards[PieceType::BK]   |= mask; break;

        default: return;
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
    occupancyWhite = bitboards[PieceType::WP]   | bitboards[PieceType::WN] |
                     bitboards[PieceType::WB] | bitboards[PieceType::WR]   |
                     bitboards[PieceType::WQ]  | bitboards[PieceType::WK];

    occupancyBlack = bitboards[PieceType::BP]   | bitboards[PieceType::BN] |
                     bitboards[PieceType::BB] | bitboards[PieceType::BR]   |
                     bitboards[PieceType::BQ]  | bitboards[PieceType::BK];

    occupancyAll = occupancyWhite | occupancyBlack;
}

bool Board::isInCheck(int color) const {
    int kingSq = (color == Data::Piece::WHITE) ? whiteKingPosition : blackKingPosition;
    int enemy = (color == Data::Piece::WHITE ? Data::Piece::BLACK : Data::Piece::WHITE);
    return isSquareAttacked(kingSq, enemy);
}

bool Board::isSquareAttacked(int square, int attackingColor) const {
    u64 occ = occupancyAll;

    int r = square / 8;
    int f = square % 8;


    // pawns
    if (attackingColor == Data::Piece::WHITE) {
        u64 attackers = 0ULL;

        // White pawns attack from squares one rank *below* the target:
        // from = square + 7 (down-left) or square + 9 (down-right)
        if (r < 7 && f > 0) attackers |= 1ULL << (square + 7);  // from file-1, rank+1 (d2 -> e3)
        if (r < 7 && f < 7) attackers |= 1ULL << (square + 9);  // from file+1, rank+1 (f2 -> e3)

        if (bitboards[PieceType::WP] & attackers) return true;

    } else { // attackingColor == BLACK
        u64 attackers = 0ULL;

        // Black pawns attack from squares one rank *above* the target:
        // from = square - 7 (up-right) or square - 9 (up-left)
        if (r > 0 && f < 7) attackers |= 1ULL << (square - 7);  // from file+1, rank-1 (e4 -> d5)
        if (r > 0 && f > 0) attackers |= 1ULL << (square - 9);  // from file-1, rank-1 (c4 -> d5)

        if (bitboards[PieceType::BP] & attackers) return true;
    }

    // knights
    if (attackingColor == Data::Piece::WHITE) {
        if (bitboards[PieceType::WN] & knightAttacks[square]) return true;
    } else {
        if (bitboards[PieceType::BN] & knightAttacks[square]) return true;
    }

    // bishops + queens (diagonal)
    if (attackingColor == Data::Piece::WHITE) {
        if (bishopAttacksMagic(square, occ) & (bitboards[PieceType::WB] | bitboards[PieceType::WQ])) return true;
    } else {
        if (bishopAttacksMagic(square, occ) & (bitboards[PieceType::BB] | bitboards[PieceType::BQ])) return true;
    }

    // rooks + queens (orthogonal)
    if (attackingColor == Data::Piece::WHITE) {
        if (rookAttacksMagic(square, occ) & (bitboards[PieceType::WR] | bitboards[PieceType::WQ])) return true;
    } else {
        if (rookAttacksMagic(square, occ) & (bitboards[PieceType::BR] | bitboards[PieceType::BQ])) return true;
    }

    // kings
    if (attackingColor == Data::Piece::WHITE) {
        if (bitboards[PieceType::WK] & kingAttacks[square]) return true;
    } else {
        if (bitboards[PieceType::BK] & kingAttacks[square]) return true;
    }

    return false;
}

bool Board::enPassantAvailable() const{
    if (en_passant == -1) return false;

    const int enPassantRank = en_passant / 8;
    const int enPassantFile = en_passant % 8;

    int fileOffsets[2] = {1, -1};

    const int pawn = (turn == Data::Piece::WHITE) ? Data::Piece::W_PAWN : Data::Piece::B_PAWN;
    const int pawnRank = (turn == Data::Piece::WHITE) ? enPassantRank + 1 : enPassantRank - 1;

    if (pawnRank < 0 || pawnRank > 7)
        return false;

    for (int dFile : fileOffsets) {
        int adjacentFile = enPassantFile + dFile;
        if (adjacentFile < 0 || adjacentFile > 7) continue;

        int square = pawnRank * 8 + adjacentFile;

        if (squares[square] == pawn) return true;
    }

    return false;
}

int Board::getEnPassantCaptureSquare() const {
    if (en_passant == -1)
        return -1;

    int capSq = (turn == Data::Piece::WHITE) ? en_passant + Data::Board::DOWN : en_passant + Data::Board::UP;

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

    for (int i = lastIrreversiblePly; i < repetitionHistory.size(); i++) {
        if (repetitionHistory[i] == current) {
            count++;
            if (count >= 3) return true;
        }
    }
    return false;
}

bool Board::isInsuffucientMaterial() {
    int wBishops = 0; int bBishops = 0;
    int wKnights = 0; int bKnights = 0;
    bool whiteBishopLight = false; bool whiteBishopDark = false;
    bool blackBishopLight = false; bool blackBishopDark = false;

    for (int sq = 0; sq < 64; sq++) {
        int piece = squares[sq];
        if (piece == Data::Piece::EMPTY) continue;

        switch (piece) {
            // pawns/rooks/queens always have possible mate
            case Data::Piece::W_PAWN:  case Data::Piece::B_PAWN:
            case Data::Piece::W_ROOK:  case Data::Piece::B_ROOK:
            case Data::Piece::W_QUEEN: case Data::Piece::B_QUEEN:
                return false;

            case Data::Piece::W_BISHOP:
                wBishops++;
                // if row + col % 2 == 0, bishop is light
                if ((sq / 8 + sq % 8) % 2 == 0) whiteBishopLight = true;
                else whiteBishopDark = true;
                break;

            case Data::Piece::B_BISHOP:
                bBishops++;
                if ((sq / 8 + sq % 8) % 2 == 0) blackBishopLight = true;
                else blackBishopDark = true;
                break;

            case Data::Piece::W_KNIGHT: wKnights++; break;
            case Data::Piece::B_KNIGHT: bKnights++; break;

            default: break; // kings dont matter
        }
    }

    int totalBishops = wBishops + bBishops;
    int totalKnights = wKnights + bKnights;

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
    if (movedPiece == Data::Piece::W_KING)      { castling &= ~(Data::Castling::CASTLE_WK | Data::Castling::CASTLE_WQ); }
    else if (movedPiece == Data::Piece::B_KING) { castling &= ~(Data::Castling::CASTLE_BK | Data::Castling::CASTLE_BQ); }

    // if rook moves
    if (fromSquare == Data::Castling::W_ROOK_H1) { castling &= ~Data::Castling::CASTLE_WK; }
    if (fromSquare == Data::Castling::W_ROOK_A1) { castling &= ~Data::Castling::CASTLE_WQ; }
    if (fromSquare == Data::Castling::B_ROOK_H8) { castling &= ~Data::Castling::CASTLE_BK; }
    if (fromSquare == Data::Castling::B_ROOK_A8) { castling &= ~Data::Castling::CASTLE_BQ; }

    // rook captured on starting square
    if (capturedPiece == Data::Piece::W_ROOK) {
        if (toSquare == Data::Castling::W_ROOK_H1) castling &= ~Data::Castling::CASTLE_WK;
        if (toSquare == Data::Castling::W_ROOK_A1) castling &= ~Data::Castling::CASTLE_WQ;
    }
    else if (capturedPiece == Data::Piece::B_ROOK) {
        if (toSquare == Data::Castling::B_ROOK_H8) castling &= ~Data::Castling::CASTLE_BK;
        if (toSquare == Data::Castling::B_ROOK_A8) castling &= ~Data::Castling::CASTLE_BQ;
    }
}

void Board::makeMove(Move move, bool updateHash) {
    int fromSquare = move.from_square();
    int toSquare   = move.to_square();
    int flags      = move.flags();

    int piece = squares[fromSquare];

    // compute en-passant capture square if this move is en-passant
    int capSq = -1;
    if (flags & Flags::FLAG_EN_PASSANT) {
        capSq = (turn == Data::Piece::WHITE) ? toSquare + Data::Board::DOWN : toSquare + Data::Board::UP;
    }

    // captured piece: for en-passant the captured pawn is on capSq, otherwise on toSquare
    int captured = (flags & Flags::FLAG_EN_PASSANT) ? (isValidSquare(capSq) ? squares[capSq] : Data::Piece::EMPTY) : squares[toSquare];

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
    squares[fromSquare] = Data::Piece::EMPTY;

    // Clear en_passant by default; will set if double push
    en_passant = -1;

    // Remove captured piece from mailbox/bitboards BEFORE placing/moving the moving piece
    if (flags & Flags::FLAG_EN_PASSANT) {
        if (isValidSquare(capSq) && captured != Data::Piece::EMPTY) {
            // captured pawn is on capSq (behind the toSquare)
            int capPiece = (turn == Data::Piece::WHITE) ? Data::Piece::B_PAWN : Data::Piece::W_PAWN;
            squares[capSq] = Data::Piece::EMPTY;
            removePieceBB(capSq, capPiece);
        }
    } else if (captured != Data::Piece::EMPTY) {
        // normal capture on toSquare
        squares[toSquare] = Data::Piece::EMPTY; // will be overwritten below
        removePieceBB(toSquare, captured);
    }

    // Handle promotion
    if (isPromotion(flags)) {
        int promo = promotionPiece(flags, turn); // e.g. W_QUEEN or B_QUEEN
        // remove pawn from fromSquare bitboard
        removePieceBB(fromSquare, (turn == Data::Piece::WHITE ? Data::Piece::W_PAWN : Data::Piece::B_PAWN));
        // place promoted piece on toSquare (mailbox + bitboard)
        squares[toSquare] = promo;
        addPieceBB(toSquare, promo);
    } else {
        // Normal move: move bitboard entry and mailbox
        movePieceBB(fromSquare, toSquare, piece);
        squares[toSquare] = piece;
    }

    // Handle double pawn push -> set en_passant square
    if (flags & Flags::FLAG_DOUBLE_PUSH) {
        en_passant = (turn == Data::Piece::WHITE) ? toSquare + Data::Board::DOWN : toSquare + Data::Board::UP;
    }

    // Handle castling rook moves (mailbox + bitboards)
    if (flags & Flags::FLAG_CASTLE_WK) {
        // White king-side: rook from h1 -> f1
        squares[Data::Castling::W_ROOK_H1] = Data::Piece::EMPTY;
        squares[Data::Castling::W_ROOK_F1_END] = Data::Piece::W_ROOK;
        movePieceBB(Data::Castling::W_ROOK_H1, Data::Castling::W_ROOK_F1_END, Data::Piece::W_ROOK);
        whiteKingPosition = toSquare;
    }
    if (flags & Flags::FLAG_CASTLE_WQ) {
        // White queen-side: rook from a1 -> d1
        squares[Data::Castling::W_ROOK_A1] = Data::Piece::EMPTY;
        squares[Data::Castling::W_ROOK_D1_END] = Data::Piece::W_ROOK;
        movePieceBB(Data::Castling::W_ROOK_A1, Data::Castling::W_ROOK_D1_END, Data::Piece::W_ROOK);
        whiteKingPosition = toSquare;
    }
    if (flags & Flags::FLAG_CASTLE_BK) {
        // Black king-side: rook from h8 -> f8
        squares[Data::Castling::B_ROOK_H8] = Data::Piece::EMPTY;
        squares[Data::Castling::B_ROOK_F8_END] = Data::Piece::B_ROOK;
        movePieceBB(Data::Castling::B_ROOK_H8, Data::Castling::B_ROOK_F8_END, Data::Piece::B_ROOK);
        blackKingPosition = toSquare;
    }
    if (flags & Flags::FLAG_CASTLE_BQ) {
        // Black queen-side: rook from a8 -> d8
        squares[Data::Castling::B_ROOK_A8] = Data::Piece::EMPTY;
        squares[Data::Castling::B_ROOK_D8_END] = Data::Piece::B_ROOK;
        movePieceBB(Data::Castling::B_ROOK_A8, Data::Castling::B_ROOK_D8_END, Data::Piece::B_ROOK);
        blackKingPosition = toSquare;
    }

    // Update king position for non-castling king moves
    if (piece == Data::Piece::W_KING && !(flags & (Flags::FLAG_CASTLE_WK | Flags::FLAG_CASTLE_WQ))) {
        whiteKingPosition = toSquare;
    }
    if (piece == Data::Piece::B_KING && !(flags & (Flags::FLAG_CASTLE_BK | Flags::FLAG_CASTLE_BQ))) {
        blackKingPosition = toSquare;
    }

    // Rebuild occupancy
    updateOccupancy();

    // halfmove/fullmove bookkeeping
    halfmove = (piece == Data::Piece::W_PAWN || piece == Data::Piece::B_PAWN || captured != Data::Piece::EMPTY) ? 0 : halfmove + 1;
    if (turn == Data::Piece::BLACK) fullmove++;

    // update castling rights (uses moved piece and captured piece)
    updateCastlingRights(fromSquare, toSquare, piece, captured);

    // flip side to move
    flipTurn();

    // update last irreversible ply (for repetition)
    if (piece == Data::Piece::W_PAWN || piece == Data::Piece::B_PAWN || captured != Data::Piece::EMPTY) {
        lastIrreversiblePly = (int)repetitionHistory.size();
    }

    // update zobrist and repetition history
    if (updateHash) {
        // XOR out piece from fromSquare (use ui.movedPiece)
        zobristKey ^= Zobrist::piece[pieceToIndex(ui.movedPiece)][fromSquare];

        // XOR out captured piece (use ui.capturedPiece and ui.capturedSquare for en-passant)
        if (ui.capturedPiece != Data::Piece::EMPTY) {
            if (ui.capturedSquare != -1 && (flags & Flags::FLAG_EN_PASSANT)) {
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
    if (flags & Flags::FLAG_CASTLE_WK) {
        movePieceBB(Data::Castling::W_ROOK_F1_END, Data::Castling::W_ROOK_H1, Data::Piece::W_ROOK);
        squares[Data::Castling::W_ROOK_H1] = Data::Piece::W_ROOK;
        squares[Data::Castling::W_ROOK_F1_END] = Data::Piece::EMPTY;
    }
    if (flags & Flags::FLAG_CASTLE_WQ) {
        movePieceBB(Data::Castling::W_ROOK_D1_END, Data::Castling::W_ROOK_A1, Data::Piece::W_ROOK);
        squares[Data::Castling::W_ROOK_A1] = Data::Piece::W_ROOK;
        squares[Data::Castling::W_ROOK_D1_END] = Data::Piece::EMPTY;
    }
    if (flags & Flags::FLAG_CASTLE_BK) {
        movePieceBB(Data::Castling::B_ROOK_F8_END, Data::Castling::B_ROOK_H8, Data::Piece::B_ROOK);
        squares[Data::Castling::B_ROOK_H8] = Data::Piece::B_ROOK;
        squares[Data::Castling::B_ROOK_F8_END] = Data::Piece::EMPTY;
    }
    if (flags & Flags::FLAG_CASTLE_BQ) {
        movePieceBB(Data::Castling::B_ROOK_D8_END, Data::Castling::B_ROOK_A8, Data::Piece::B_ROOK);
        squares[Data::Castling::B_ROOK_A8] = Data::Piece::B_ROOK;
        squares[Data::Castling::B_ROOK_D8_END] = Data::Piece::EMPTY;
    }

    // Undo promotion or normal move
    if (isPromotion(flags)) {
        // promoted piece is on toSquare; remove it
        int promo = promotionPiece(flags, (ui.movedPiece > 0) ? Data::Piece::WHITE : Data::Piece::BLACK);
        removePieceBB(toSquare, promo);
        squares[toSquare] = Data::Piece::EMPTY;

        // restore pawn on fromSquare (movedPiece indicates color)
        int pawn = (ui.movedPiece > 0) ? Data::Piece::W_PAWN : Data::Piece::B_PAWN;
        addPieceBB(fromSquare, pawn);
        squares[fromSquare] = pawn;
    } else {
        // move piece back from toSquare to fromSquare
        movePieceBB(toSquare, fromSquare, movedPiece);
        squares[fromSquare] = movedPiece;
        squares[toSquare] = Data::Piece::EMPTY;
    }

    // Restore captured piece
    if (captured != Data::Piece::EMPTY) {
        if (flags & Flags::FLAG_EN_PASSANT) {
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
        if (piece != Data::Piece::EMPTY) {
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
    if (turn == Data::Piece::BLACK) {
        key ^= Zobrist::side;
    }

    return key;
}

int Board::getKingSquare(int side) const {
    u64 bb = (side == Data::Piece::WHITE) ? bitboards[PieceType::WK] : bitboards[PieceType::BK];
    return __builtin_ctzll(bb);
}

u64 Board::getCheckers() const {
    int side = getTurn();
    int enemy = side ^ 1;
    int kingSq = getKingSquare(side);

    u64 checkers = 0ULL;

    // pawn checks
    checkers |= pawnAttacks[enemy][kingSq] &
                (enemy == Data::Piece::WHITE ? bitboards[PieceType::WP] : bitboards[PieceType::BP]);

    // knight checks
    checkers |= knightAttacks[kingSq] &
                (enemy == Data::Piece::WHITE ? bitboards[PieceType::WN] : bitboards[PieceType::BN]);

    // bishop/queen diagonal checks
    u64 bishopLike = bishopAttacksMagic(kingSq, occupancyAll) &
                     (enemy == Data::Piece::WHITE ?
                            (bitboards[PieceType::WB] | bitboards[PieceType::WQ]) :
                            (bitboards[PieceType::BB] | bitboards[PieceType::BQ])
                        );

    checkers |= bishopLike;

    // rook/queen orthogonal checks
    u64 rookLike = rookAttacksMagic(kingSq, occupancyAll) &
                   (enemy == Data::Piece::WHITE ?
                        (bitboards[PieceType::WR] | bitboards[PieceType::WQ]) :
                        (bitboards[PieceType::BR] | bitboards[PieceType::BQ])
                    );

    checkers |= rookLike;

    return checkers;
}