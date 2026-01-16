#pragma once

#include <cstdint>
#include <vector>
#include <array>

#include "core/undoInfo.hpp"

using u64 = uint64_t;

#define NO_DISCARD [[nodiscard]]

enum PieceType {
    WP = 0,
    WN,
    WB,
    WR,
    WQ,
    WK,
    BP,
    BN,
    BB,
    BR,
    BQ,
    BK
};

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
    void updateOccupancy();

    // u64 whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
    // u64 blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;
    u64 occupancyWhite, occupancyBlack, occupancyAll;

    std::array<u64, 12> bitboards;

public:

    NO_DISCARD inline u64 getBitboard(PieceType p) const { return bitboards[p]; }

    NO_DISCARD inline u64 getOccupancyAll()   const { return occupancyAll; }
    NO_DISCARD inline u64 getOccupancyWhite() const { return occupancyWhite; }
    NO_DISCARD inline u64 getOccupancyBlack() const { return occupancyBlack; }


    NO_DISCARD inline int getTurn()           const { return turn; }
    NO_DISCARD inline int getCastlingRights() const { return castling; }
    NO_DISCARD inline int getEnPassant()      const { return en_passant; }
    NO_DISCARD inline int getFullmove()       const { return fullmove; }
    NO_DISCARD inline int getHalfmove()       const { return halfmove; }

    NO_DISCARD inline bool canCastleWhiteKingside()  const { return castling & Data::Castling::CASTLE_WK; }
    NO_DISCARD inline bool canCastleWhiteQueenside() const { return castling & Data::Castling::CASTLE_WQ; }
    NO_DISCARD inline bool canCastleBlackKingside()  const { return castling & Data::Castling::CASTLE_BK; }
    NO_DISCARD inline bool canCastleBlackQueenside() const { return castling & Data::Castling::CASTLE_BQ; }

    NO_DISCARD inline int getWhiteKingPos() const { return whiteKingPosition; }
    NO_DISCARD inline int getBlackKingPos() const { return blackKingPosition; }

    NO_DISCARD inline u64 getZobristKey() const { return zobristKey; }

    NO_DISCARD bool isInCheck(int color) const;
    NO_DISCARD bool isSquareAttacked(int square, int attackingColor) const;
    NO_DISCARD bool enPassantAvailable() const;
    NO_DISCARD int getEnPassantCaptureSquare() const;

    NO_DISCARD u64 computeZobrist() const;

    inline void setTurn(int t)           { turn = t; }
    inline void setCastlingRights(int c) { castling = c; }
    inline void setEnPassant(int sq)     { en_passant = sq; }

    NO_DISCARD bool hasLegalMoves();
    NO_DISCARD bool isDraw();
    NO_DISCARD bool isCheckmate();
    NO_DISCARD bool isStalemate();
    NO_DISCARD bool isInsuffucientMaterial();
    NO_DISCARD bool isThreefoldRepetition();
    NO_DISCARD bool isTerminal();

    // bitboard helpers
    u64 getCheckers() const;
    NO_DISCARD int getKingSquare(int side) const;

    Board();
    void setPiece(int square, int piece);
    NO_DISCARD int getPiece(int square) const;
    void flipTurn();
    void printBoard() const;
    void setupStartPosition();
    void setupFromFen(const std::string& fen);

    void makeMove(Move move, bool updateHash=true);
    void unmakeMove(Move move, bool updateHash=true);
};

