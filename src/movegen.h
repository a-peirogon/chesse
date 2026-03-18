#pragma once
#include <vector>
#include "chessboard.h"
#include "move.h"

// Sliding piece move bitboards
uint64_t getDiagMovesBB(int i, uint64_t occ);
uint64_t getAntidiagMovesBB(int i, uint64_t occ);
uint64_t getRankMovesBB(int i, uint64_t occ);
uint64_t getFileMovesBB(int i, uint64_t occ);

// Per-piece move bitboards
uint64_t getKingMovesBB(Square sq, const ChessBoard& board);
uint64_t getKnightMovesBB(Square sq, const ChessBoard& board);
uint64_t getPawnMovesBB(Square sq, const ChessBoard& board);
uint64_t getBishopMovesBB(Square sq, const ChessBoard& board);
uint64_t getRookMovesBB(Square sq, const ChessBoard& board);
uint64_t getQueenMovesBB(Square sq, const ChessBoard& board);

// Move generation
std::vector<Move> genPieceMoves(Square src, const ChessBoard& board, Piece piece);
std::vector<Move> genMoves(const ChessBoard& board);      // pseudo-legal
std::vector<Move> genLegalMoves(const ChessBoard& board); // legal
bool leavesInCheck(const ChessBoard& board, const Move& move);
