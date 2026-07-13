#pragma once
#include "chessboard.h"
#include "move.h"
#include <optional>

// Alpha-beta negamax search. alpha/beta default to the full window so
// existing callers that don't care about pruning bounds still work.
int negamax(const ChessBoard& board, int depth,
            int alpha = -2000000, int beta = 2000000);

std::optional<Move> bestMove(const ChessBoard& board, int depth);
