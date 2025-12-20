#ifndef MOVE_H
#define MOVE_H

#include <string>

struct Move {
    unsigned int data;

    Move() : data(0) {};

    Move(int from_sq, int to_sq, int flags = 0) {
        data = from_sq | (to_sq << 6) | (flags << 12);
    }
    
    static unsigned int encode(int from_sq, int to_sq, int flags = 0) {
        return from_sq | (to_sq << 6) | (flags << 12);
    }

    int from_square() const {return data & 0x3f;}
    int to_square() const {return (data >> 6) & 0x3f;}
    int flags() const {return (data >> 12) & 0x3f;}

    std::string to_string() const {
        auto squareToString = [](int sq) {
            char file = 'a' + (sq % 8);
            char rank = '8' - (sq / 8);
            return std::string{file} + rank;
        };
        return squareToString(from_square()) + squareToString(to_square());
    }
};

#endif