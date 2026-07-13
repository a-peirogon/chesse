#pragma once
#include <cstdint>
#include <string>
#include "constants.h"

class Square {
public:
    uint8_t index;

    explicit Square(uint8_t idx) : index(idx) {}

    static Square fromPosition(Rank r, File f) {
        return Square((static_cast<int>(r) << 3) | static_cast<int>(f));
    }

    static Square fromStr(const std::string& s) {
        uint8_t f = static_cast<uint8_t>(s[0] - 'A');
        uint8_t r = static_cast<uint8_t>(s[1] - '1');
        return Square((r << 3) | f);
    }

    std::string toString() const {
        char buf[3];
        buf[0] = 'A' + (index % 8);
        buf[1] = '1' + (index / 8);
        buf[2] = '\0';
        return std::string(buf);
    }

    uint64_t toBitboard() const {
        return uint64_t(1) << index;
    }
};
