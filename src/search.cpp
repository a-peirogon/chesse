#include "search.h"
#include "movegen.h"
#include "evaluation.h"
#include <optional>
#include <limits>
#include <algorithm>

namespace {

// Standard piece values used purely for move ordering (MVV-LVA), kept
// local to search so they can be tuned independently of Score::* used
// in static evaluation.
int pieceOrderValue(Piece p) {
    switch (p) {
        case Piece::PAWN:   return 100;
        case Piece::KNIGHT: return 300;
        case Piece::BISHOP: return 300;
        case Piece::ROOK:   return 500;
        case Piece::QUEEN:  return 900;
        case Piece::KING:   return 10000;
        default:            return 0;
    }
}

// Score a move for ordering purposes: captures first, ranked by
// Most-Valuable-Victim / Least-Valuable-Attacker (MVV-LVA), then
// quiet (non-capture) moves after, all equal.
int moveOrderScore(const ChessBoard& board, const Move& move) {
    auto victim = board.pieceOn(move.dest, ~board.color);
    if (!victim.has_value())
        return 0; // quiet move

    auto attacker = board.pieceOn(move.src, board.color);
    int victimValue   = pieceOrderValue(victim.value());
    int attackerValue = attacker.has_value() ? pieceOrderValue(attacker.value()) : 0;

    // High victim value dominates; subtracting attacker value (scaled down)
    // breaks ties in favor of using the least valuable attacker.
    return 10000 + victimValue * 10 - attackerValue;
}

void orderMoves(const ChessBoard& board, std::vector<Move>& moves) {
    std::sort(moves.begin(), moves.end(), [&board](const Move& a, const Move& b) {
        return moveOrderScore(board, a) > moveOrderScore(board, b);
    });
}

} // namespace

int negamax(const ChessBoard& board, int depth, int alpha, int beta) {
    std::vector<Move> legal = genLegalMoves(board);

    if (legal.empty()) {
        // No legal moves: checkmate or stalemate. Prefer faster mates and
        // avoid slower ones by scaling the mate score with remaining depth,
        // so a mate found deeper in the tree (i.e. more plies from the
        // root) is worth less than one found shallower.
        if (isInCheck(board)) {
            // Being mated here is bad, but being mated with more depth
            // remaining (i.e. further from the root) should be *less* bad
            // than being mated immediately, so the search prefers to delay
            // an inevitable mate and, symmetrically (once negated one ply
            // up), prefers to deliver mate as quickly as possible.
            return Score::CHECKMATE + (1000 - depth);
        }
        return 0; // stalemate = draw
    }

    if (depth == 0)
        return evaluateMaterial(board) + Score::MOVE * static_cast<int>(legal.size());

    orderMoves(board, legal);

    int best = Score::CHECKMATE;
    for (const auto& move : legal) {
        ChessBoard nb = board.applyMove(move);
        int score = -negamax(nb, depth - 1, -beta, -alpha);
        if (score > best)
            best = score;
        if (best > alpha)
            alpha = best;
        if (alpha >= beta)
            break; // beta cutoff: opponent won't allow this line
    }
    return best;
}

std::optional<Move> bestMove(const ChessBoard& board, int depth) {
    std::vector<Move> legal = genLegalMoves(board);
    if (legal.empty())
        return std::nullopt;

    orderMoves(board, legal);

    int alpha = -2000000;
    const int beta = 2000000;
    std::optional<Move> best = std::nullopt;

    for (const auto& move : legal) {
        ChessBoard nb = board.applyMove(move);
        int score = -negamax(nb, depth - 1, -beta, -alpha);
        if (!best.has_value() || score > alpha) {
            alpha = score;
            best = move;
        }
    }
    return best;
}
