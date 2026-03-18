#pragma once
#include "chessboard.h"
#include "move.h"
#include <string>

std::string squareToUCI(Square sq);
Square squareFromUCI(const std::string& s);
std::string moveToUCI(const Move& m);
Move moveFromUCI(const std::string& s);

// Parse a FEN string into a ChessBoard
ChessBoard parseFEN(const std::string& fen);

void runUCI();
