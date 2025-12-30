#include "../data/constants.hpp"


inline int rank(int square) { return square / 8; }
inline int file(int square) { return square % 8; }
inline bool isValidSquare(int square) {return square >= 0 && square < 64;}

inline bool isCapture(int flags)     { return flags & FLAG_CAPTURE; }
inline bool isPromotion(int flags)   { return flags & FLAG_PROMOTION; }

inline int promotionPiece(int flags, int turn) {
    if (flags & FLAG_PROMO_N) { return (turn == WHITE) ? W_KNIGHT : B_KNIGHT; }
    if (flags & FLAG_PROMO_B) { return (turn == WHITE) ? W_BISHOP : B_BISHOP; }
    if (flags & FLAG_PROMO_R) { return (turn == WHITE) ? W_ROOK : B_ROOK; }
    if (flags & FLAG_PROMO_Q) { return (turn == WHITE) ? W_QUEEN : B_QUEEN; }
    return EMPTY;
}

inline bool isOpponent(int piece, int targetPiece) {
    return (piece > 0 && targetPiece < 0) || (piece < 0 && targetPiece > 0);
}

inline int pieceToIndex(int piece) {
    switch (piece) {
        case W_PAWN:   return 0;
        case W_KNIGHT: return 1;
        case W_BISHOP: return 2;
        case W_ROOK:   return 3;
        case W_QUEEN:  return 4;
        case W_KING:   return 5;

        case B_PAWN:   return 6;
        case B_KNIGHT: return 7;
        case B_BISHOP: return 8;
        case B_ROOK:   return 9;
        case B_QUEEN:  return 10;
        case B_KING:   return 11;
    }
    return -1; // should never happen
}




inline char pieceToChar(int piece) {
    switch (piece) {
        case W_PAWN:   return 'P';
        case W_KNIGHT: return 'N';
        case W_BISHOP: return 'B';
        case W_ROOK:   return 'R';
        case W_QUEEN:  return 'Q';
        case W_KING:   return 'K';

        case B_PAWN:   return 'p';
        case B_KNIGHT: return 'n';
        case B_BISHOP: return 'b';
        case B_ROOK:   return 'r';
        case B_QUEEN:  return 'q';
        case B_KING:   return 'k';

        case EMPTY:    return '.';
        default:       return '?'; // unknown
    }
}