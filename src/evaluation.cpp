#include "evaluation.h"
#include "bitboard.h"
#include "movegen.h"
#include "tables.h"

static int pieceDiff(const ChessBoard& board, Piece p) {
    return popCount(board.getPieceBB(p, board.color))
         - popCount(board.getPieceBB(p, ~board.color));
}

static int evalPieces(const ChessBoard& board) {
    return Score::PAWN   * pieceDiff(board, Piece::PAWN)
         + Score::KNIGHT * pieceDiff(board, Piece::KNIGHT)
         + Score::BISHOP * pieceDiff(board, Piece::BISHOP)
         + Score::ROOK   * pieceDiff(board, Piece::ROOK)
         + Score::QUEEN  * pieceDiff(board, Piece::QUEEN);
}

static int evalCenter(const ChessBoard& board) {
    return Score::CENTER
         * popCount(board.combinedColor[colorIdx(board.color)] & CENTER);
}

int evaluateMaterial(const ChessBoard& board) {
    return evalPieces(board) + evalCenter(board);
}

// Full static evaluation from the perspective of board.color.
int evaluate(const ChessBoard& board) {
    auto legal = genLegalMoves(board);
    if (legal.empty()) {
        // No legal moves: either checkmate or stalemate.
        if (isInCheck(board))
            return Score::CHECKMATE; // getting mated is as bad as it gets
        return 0;                    // stalemate = draw
    }
    int n = static_cast<int>(legal.size());
    return evaluateMaterial(board) + Score::MOVE * n;
}
