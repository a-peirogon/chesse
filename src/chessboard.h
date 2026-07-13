#pragma once
#include <cstdint>
#include <array>
#include <optional>
#include <string>
#include "constants.h"
#include "square.h"
#include "move.h"
#include "bitboard.h"

class ChessBoard {
public:
    // pieces[color][piece]
    std::array<std::array<uint64_t, 6>, 2> pieces{};
    std::array<uint64_t, 2> combinedColor{};
    uint64_t combinedAll = 0ULL;
    Color color = Color::WHITE;

    ChessBoard() {
        for (auto& side : pieces)
            side.fill(0ULL);
        combinedColor.fill(0ULL);
    }

    std::string toString() const;

    uint64_t getPieceBB(Piece p, std::optional<Color> c = std::nullopt) const {
        Color col = c.value_or(color);
        return pieces[colorIdx(col)][static_cast<int>(p)];
    }

    std::optional<Piece> pieceOn(Square sq, std::optional<Color> c = std::nullopt) const {
        Color col = c.value_or(color);
        for (auto p : ALL_PIECES)
            if (isSet(getPieceBB(p, col), sq))
                return p;
        return std::nullopt;
    }

    void setSquare(Square sq, Piece p, std::optional<Color> c = std::nullopt) {
        Color col = c.value_or(color);
        int ci = colorIdx(col);
        int pi = static_cast<int>(p);
        pieces[ci][pi]    = ::setSquare(pieces[ci][pi], sq);
        combinedColor[ci] = ::setSquare(combinedColor[ci], sq);
        combinedAll       = ::setSquare(combinedAll, sq);
    }

    void clearSquare(Square sq, std::optional<Color> c = std::nullopt) {
        Color col = c.value_or(color);
        auto opt = pieceOn(sq, col);
        if (!opt.has_value()) return;
        int ci = colorIdx(col);
        int pi = static_cast<int>(opt.value());
        pieces[ci][pi]    = ::clearSquare(pieces[ci][pi], sq);
        combinedColor[ci] = ::clearSquare(combinedColor[ci], sq);
        combinedAll       = ::clearSquare(combinedAll, sq);
    }

    ChessBoard applyMove(const Move& move) const;
    void initGame();
};
