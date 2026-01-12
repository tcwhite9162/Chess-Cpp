#pragma once

#include <string>

#include "constants.hpp"

struct Move {
    
    unsigned int data;
    int score = 0;
    
    Move() : data(0) {};

    static const Move nullMove() { return Move(); }
    
    Move(int from_sq, int to_sq, int flags = 0) {
        data = from_sq | (to_sq << 6) | (flags << 12);
    }
    
    bool operator==(const Move& other) const {
        return (other.to_square() == to_square() &&
        other.from_square() == from_square() &&
        other.flags() == flags());
    }
    
    int from_square() const {return data & 0x3f;}
    int to_square() const {return (data >> 6) & 0x3f;}
    int flags() const {return (data >> 12) & 0xfff;}
    
    static unsigned int encode(int from_sq, int to_sq, int flags = 0) {
        return from_sq | (to_sq << 6) | (flags << 12);
    }
    
    bool isValid() const {
        return data != 0; 
    }
    
    bool isCapture() const { return flags() & (FLAG_CAPTURE | FLAG_EN_PASSANT); }
    bool isPromotion() const { return flags() & FLAG_PROMOTION; }
    bool isEnPassant() const { return flags() & FLAG_EN_PASSANT; }
    bool isDoublePush() const { return flags() & FLAG_DOUBLE_PUSH; }
    bool isCastle() const { return flags() & FLAG_CASTLE_ANY; }
    
    bool isQuiet() const {
        return !(flags() & (FLAG_EN_PASSANT| FLAG_CAPTURE | FLAG_PROMOTION | FLAG_CASTLE_ANY));
    }
    
    int promotionPiece() const {
        if (!(flags() & FLAG_PROMOTION)) return 0;
        
        if (flags() & FLAG_PROMO_Q) return QUEEN;
        if (flags() & FLAG_PROMO_R) return ROOK;
        if (flags() & FLAG_PROMO_B) return BISHOP;
        if (flags() & FLAG_PROMO_N) return KNIGHT;
        
        return 0;
    }
    
    std::string to_string() const {
        auto squareToString = [](int sq) {
            char file = 'a' + (sq % 8);
            char rank = '8' - (sq / 8);
            return std::string{file} + rank;
        };
        return squareToString(from_square()) + squareToString(to_square());
    }
};