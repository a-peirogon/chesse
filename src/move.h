#pragma once
#include <optional>
#include <string>
#include "square.h"
#include "constants.h"

struct Move {
    Square src;
    Square dest;
    std::optional<Piece> promo;

    Move(Square s, Square d, std::optional<Piece> p = std::nullopt)
        : src(s), dest(d), promo(p) {}

    std::string toString() const {
        if (promo.has_value()) {
            std::string pname(1, pieceToChar(promo.value()));
            return src.toString() + " -> " + dest.toString() + " = " + pname;
        }
        return src.toString() + " -> " + dest.toString();
    }
};
