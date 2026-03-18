#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include "square.h"
#include "constants.h"

/*
 * Little-endian rank-file mapping:
 *
 * 56 57 58 59 60 61 62 63    A8 B8 C8 D8 E8 F8 G8 H8
 * 48 49 50 51 52 53 54 55    A7 B7 C7 D7 E7 F7 G7 H7
 * 40 41 42 43 44 45 46 47    A6 B6 C6 D6 E6 F6 G6 H6
 * 32 33 34 35 36 37 38 39    A5 B5 C5 D5 E5 F5 G5 H5
 * 24 25 26 27 28 29 30 31    A4 B4 C4 D4 E4 F4 G4 H4
 * 16 17 18 19 20 21 22 23    A3 B3 C3 D3 E3 F3 G3 H3
 *  8  9 10 11 12 13 14 15    A2 B2 C2 D2 E2 F2 G2 H2
 *  0  1  2  3  4  5  6  7    A1 B1 C1 D1 E1 F1 G1 H1
 */

constexpr uint64_t EMPTY_BB = 0ULL;

// De Bruijn multiplication bitscan
constexpr uint64_t DEBRUIJN = 0x03f79d71b4cb0a89ULL;

extern const uint8_t LSB_LOOKUP[64];
extern const uint8_t MSB_LOOKUP[64];

inline int lsbBitscan(uint64_t bb) {
    return LSB_LOOKUP[((bb & (uint64_t)(-(int64_t)bb)) * DEBRUIJN) >> 58];
}

inline int msbBitscan(uint64_t bb) {
    bb |= bb >> 1;
    bb |= bb >> 2;
    bb |= bb >> 4;
    bb |= bb >> 8;
    bb |= bb >> 16;
    bb |= bb >> 32;
    return MSB_LOOKUP[(bb * DEBRUIJN) >> 58];
}

inline int popCount(uint64_t bb) {
    int count = 0;
    while (bb) {
        ++count;
        bb &= bb - 1;
    }
    return count;
}

inline bool isSet(uint64_t bb, Square sq) {
    return (sq.toBitboard() & bb) != 0ULL;
}

inline uint64_t clearSquare(uint64_t bb, Square sq) {
    return (~sq.toBitboard()) & bb;
}

inline uint64_t setSquare(uint64_t bb, Square sq) {
    return sq.toBitboard() | bb;
}

std::string bbToStr(uint64_t bb);

// Iterates over occupied squares (LSB first), calling callback(Square)
template<typename F>
inline void occupiedSquares(uint64_t bb, F callback) {
    while (bb != EMPTY_BB) {
        Square sq(static_cast<uint8_t>(lsbBitscan(bb)));
        callback(sq);
        bb ^= sq.toBitboard();
    }
}
