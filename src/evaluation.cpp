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

static int evalMoves(const ChessBoard& board) {
    int n = static_cast<int>(genLegalMoves(board).size());
    if (n == 0) return Score::CHECKMATE;
    return Score::MOVE * n;
}

int evaluate(const ChessBoard& board) {
    return evalPieces(board) + evalCenter(board) + evalMoves(board);
}
