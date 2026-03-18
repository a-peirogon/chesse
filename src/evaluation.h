#pragma once
#include <cstdint>
#include "chessboard.h"

namespace Score {
    constexpr int PAWN      =  100;
    constexpr int KNIGHT    =  300;
    constexpr int BISHOP    =  300;
    constexpr int ROOK      =  500;
    constexpr int QUEEN     =  900;
    constexpr int CHECKMATE = -1000000;
    constexpr int CENTER    =  5;
    constexpr int MOVE      =  5;
}

int evaluate(const ChessBoard& board);
