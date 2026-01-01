#pragma once

#include <cstdint>
#include <vector>
#include <array>

#include "../data/constants.hpp"
#include "move.hpp"
#include "undoInfo.hpp"

class Board {
private:
    std::array<int, 64> squares;
    int turn;
    int castling;
    int en_passant;
    int halfmove;
    int fullmove;
    int blackKingPosition;
    int whiteKingPosition;

    
    uint64_t zobristKey;
    std::vector<uint64_t> repetitionHistory;
    int lastIrreversiblePly;
    
    std::vector<undoInfo> history;
    
    void updateCastlingRights(int fromSquare, int toSquare, int piece, int capturedPiece);
    
    // bitboards
    void setPieceAndBitboard(int square, int piece);
    
    void removePieceBB(int square, int piece);
    void addPieceBB(int square, int piece);
    void movePieceBB(int from, int to, int piece);
    void removeCapturedPieceBB(int square, int piece);
    void handlePromotionBB(int from, int to, int promotionPiece, int color);
    void updateOccupancy();
    
public:

    u64 whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
    u64 blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;
    u64 occupancyWhite, occupancyBlack, occupancyAll;
    
    int getTurn() const {return turn;}
    int getCastlingRights() const {return castling;}
    int getEnPassant() const {return en_passant;}

    bool canCastleWhiteKingside()  const { return castling & CASTLE_WK; }
    bool canCastleWhiteQueenside() const { return castling & CASTLE_WQ; }
    bool canCastleBlackKingside()  const { return castling & CASTLE_BK; }
    bool canCastleBlackQueenside() const { return castling & CASTLE_BQ; }

    int getWhiteKingPos() const { return whiteKingPosition; }
    int getBlackKingPos() const { return blackKingPosition; }

    void setTurn(int t) {turn = t;}
    void setCastlingRights(int c) {castling = c;}
    void setEnPassant(int sq) {en_passant = sq;}

    bool isInCheck(int color) const;
    bool isSquareAttacked(int square, int attackingColor) const;
    bool enPassantAvailable() const;

    uint64_t computeZobrist() const;
    
    bool hasLegalMoves();
    bool isDraw();
    bool isCheckmate();
    bool isStalemate();
    bool isInsuffucientMaterial();
    bool isThreefoldRepetition();
    bool isTerminal();

    Board();
    void setPiece(int square, int piece);
    int getPiece(int square) const;
    void flipTurn();
    void printBoard() const;
    void setupStartPosition();

    void makeMove(Move move, bool updateHash=true);
    void unmakeMove(Move move, bool updateHash=true);
};

