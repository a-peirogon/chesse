#pragma once
#include <string>

enum class Color : int {
    WHITE = 0,
    BLACK = 1
};

inline Color operator~(Color c) {
    return (c == Color::WHITE) ? Color::BLACK : Color::WHITE;
}

inline int colorIdx(Color c) { return static_cast<int>(c); }

enum class Piece : int {
    PAWN   = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK   = 3,
    QUEEN  = 4,
    KING   = 5
};

inline char pieceToChar(Piece p) {
    switch (p) {
        case Piece::PAWN:   return 'p';
        case Piece::KNIGHT: return 'n';
        case Piece::BISHOP: return 'b';
        case Piece::ROOK:   return 'r';
        case Piece::QUEEN:  return 'q';
        case Piece::KING:   return 'k';
        default:            return '?';
    }
}

inline Piece charToPiece(char c) {
    switch (c) {
        case 'p': return Piece::PAWN;
        case 'n': return Piece::KNIGHT;
        case 'b': return Piece::BISHOP;
        case 'r': return Piece::ROOK;
        case 'q': return Piece::QUEEN;
        case 'k': return Piece::KING;
        default:  return Piece::PAWN; // fallback
    }
}

inline const Piece ALL_PIECES[] = {
    Piece::PAWN, Piece::KNIGHT, Piece::BISHOP,
    Piece::ROOK, Piece::QUEEN,  Piece::KING
};
constexpr int NUM_PIECES = 6;

enum class Rank : int {
    ONE   = 0,
    TWO   = 1,
    THREE = 2,
    FOUR  = 3,
    FIVE  = 4,
    SIX   = 5,
    SEVEN = 6,
    EIGHT = 7
};

enum class File : int {
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    E = 4,
    F = 5,
    G = 6,
    H = 7
};
