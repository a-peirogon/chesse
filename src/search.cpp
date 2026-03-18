#include "search.h"
#include "movegen.h"
#include "evaluation.h"
#include <optional>
#include <limits>

int negamax(const ChessBoard& board, int depth) {
    if (depth == 0)
        return evaluate(board);

    int maxScore = Score::CHECKMATE;
    for (const auto& move : genLegalMoves(board)) {
        ChessBoard nb = board.applyMove(move);
        int score = -negamax(nb, depth - 1);
        if (score > maxScore)
            maxScore = score;
    }
    return maxScore;
}

std::optional<Move> bestMove(const ChessBoard& board, int depth) {
    int maxScore = Score::CHECKMATE;
    std::optional<Move> best = std::nullopt;

    for (const auto& move : genLegalMoves(board)) {
        ChessBoard nb = board.applyMove(move);
        int score = -negamax(nb, depth - 1);
        if (score > maxScore) {
            maxScore = score;
            best = move;
        }
    }
    return best;
}
