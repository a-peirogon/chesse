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

// Full static evaluation, including correct checkmate/stalemate handling
// and legal-move-count based mobility. This generates legal moves
// internally, so it is relatively expensive — prefer evaluateMaterial()
// inside hot search loops where the caller already has (or can cheaply
// get) the legal move list.
int evaluate(const ChessBoard& board);

// Cheap material + center evaluation only (no move generation). Does NOT
// know about checkmate/stalemate — callers in search must handle terminal
// nodes themselves (see search.cpp).
int evaluateMaterial(const ChessBoard& board);
