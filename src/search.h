#pragma once
#include "chessboard.h"
#include "move.h"
#include <optional>

int negamax(const ChessBoard& board, int depth);
std::optional<Move> bestMove(const ChessBoard& board, int depth);
