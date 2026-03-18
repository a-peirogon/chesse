#include "chessboard.h"
#include <sstream>

std::string ChessBoard::toString() const {
    std::ostringstream oss;
    for (int r = 7; r >= 0; --r) {
        for (int f = 0; f < 8; ++f) {
            Square sq = Square::fromPosition(static_cast<Rank>(r), static_cast<File>(f));
            auto wp = pieceOn(sq, Color::WHITE);
            auto bp = pieceOn(sq, Color::BLACK);
            if (wp.has_value())
                oss << (char)toupper(pieceToChar(wp.value()));
            else if (bp.has_value())
                oss << pieceToChar(bp.value());
            else
                oss << '.';
        }
        oss << '\n';
    }
    oss << (color == Color::WHITE ? "WHITE" : "BLACK") << " to move";
    return oss.str();
}

ChessBoard ChessBoard::applyMove(const Move& move) const {
    ChessBoard nb = *this;

    auto p = pieceOn(move.src);
    nb.clearSquare(move.src);
    nb.clearSquare(move.dest, ~nb.color); // capture
    Piece placed = move.promo.value_or(p.value());
    nb.setSquare(move.dest, placed);
    nb.color = ~nb.color;
    return nb;
}

void ChessBoard::initGame() {
    pieces[colorIdx(Color::WHITE)][static_cast<int>(Piece::PAWN)]   = 0x000000000000FF00ULL;
    pieces[colorIdx(Color::WHITE)][static_cast<int>(Piece::KNIGHT)] = 0x0000000000000042ULL;
    pieces[colorIdx(Color::WHITE)][static_cast<int>(Piece::BISHOP)] = 0x0000000000000024ULL;
    pieces[colorIdx(Color::WHITE)][static_cast<int>(Piece::ROOK)]   = 0x0000000000000081ULL;
    pieces[colorIdx(Color::WHITE)][static_cast<int>(Piece::QUEEN)]  = 0x0000000000000008ULL;
    pieces[colorIdx(Color::WHITE)][static_cast<int>(Piece::KING)]   = 0x0000000000000010ULL;

    pieces[colorIdx(Color::BLACK)][static_cast<int>(Piece::PAWN)]   = 0x00FF000000000000ULL;
    pieces[colorIdx(Color::BLACK)][static_cast<int>(Piece::KNIGHT)] = 0x4200000000000000ULL;
    pieces[colorIdx(Color::BLACK)][static_cast<int>(Piece::BISHOP)] = 0x2400000000000000ULL;
    pieces[colorIdx(Color::BLACK)][static_cast<int>(Piece::ROOK)]   = 0x8100000000000000ULL;
    pieces[colorIdx(Color::BLACK)][static_cast<int>(Piece::QUEEN)]  = 0x0800000000000000ULL;
    pieces[colorIdx(Color::BLACK)][static_cast<int>(Piece::KING)]   = 0x1000000000000000ULL;

    for (int c = 0; c < 2; ++c) {
        combinedColor[c] = 0ULL;
        for (int p = 0; p < NUM_PIECES; ++p)
            combinedColor[c] |= pieces[c][p];
    }
    combinedAll = combinedColor[0] | combinedColor[1];
}
