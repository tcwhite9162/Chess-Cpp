#include <sstream>
#include <iostream>

#include "constants.hpp"
#include "core/board.hpp"
#include "core/utils.hpp"
#include "movegen/moveGen.hpp"
#include "bitboard/bitboard.hpp"

Board::Board() {
    squares.fill(Constants::Piece::EMPTY);

    turn       = Constants::Piece::WHITE;
    castling   = 0;
    en_passant = -1;
    halfmove   = 0;
    fullmove   = 1;

    whiteKingPosition   = 0;
    blackKingPosition   = 0;
    lastIrreversiblePly = 0;

    bitboards.fill(0ULL);

    occupancyWhite = 0ULL;
    occupancyBlack = 0ULL;
    occupancyAll   = 0ULL;

    zobristKey = computeZobrist();
    repetitionHistory.clear();
    repetitionHistory.push_back(zobristKey);
}

int Board::getPiece(int square) const {
    if (square < 0 || square >= 64)
        return Constants::Piece::EMPTY;

    const u64 mask = 1ULL << square;

    if (!(mask & occupancyAll))
        return Constants::Piece::EMPTY;

    if (mask & occupancyWhite) {
        if (bitboards[PieceType::WP] & mask)
            return Constants::Piece::W_PAWN;
        if (bitboards[PieceType::WN] & mask)
            return Constants::Piece::W_KNIGHT;
        if (bitboards[PieceType::WB] & mask)
            return Constants::Piece::W_BISHOP;
        if (bitboards[PieceType::WR] & mask)
            return Constants::Piece::W_ROOK;
        if (bitboards[PieceType::WQ] & mask)
            return Constants::Piece::W_QUEEN;
        return Constants::Piece::W_KING;
    }
    else {
        if (bitboards[PieceType::BP] & mask)
            return Constants::Piece::B_PAWN;
        if (bitboards[PieceType::BN] & mask)
            return Constants::Piece::B_KNIGHT;
        if (bitboards[PieceType::BB] & mask)
            return Constants::Piece::B_BISHOP;
        if (bitboards[PieceType::BR] & mask)
            return Constants::Piece::B_ROOK;
        if (bitboards[PieceType::BQ] & mask)
            return Constants::Piece::B_QUEEN;
        return Constants::Piece::B_KING;
    }
}

void Board::setPiece(int square, int piece) {
    if (piece < 0 || piece >= 64)
        return;

    if (piece != Constants::Piece::EMPTY) {
        const PieceType p = pieceToPieceType(piece);
        bitboards[p] |= (1ULL << square);
    }
}

void Board::setPieceAndBitboard(int square, int piece) {
    const u64 mask = 1ULL << square;

    const int oldPiece = squares[square];
    squares[square]    = piece;

    // clear square from corresponding bitboard
    if (oldPiece != Constants::Piece::EMPTY)
        bitboards[pieceToPieceType(oldPiece)] &= ~mask;

    // add to corresponding bitboard
    if (piece != Constants::Piece::EMPTY)
        bitboards[pieceToPieceType(piece)] |= mask;
}

void Board::removePieceBB(int square, int piece) {
    const u64 mask     = 1ULL << square;
    const PieceType pt = pieceToPieceType(piece);

    bitboards[pt] &= ~mask;

    if (piece > 0) // white
        occupancyWhite &= ~mask;
    else
        occupancyBlack &= ~mask;

    occupancyAll &= ~mask;
}

void Board::addPieceBB(int square, int piece) {
    const u64 mask     = 1ULL << square;
    const PieceType pt = pieceToPieceType(piece);

    bitboards[pt] |= mask;

    if (piece > 0) // white
        occupancyWhite |= mask;
    else
        occupancyBlack |= mask;

    occupancyAll |= mask;
}

void Board::movePieceBB(int from, int to, int piece) {
    const u64 fromMask = 1ULL << from;
    const u64 toMask   = 1ULL << to;
    const PieceType pt = pieceToPieceType(piece);

    bitboards[pt] ^= fromMask | toMask;

    if (piece > 0) { // white
        occupancyWhite ^= fromMask;
        occupancyWhite |= toMask;
    }
    else {
        occupancyBlack ^= fromMask;
        occupancyBlack |= toMask;
    }

    occupancyAll = occupancyWhite | occupancyBlack;
}

void Board::removeCapturedPieceBB(int square, int piece) {
    removePieceBB(square, piece);
}

void Board::updateCastlingRights(int fromSquare, int toSquare, int movedPiece, int capturedPiece) {
    // clear castling if king moves
    if (movedPiece == Constants::Piece::W_KING)
        castling &= ~(Constants::Castling::CASTLE_WK | Constants::Castling::CASTLE_WQ);

    else if (movedPiece == Constants::Piece::B_KING)
        castling &= ~(Constants::Castling::CASTLE_BK | Constants::Castling::CASTLE_BQ);

    // if rook moves from start square
    if (fromSquare == Constants::Castling::W_ROOK_H1)
        castling &= ~Constants::Castling::CASTLE_WK;
    if (fromSquare == Constants::Castling::W_ROOK_A1)
        castling &= ~Constants::Castling::CASTLE_WQ;
    if (fromSquare == Constants::Castling::B_ROOK_H8)
        castling &= ~Constants::Castling::CASTLE_BK;
    if (fromSquare == Constants::Castling::B_ROOK_A8)
        castling &= ~Constants::Castling::CASTLE_BQ;

    // if rook captured on start square
    if (capturedPiece == Constants::Piece::W_ROOK) {
        if (toSquare == Constants::Castling::W_ROOK_H1)
            castling &= ~Constants::Castling::CASTLE_WK;
        if (toSquare == Constants::Castling::W_ROOK_A1)
            castling &= ~Constants::Castling::CASTLE_WQ;
    }
    else if (capturedPiece == Constants::Piece::B_ROOK) {
        if (toSquare == Constants::Castling::B_ROOK_H8)
            castling &= ~Constants::Castling::CASTLE_BK;
        if (toSquare == Constants::Castling::B_ROOK_A8)
            castling &= ~Constants::Castling::CASTLE_BQ;
    }
}

void Board::makeMove(Move move, bool updateHash) {
    const int fromSquare = move.from_square();
    const int toSquare   = move.to_square();
    const int flags      = move.flags();

    const int piece = squares[fromSquare];

    int capSq = -1;
    if (flags & Flags::FLAG_EN_PASSANT)
        capSq = (turn == Constants::Piece::WHITE) ? toSquare + Constants::Squares::DOWN : toSquare + Constants::Squares::UP;

    // captured piece: for en-passant the captured pawn is on capSq, otherwise on toSquare
    const int captured = (flags & Flags::FLAG_EN_PASSANT) ? (isValidSquare(capSq) ? squares[capSq] : Constants::Piece::EMPTY)
                                                          : squares[toSquare];

    // Save undo info (store previous castling/enpassant/halfmove/fullmove and king positions)
    undoInfo ui{move,
                piece,
                captured,
                capSq,
                castling,
                en_passant,
                halfmove,
                fullmove,
                whiteKingPosition,
                blackKingPosition,
                occupancyWhite,
                occupancyBlack,
                occupancyAll,
                zobristKey};

    history.push_back(ui);

    // Clear mailbox from-square immediately (set destination below)
    squares[fromSquare] = Constants::Piece::EMPTY;

    // Clear en_passant by default; will set if double push
    en_passant = -1;

    // Remove captured piece from mailbox/bitboards BEFORE placing/moving the moving piece
    if (flags & Flags::FLAG_EN_PASSANT) {
        if (isValidSquare(capSq) && captured != Constants::Piece::EMPTY) {
            // captured pawn is on capSq (behind the toSquare)
            int capPiece   = (turn == Constants::Piece::WHITE) ? Constants::Piece::B_PAWN : Constants::Piece::W_PAWN;
            squares[capSq] = Constants::Piece::EMPTY;
            removePieceBB(capSq, capPiece);
        }
    }
    else if (captured != Constants::Piece::EMPTY) {
        // normal capture on toSquare
        squares[toSquare] = Constants::Piece::EMPTY; // will be overwritten below if needed
        removePieceBB(toSquare, captured);
    }

    // Handle promotion
    if (isPromotion(flags)) {
        int promo = promotionPiece(flags, turn);
        // remove pawn from fromSquare bitboard
        removePieceBB(fromSquare, (turn == Constants::Piece::WHITE ? Constants::Piece::W_PAWN : Constants::Piece::B_PAWN));
        // place promoted piece on toSquare (mailbox + bitboard)
        squares[toSquare] = promo;
        addPieceBB(toSquare, promo);
    }
    else {
        // Normal move
        movePieceBB(fromSquare, toSquare, piece);
        squares[toSquare] = piece;
    }

    // double pawn push -> set en_passant square
    if (flags & Flags::FLAG_DOUBLE_PUSH)
        en_passant =
            (turn == Constants::Piece::WHITE) ? toSquare + Constants::Squares::DOWN : toSquare + Constants::Squares::UP;

    // castling rook moves (mailbox + bitboards)
    if (flags & Flags::FLAG_CASTLE_WK) {
        // White king-side: rook from h1 -> f1
        squares[Constants::Castling::W_ROOK_H1]     = Constants::Piece::EMPTY;
        squares[Constants::Castling::W_ROOK_F1_END] = Constants::Piece::W_ROOK;
        movePieceBB(Constants::Castling::W_ROOK_H1, Constants::Castling::W_ROOK_F1_END, Constants::Piece::W_ROOK);
        whiteKingPosition = toSquare;
    }
    if (flags & Flags::FLAG_CASTLE_WQ) {
        // White queen-side: rook from a1 -> d1
        squares[Constants::Castling::W_ROOK_A1]     = Constants::Piece::EMPTY;
        squares[Constants::Castling::W_ROOK_D1_END] = Constants::Piece::W_ROOK;
        movePieceBB(Constants::Castling::W_ROOK_A1, Constants::Castling::W_ROOK_D1_END, Constants::Piece::W_ROOK);
        whiteKingPosition = toSquare;
    }
    if (flags & Flags::FLAG_CASTLE_BK) {
        // Black king-side: rook from h8 -> f8
        squares[Constants::Castling::B_ROOK_H8]     = Constants::Piece::EMPTY;
        squares[Constants::Castling::B_ROOK_F8_END] = Constants::Piece::B_ROOK;
        movePieceBB(Constants::Castling::B_ROOK_H8, Constants::Castling::B_ROOK_F8_END, Constants::Piece::B_ROOK);
        blackKingPosition = toSquare;
    }
    if (flags & Flags::FLAG_CASTLE_BQ) {
        // Black queen-side: rook from a8 -> d8
        squares[Constants::Castling::B_ROOK_A8]     = Constants::Piece::EMPTY;
        squares[Constants::Castling::B_ROOK_D8_END] = Constants::Piece::B_ROOK;
        movePieceBB(Constants::Castling::B_ROOK_A8, Constants::Castling::B_ROOK_D8_END, Constants::Piece::B_ROOK);
        blackKingPosition = toSquare;
    }

    // king position for non-castling king moves
    if (piece == Constants::Piece::W_KING && !(flags & (Flags::FLAG_CASTLE_WK | Flags::FLAG_CASTLE_WQ)))
        whiteKingPosition = toSquare;

    if (piece == Constants::Piece::B_KING && !(flags & (Flags::FLAG_CASTLE_BK | Flags::FLAG_CASTLE_BQ)))
        blackKingPosition = toSquare;

    // halfmove/fullmove update
    halfmove =
        (piece == Constants::Piece::W_PAWN || piece == Constants::Piece::B_PAWN || captured != Constants::Piece::EMPTY)
            ? 0
            : halfmove + 1;
    if (turn == Constants::Piece::BLACK)
        fullmove++;

    updateCastlingRights(fromSquare, toSquare, piece, captured);

    flipTurn();

    // update last irreversible ply (for repetition)
    if (piece == Constants::Piece::W_PAWN || piece == Constants::Piece::B_PAWN || captured != Constants::Piece::EMPTY)
        lastIrreversiblePly = (int)repetitionHistory.size();

    // update zobrist and repetition history
    if (updateHash) {
        // XOR out piece from fromSquare (use ui.movedPiece)
        zobristKey ^= Zobrist::piece[pieceToIndex(ui.movedPiece)][fromSquare];

        // XOR out captured piece (use ui.capturedPiece and ui.capturedSquare for en-passant)
        if (ui.capturedPiece != Constants::Piece::EMPTY) {
            if (ui.capturedSquare != -1 && (flags & Flags::FLAG_EN_PASSANT))
                zobristKey ^= Zobrist::piece[pieceToIndex(ui.capturedPiece)][ui.capturedSquare];
            else
                zobristKey ^= Zobrist::piece[pieceToIndex(ui.capturedPiece)][toSquare];
        }

        // XOR in moved piece at toSquare (note promotions: squares[toSquare] already holds the new piece)
        int newPiece = isPromotion(flags) ? promotionPiece(flags, turn) : piece;
        zobristKey ^= Zobrist::piece[pieceToIndex(newPiece)][toSquare];

        // castling rights: XOR old and new
        zobristKey ^= Zobrist::castling[ui.castlingRights];
        zobristKey ^= Zobrist::castling[castling];

        // en passant: XOR old and new (use file index)
        if (ui.enPassantSquare != -1)
            zobristKey ^= Zobrist::enpassant[ui.enPassantSquare % 8];

        if (en_passant != -1)
            zobristKey ^= Zobrist::enpassant[en_passant % 8];

        // side to move
        zobristKey ^= Zobrist::side;

        // push new key into history
        repetitionHistory.push_back(zobristKey);
    }
}

void Board::unmakeMove(bool updateHash) {
    if (history.empty())
        return;

    undoInfo ui = history.back();
    history.pop_back();

    int flags      = ui.move.flags();
    int fromSquare = ui.move.from_square();
    int toSquare   = ui.move.to_square();
    int movedPiece = ui.movedPiece;
    int captured   = ui.capturedPiece;
    int capSq      = ui.capturedSquare;

    flipTurn();

    // Undo castling rook moves first (bitboards + mailbox)
    if (flags & Flags::FLAG_CASTLE_WK) {
        movePieceBB(Constants::Castling::W_ROOK_F1_END, Constants::Castling::W_ROOK_H1, Constants::Piece::W_ROOK);
        squares[Constants::Castling::W_ROOK_H1]     = Constants::Piece::W_ROOK;
        squares[Constants::Castling::W_ROOK_F1_END] = Constants::Piece::EMPTY;
    }
    if (flags & Flags::FLAG_CASTLE_WQ) {
        movePieceBB(Constants::Castling::W_ROOK_D1_END, Constants::Castling::W_ROOK_A1, Constants::Piece::W_ROOK);
        squares[Constants::Castling::W_ROOK_A1]     = Constants::Piece::W_ROOK;
        squares[Constants::Castling::W_ROOK_D1_END] = Constants::Piece::EMPTY;
    }
    if (flags & Flags::FLAG_CASTLE_BK) {
        movePieceBB(Constants::Castling::B_ROOK_F8_END, Constants::Castling::B_ROOK_H8, Constants::Piece::B_ROOK);
        squares[Constants::Castling::B_ROOK_H8]     = Constants::Piece::B_ROOK;
        squares[Constants::Castling::B_ROOK_F8_END] = Constants::Piece::EMPTY;
    }
    if (flags & Flags::FLAG_CASTLE_BQ) {
        movePieceBB(Constants::Castling::B_ROOK_D8_END, Constants::Castling::B_ROOK_A8, Constants::Piece::B_ROOK);
        squares[Constants::Castling::B_ROOK_A8]     = Constants::Piece::B_ROOK;
        squares[Constants::Castling::B_ROOK_D8_END] = Constants::Piece::EMPTY;
    }

    // Undo promotion or normal move
    if (isPromotion(flags)) {
        // promoted piece is on toSquare, remove it
        int promo = promotionPiece(flags, (ui.movedPiece > 0) ? Constants::Piece::WHITE : Constants::Piece::BLACK);
        removePieceBB(toSquare, promo);
        squares[toSquare] = Constants::Piece::EMPTY;

        // restore pawn on fromSquare (movedPiece gives color)
        int pawn = (ui.movedPiece > 0) ? Constants::Piece::W_PAWN : Constants::Piece::B_PAWN;
        addPieceBB(fromSquare, pawn);
        squares[fromSquare] = pawn;
    }
    else {
        // move piece back from toSquare to fromSquare
        movePieceBB(toSquare, fromSquare, movedPiece);
        squares[fromSquare] = movedPiece;
        squares[toSquare]   = Constants::Piece::EMPTY;
    }

    // Restore captured piece
    if (captured != Constants::Piece::EMPTY) {
        if (flags & Flags::FLAG_EN_PASSANT) {
            // captured pawn was on capSq
            if (isValidSquare(capSq)) {
                addPieceBB(capSq, captured);
                squares[capSq] = captured;
            }
        }
        else {
            // normal capture on toSquare
            addPieceBB(toSquare, captured);
            squares[toSquare] = captured;
        }
    }

    // restore
    castling          = ui.castlingRights;
    en_passant        = ui.enPassantSquare;
    halfmove          = ui.halfmoveClock;
    fullmove          = ui.fullmoveNumber;
    whiteKingPosition = ui.whiteKingPos;
    blackKingPosition = ui.blackKingPos;

    // Zobrist: restore previous key by popping repetition history (safer than incremental XOR mistakes)
    if (updateHash) {
        if (!repetitionHistory.empty())
            repetitionHistory.pop_back();
        if (!repetitionHistory.empty()) {
            zobristKey = repetitionHistory.back();
        }
        else {
            // fallback: recompute and push
            zobristKey = computeZobrist();
            repetitionHistory.push_back(zobristKey);
        }
    }
}

bool Board::enPassantAvailable() const {
    if (en_passant == -1)
        return false;

    const int enPassantRank = en_passant / 8;
    const int enPassantFile = en_passant % 8;

    const int pawn     = (turn == Constants::Piece::WHITE) ? Constants::Piece::W_PAWN : Constants::Piece::B_PAWN;
    const int pawnRank = (turn == Constants::Piece::WHITE) ? enPassantRank + 1 : enPassantRank - 1;

    if (pawnRank < 0 || pawnRank > 7)
        return false;

    int fileOffsets[2] = {1, -1};
    for (const int dFile : fileOffsets) {
        int adjacentFile = enPassantFile + dFile;
        if (adjacentFile < 0 || adjacentFile > 7)
            continue;

        const int square = pawnRank * 8 + adjacentFile;

        if (squares[square] == pawn)
            return true;
    }

    return false;
}

int Board::getEnPassantCaptureSquare() const {
    if (en_passant == -1)
        return -1;

    const int capSq =
        (turn == Constants::Piece::WHITE) ? en_passant + Constants::Squares::DOWN : en_passant + Constants::Squares::UP;

    if (capSq < 0 || capSq >= 64)
        return -1;

    return capSq;
}

bool Board::hasLegalMoves() const {
    Board copy = *this; // safe, cheap enough (apparently)
    MoveList moves;
    MoveGen::generateLegalMoves(copy, moves);
    return (moves.count > 0);
}

bool Board::isThreefoldRepetition() const {
    const u64 current = zobristKey;

    int count = 0;
    for (int i = lastIrreversiblePly; i < (int)repetitionHistory.size(); i++) {
        if (repetitionHistory[i] == current) {
            count++;
            if (count >= 3)
                return true;
        }
    }
    return false;
}

bool Board::isInsuffucientMaterial() const {
    int wBishops = 0;
    int bBishops = 0;
    int wKnights = 0;
    int bKnights = 0;

    bool whiteBishopLight = false;
    bool whiteBishopDark  = false;
    bool blackBishopLight = false;
    bool blackBishopDark  = false;

    for (int sq = 0; sq < 64; sq++) {
        int piece = squares[sq];
        if (piece == Constants::Piece::EMPTY)
            continue;

        switch (piece) {
        // pawns/rooks/queens always have possible mate
        case Constants::Piece::W_PAWN:
        case Constants::Piece::B_PAWN:
        case Constants::Piece::W_ROOK:
        case Constants::Piece::B_ROOK:
        case Constants::Piece::W_QUEEN:
        case Constants::Piece::B_QUEEN:
            return false;

        case Constants::Piece::W_BISHOP:
            wBishops++;
            // if row + col % 2 == 0, bishop is light
            if ((sq / 8 + sq % 8) % 2 == 0)
                whiteBishopLight = true;
            else
                whiteBishopDark = true;
            break;

        case Constants::Piece::B_BISHOP:
            bBishops++;
            if ((sq / 8 + sq % 8) % 2 == 0)
                blackBishopLight = true;
            else
                blackBishopDark = true;
            break;

        case Constants::Piece::W_KNIGHT:
            wKnights++;
            break;
        case Constants::Piece::B_KNIGHT:
            bKnights++;
            break;

        default:
            break; // kings dont matter
        }
    }

    const int totalBishops = wBishops + bBishops;
    const int totalKnights = wKnights + bKnights;

    // king vs king
    if (totalBishops == 0 && totalKnights == 0)
        return true;

    // king + bishop vs king
    if (totalBishops == 1 && totalKnights == 0)
        return true;

    // king + knight vs king
    if (totalBishops == 0 && totalKnights == 1)
        return true;

    // king + bishop vs king + bishop (same color bishops)
    if (totalBishops == 2 && totalKnights == 0) {
        if ((whiteBishopLight && blackBishopLight) || (whiteBishopDark && blackBishopDark))
            return true;
    }

    return false;
}

u64 Board::computeZobrist() const {
    u64 key = 0;

    // pieces
    for (int sq = 0; sq < 64; sq++) {
        int piece = squares[sq];
        if (piece != Constants::Piece::EMPTY) {
            int index = pieceToIndex(piece);
            key ^= Zobrist::piece[index][sq];
        }
    }

    key ^= Zobrist::castling[castling];

    // en passant file
    if (en_passant != -1) {
        int file = en_passant % 8;
        key ^= Zobrist::enpassant[file];
    }
    // side to move
    if (turn == Constants::Piece::BLACK)
        key ^= Zobrist::side;

    return key;
}

u64 Board::getCheckers() const {
    const int side   = getTurn();
    const int enemy  = side ^ 1;
    const int kingSq = getKingSquare(side);

    u64 checkers = 0ULL;

    u64* pawnAttacks = (side == Constants::Piece::WHITE) ? pawnAttacksWhite : pawnAttacksBlack;
    // pawn
    checkers |=
        pawnAttacks[kingSq] & (enemy == Constants::Piece::WHITE ? bitboards[PieceType::WP] : bitboards[PieceType::BP]);

    // knight
    checkers |=
        knightAttacks[kingSq] & (enemy == Constants::Piece::WHITE ? bitboards[PieceType::WN] : bitboards[PieceType::BN]);

    // bishop/queen diagonal
    u64 bishopLike = bishopAttacksMagic(kingSq, occupancyAll) &
                     (enemy == Constants::Piece::WHITE ? (bitboards[PieceType::WB] | bitboards[PieceType::WQ])
                                                       : (bitboards[PieceType::BB] | bitboards[PieceType::BQ]));
    checkers |= bishopLike;

    // rook/queen orthogonal
    u64 rookLike = rookAttacksMagic(kingSq, occupancyAll) &
                   (enemy == Constants::Piece::WHITE ? (bitboards[PieceType::WR] | bitboards[PieceType::WQ])
                                                     : (bitboards[PieceType::BR] | bitboards[PieceType::BQ]));
    checkers |= rookLike;

    return checkers;
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

        if (rank < 7)
            std::cout << "  ├───┼───┼───┼───┼───┼───┼───┼───┤\n";
        else
            std::cout << "  └───┴───┴───┴───┴───┴───┴───┴───┘\n";
    }
    std::cout << "   a   b   c   d   e   f   g   h\n";
}

void Board::setupStartPosition() {
    setupFromFen(Constants::FEN::startFen);
}

void Board::setupFromFen(const std::string& fen) {
    // clear everything
    squares.fill(Constants::Piece::EMPTY);

    // reset all bitboards
    bitboards.fill(0ULL);

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

            case 'p':
                setPieceAndBitboard(squareIndex, Constants::Piece::B_PAWN);
                break;

            case 'n':
                setPieceAndBitboard(squareIndex, Constants::Piece::B_KNIGHT);
                break;

            case 'b':
                setPieceAndBitboard(squareIndex, Constants::Piece::B_BISHOP);
                break;

            case 'r':
                setPieceAndBitboard(squareIndex, Constants::Piece::B_ROOK);
                break;
            case 'q':
                setPieceAndBitboard(squareIndex, Constants::Piece::B_QUEEN);
                break;
            case 'k':
                setPieceAndBitboard(squareIndex, Constants::Piece::B_KING);
                blackKingPosition = squareIndex;
                break;

            case 'P':
                setPieceAndBitboard(squareIndex, Constants::Piece::W_PAWN);
                break;
            case 'N':
                setPieceAndBitboard(squareIndex, Constants::Piece::W_KNIGHT);
                break;
            case 'B':
                setPieceAndBitboard(squareIndex, Constants::Piece::W_BISHOP);
                break;
            case 'R':
                setPieceAndBitboard(squareIndex, Constants::Piece::W_ROOK);
                break;
            case 'Q':
                setPieceAndBitboard(squareIndex, Constants::Piece::W_QUEEN);
                break;
            case 'K':
                setPieceAndBitboard(squareIndex, Constants::Piece::W_KING);
                whiteKingPosition = squareIndex;
                break;

            default:
                break;
            }
            squareIndex++;
        }
    }

    turn = (turnPart == "w") ? Constants::Piece::WHITE : Constants::Piece::BLACK;

    castling = 0;
    if (castlingPart.find('K') != std::string::npos)
        castling |= Constants::Castling::CASTLE_WK;
    if (castlingPart.find('Q') != std::string::npos)
        castling |= Constants::Castling::CASTLE_WQ;
    if (castlingPart.find('k') != std::string::npos)
        castling |= Constants::Castling::CASTLE_BK;
    if (castlingPart.find('q') != std::string::npos)
        castling |= Constants::Castling::CASTLE_BQ;

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

    occupancyWhite = bitboards[PieceType::WP] | bitboards[PieceType::WN] | bitboards[PieceType::WB] |
                     bitboards[PieceType::WR] | bitboards[PieceType::WQ] | bitboards[PieceType::WK];

    occupancyBlack = bitboards[PieceType::BP] | bitboards[PieceType::BN] | bitboards[PieceType::BB] |
                     bitboards[PieceType::BR] | bitboards[PieceType::BQ] | bitboards[PieceType::BK];

    occupancyAll = occupancyWhite | occupancyBlack;

    zobristKey = computeZobrist();
    repetitionHistory.clear();
    repetitionHistory.push_back(zobristKey);
}
