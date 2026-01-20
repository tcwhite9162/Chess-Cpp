#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "bitboard/bitboard.hpp"
#include "core/undoInfo.hpp"

using u64 = uint64_t;
using u8  = uint8_t;

#define NO_DISCARD [[nodiscard]]

enum class PieceType : u8 { // used for index into bitboard array
    WP = 0,
    WN, WB,
    WR, WQ, WK,
    BP, BN, BB,
    BR, BQ, BK
};

struct PieceBitBoards {
    std::array<u64, 12> data{};

    u64& operator[](PieceType p) { return data[static_cast<u8>(p)]; }

    const u64& operator[](PieceType p) const { return data[static_cast<u8>(p)]; }

    void fill(u64 val) { data.fill(val); }
};

class Board {
  private:
    std::array<int, 64> squares;

    int turn;
    int halfmove;
    int fullmove;
    int castling;
    int en_passant;
    int lastIrreversiblePly;

    int blackKingPosition;
    int whiteKingPosition;

    u64 occupancyAll;
    u64 occupancyWhite;
    u64 occupancyBlack;
    PieceBitBoards bitboards;

    u64 zobristKey;

    std::vector<undoInfo> history;
    std::vector<u64> repetitionHistory;

    // internal helpers
    void updateCastlingRights(int fromSquare, int toSquare, int piece, int capturedPiece);

    void addPieceBB(int square, int piece);
    void removePieceBB(int square, int piece);
    void movePieceBB(int from, int to, int piece);
    void setPieceAndBitboard(int square, int piece);
    void removeCapturedPieceBB(int square, int piece);

  public:
    Board();

    void setupStartPosition();
    void setupFromFen(const std::string& fen);
    void printBoard() const;
    NO_DISCARD u64 computeZobrist() const;

    // state access
    NO_DISCARD inline int getTurn() const { return turn; }
    NO_DISCARD inline int getFullmove() const { return fullmove; }
    NO_DISCARD inline int getHalfmove() const { return halfmove; }
    NO_DISCARD inline int getEnPassant() const { return en_passant; }
    NO_DISCARD inline int getCastlingRights() const { return castling; }

    NO_DISCARD inline int getWhiteKingPos() const { return whiteKingPosition; }
    NO_DISCARD inline int getBlackKingPos() const { return blackKingPosition; }

    NO_DISCARD inline u64 getOccupancyAll() const { return occupancyAll; }
    NO_DISCARD inline u64 getOccupancyWhite() const { return occupancyWhite; }
    NO_DISCARD inline u64 getOccupancyBlack() const { return occupancyBlack; }
    NO_DISCARD inline const u64& getBitboard(PieceType piece) const { return bitboards[piece]; }

    NO_DISCARD inline u64 getZobristKey() const { return zobristKey; }

    // logic queries
    NO_DISCARD inline bool canCastleWhiteKingside() const { return castling & Constants::Castling::CASTLE_WK; }
    NO_DISCARD inline bool canCastleBlackKingside() const { return castling & Constants::Castling::CASTLE_BK; }
    NO_DISCARD inline bool canCastleWhiteQueenside() const { return castling & Constants::Castling::CASTLE_WQ; }
    NO_DISCARD inline bool canCastleBlackQueenside() const { return castling & Constants::Castling::CASTLE_BQ; }

    NO_DISCARD bool enPassantAvailable() const;
    NO_DISCARD int getEnPassantCaptureSquare() const;

    NO_DISCARD u64 getCheckers() const;

    NO_DISCARD int getKingSquare(int color) const {
        u64 bb = (color == Constants::Piece::WHITE) ? bitboards[PieceType::WK] : bitboards[PieceType::BK];
        return __builtin_ctzll(bb);
    }

    NO_DISCARD inline bool isInCheck(int color) const {
        const int kingSq = (color == Constants::Piece::WHITE) ? whiteKingPosition : blackKingPosition;
        const int enemy  = (color == Constants::Piece::WHITE ? Constants::Piece::BLACK : Constants::Piece::WHITE);
        return isSquareAttacked(kingSq, enemy);
    }

    NO_DISCARD inline bool isSquareAttacked(int square, int attackingColor) const { // looks terrible but i want the inline
        const bool white = (attackingColor == Constants::Piece::WHITE);

        const u64 pawnMask   = white ? pawnAttacksWhite[square] : pawnAttacksBlack[square];
        const u64 knightMask = knightAttacks[square];
        const u64 kingMask   = kingAttacks[square];

        const u64 pawnBB   = white ? bitboards[PieceType::WP] : bitboards[PieceType::BP];
        const u64 knightBB = white ? bitboards[PieceType::WN] : bitboards[PieceType::BN];
        const u64 bishopBB = white ? bitboards[PieceType::WB] : bitboards[PieceType::BB];
        const u64 rookBB   = white ? bitboards[PieceType::WR] : bitboards[PieceType::BR];
        const u64 queenBB  = white ? bitboards[PieceType::WQ] : bitboards[PieceType::BQ];
        const u64 kingBB   = white ? bitboards[PieceType::WK] : bitboards[PieceType::BK];

        if (pawnBB & pawnMask)
            return true;

        if (knightBB & knightMask)
            return true;

        if (kingBB & kingMask)
            return true;

        if ((bishopBB | queenBB) & bishopAttacksMagic(square, occupancyAll))
            return true;

        if ((rookBB | queenBB) & rookAttacksMagic(square, occupancyAll))
            return true;

        return false;
    }

    // terminal state
    NO_DISCARD bool hasLegalMoves() const;
    NO_DISCARD bool isInsuffucientMaterial() const;
    NO_DISCARD bool isThreefoldRepetition() const;

    NO_DISCARD inline bool isTerminal() const { return isCheckmate() || isDraw(); };
    NO_DISCARD inline bool isCheckmate() const { return isInCheck(turn) && !hasLegalMoves(); };
    NO_DISCARD inline bool isStalemate() const { return !isInCheck(turn) && !hasLegalMoves(); };
    NO_DISCARD inline bool isDraw() const {
        return isStalemate() || isInsuffucientMaterial() || isThreefoldRepetition() || halfmove >= 100;
    }

    // change state
    inline void flipTurn() { turn = -turn; };
    inline void setCastlingRights(int c) { castling = c; }
    inline void setEnPassant(int sq) { en_passant = sq; }

    void setPiece(int square, int piece);
    NO_DISCARD int getPiece(int square) const;

    void makeMove(Move move, bool updateHash = true);
    void unmakeMove(bool updateHash = true);
};
