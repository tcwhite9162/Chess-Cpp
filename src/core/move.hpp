#pragma once

#include <string>

#include "constants.hpp"

#define NO_DISCARD [[nodiscard]]

struct Move {

    int data;
    int score = 0;

    Move() : data(0) {};

    static Move nullMove() { return {}; }

    Move(int from_sq, int to_sq, int flags = 0) {
        data = from_sq | (to_sq << 6) | (flags << 12);
    }

    bool operator==(const Move& other) const {
        return (other.to_square() == to_square() &&
        other.from_square() == from_square() &&
        other.flags() == flags());
    }

    NO_DISCARD int from_square() const {return data & 0x3f;}
    NO_DISCARD int to_square() const {return (data >> 6) & 0x3f;}
    NO_DISCARD int flags() const {return (data >> 12) & 0xfff;}

    static unsigned int encode(int from_sq, int to_sq, int flags = 0) {
        return from_sq | (to_sq << 6) | (flags << 12);
    }

    NO_DISCARD bool isValid() const {
        return data != 0;
    }

    NO_DISCARD bool isCapture()    const { return flags() & (Flags::FLAG_CAPTURE | Flags::FLAG_EN_PASSANT); }
    NO_DISCARD bool isPromotion()  const { return flags() &  Flags::FLAG_PROMOTION; }
    NO_DISCARD bool isEnPassant()  const { return flags() &  Flags::FLAG_EN_PASSANT; }
    NO_DISCARD bool isDoublePush() const { return flags() &  Flags::FLAG_DOUBLE_PUSH; }
    NO_DISCARD bool isCastle()     const { return flags() &  Flags::FLAG_CASTLE_ANY; }

    NO_DISCARD bool isQuiet() const {
        return !(flags() & (Flags::FLAG_EN_PASSANT| Flags::FLAG_CAPTURE | Flags::FLAG_PROMOTION | Flags::FLAG_CASTLE_ANY));
    }

    NO_DISCARD int promotionPiece() const {
        if (!(flags() & Flags::FLAG_PROMOTION)) return 0;

        if (flags() & Flags::FLAG_PROMO_Q) return Constants::Piece::QUEEN;
        if (flags() & Flags::FLAG_PROMO_R) return Constants::Piece::ROOK;
        if (flags() & Flags::FLAG_PROMO_B) return Constants::Piece::BISHOP;
        if (flags() & Flags::FLAG_PROMO_N) return Constants::Piece::KNIGHT;

        return 0;
    }

    NO_DISCARD std::string to_string() const {
        auto squareToString = [](int sq) {
            const char file = 'a' + (sq % 8);
            const char rank = '8' - (sq / 8);
            return std::string{file} + rank;
        };
        return squareToString(from_square()) + squareToString(to_square());
    }
};
