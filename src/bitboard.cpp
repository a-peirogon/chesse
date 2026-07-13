#include "bitboard.h"
#include <sstream>

const uint8_t LSB_LOOKUP[64] = {
     0,  1, 48,  2, 57, 49, 28,  3,
    61, 58, 50, 42, 38, 29, 17,  4,
    62, 55, 59, 36, 53, 51, 43, 22,
    45, 39, 33, 30, 24, 18, 12,  5,
    63, 47, 56, 27, 60, 41, 37, 16,
    54, 35, 52, 21, 44, 32, 23, 11,
    46, 26, 40, 15, 34, 20, 31, 10,
    25, 14, 19,  9, 13,  8,  7,  6
};

const uint8_t MSB_LOOKUP[64] = {
     0, 47,  1, 56, 48, 27,  2, 60,
    57, 49, 41, 37, 28, 16,  3, 61,
    54, 58, 35, 52, 50, 42, 21, 44,
    38, 32, 29, 23, 17, 11,  4, 62,
    46, 55, 26, 59, 40, 36, 15, 53,
    34, 51, 20, 43, 31, 22, 10, 45,
    25, 39, 14, 33, 19, 30,  9, 24,
    13, 18,  8, 12,  7,  6,  5, 63
};

std::string bbToStr(uint64_t bb) {
    std::ostringstream oss;
    for (int r = 7; r >= 0; --r) {
        for (int f = 0; f < 8; ++f) {
            Square sq = Square::fromPosition(static_cast<Rank>(r), static_cast<File>(f));
            oss << (isSet(bb, sq) ? '1' : '.');
        }
        oss << '\n';
    }
    return oss.str();
}
