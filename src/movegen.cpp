#include "movegen.h"
#include "tables.h"
#include "bitboard.h"
#include <stdexcept>

// ── Sliding pieces ────────────────────────────────────────────────────────────

uint64_t getDiagMovesBB(int i, uint64_t occ) {
    int f       = i & 7;
    occ         = DIAG_MASKS[i] & occ;
    occ         = (FILES[static_cast<int>(File::A)] * occ) >> 56;
    occ         = FILES[static_cast<int>(File::A)] * FIRST_RANK_MOVES[f][occ & 0xFF];
    return DIAG_MASKS[i] & occ;
}

uint64_t getAntidiagMovesBB(int i, uint64_t occ) {
    int f       = i & 7;
    occ         = ANTIDIAG_MASKS[i] & occ;
    occ         = (FILES[static_cast<int>(File::A)] * occ) >> 56;
    occ         = FILES[static_cast<int>(File::A)] * FIRST_RANK_MOVES[f][occ & 0xFF];
    return ANTIDIAG_MASKS[i] & occ;
}

uint64_t getRankMovesBB(int i, uint64_t occ) {
    int f       = i & 7;
    occ         = RANK_MASKS[i] & occ;
    occ         = (FILES[static_cast<int>(File::A)] * occ) >> 56;
    occ         = FILES[static_cast<int>(File::A)] * FIRST_RANK_MOVES[f][occ & 0xFF];
    return RANK_MASKS[i] & occ;
}

uint64_t getFileMovesBB(int i, uint64_t occ) {
    int f = i & 7;
    // Shift to A file
    occ = FILES[static_cast<int>(File::A)] & (occ >> f);
    // Map occupancy and index to first rank
    occ = (A1H8_DIAG * occ) >> 56;
    int firstRankIndex = (i ^ 56) >> 3;
    // Lookup and map back to H file
    occ = A1H8_DIAG * FIRST_RANK_MOVES[firstRankIndex][occ & 0xFF];
    // Isolate H file and shift back
    return (FILES[static_cast<int>(File::H)] & occ) >> (f ^ 7);
}

// ── Per-piece move bitboards ─────────────────────────────────────────────────

uint64_t getKingMovesBB(Square sq, const ChessBoard& board) {
    return KING_MOVES[sq.index] & ~board.combinedColor[colorIdx(board.color)];
}

uint64_t getKnightMovesBB(Square sq, const ChessBoard& board) {
    return KNIGHT_MOVES[sq.index] & ~board.combinedColor[colorIdx(board.color)];
}

uint64_t getPawnMovesBB(Square sq, const ChessBoard& board) {
    int c = colorIdx(board.color);
    uint64_t attacks = PAWN_ATTACKS[c][sq.index]
                       & board.combinedColor[colorIdx(~board.color)];
    uint64_t quiets  = EMPTY_BB;

    bool whiteFree = (Square(sq.index + 8).toBitboard() & board.combinedAll) == 0ULL;
    bool blackFree = (Square(sq.index - 8).toBitboard() & board.combinedAll) == 0ULL;

    if ((board.color == Color::WHITE && whiteFree) ||
        (board.color == Color::BLACK && blackFree))
        quiets = PAWN_QUIETS[c][sq.index] & ~board.combinedAll;

    return attacks | quiets;
}

uint64_t getBishopMovesBB(Square sq, const ChessBoard& board) {
    return (getDiagMovesBB(sq.index, board.combinedAll)
          ^ getAntidiagMovesBB(sq.index, board.combinedAll))
          & ~board.combinedColor[colorIdx(board.color)];
}

uint64_t getRookMovesBB(Square sq, const ChessBoard& board) {
    return (getRankMovesBB(sq.index, board.combinedAll)
          ^ getFileMovesBB(sq.index, board.combinedAll))
          & ~board.combinedColor[colorIdx(board.color)];
}

uint64_t getQueenMovesBB(Square sq, const ChessBoard& board) {
    return getRookMovesBB(sq, board) | getBishopMovesBB(sq, board);
}

// ── Move generators ───────────────────────────────────────────────────────────

std::vector<Move> genPieceMoves(Square src, const ChessBoard& board, Piece piece) {
    std::vector<Move> moves;

    if (piece == Piece::PAWN) {
        uint64_t moveset = getPawnMovesBB(src, board);
        bool whitePromote = (src.toBitboard() & RANKS[static_cast<int>(Rank::SEVEN)]) != 0ULL;
        bool blackPromote = (src.toBitboard() & RANKS[static_cast<int>(Rank::TWO)])   != 0ULL;

        if ((board.color == Color::WHITE && whitePromote) ||
            (board.color == Color::BLACK && blackPromote)) {
            occupiedSquares(moveset, [&](Square dest) {
                moves.emplace_back(src, dest, Piece::QUEEN);
                moves.emplace_back(src, dest, Piece::ROOK);
                moves.emplace_back(src, dest, Piece::KNIGHT);
                moves.emplace_back(src, dest, Piece::BISHOP);
            });
            return moves;
        }
        // fall through to regular move generation
        occupiedSquares(moveset, [&](Square dest) {
            moves.emplace_back(src, dest);
        });
        return moves;
    }

    uint64_t moveset;
    switch (piece) {
        case Piece::KNIGHT: moveset = getKnightMovesBB(src, board); break;
        case Piece::BISHOP: moveset = getBishopMovesBB(src, board); break;
        case Piece::ROOK:   moveset = getRookMovesBB(src, board);   break;
        case Piece::QUEEN:  moveset = getQueenMovesBB(src, board);  break;
        case Piece::KING:   moveset = getKingMovesBB(src, board);   break;
        default: throw std::runtime_error("Invalid piece");
    }

    occupiedSquares(moveset, [&](Square dest) {
        moves.emplace_back(src, dest);
    });
    return moves;
}

std::vector<Move> genMoves(const ChessBoard& board) {
    std::vector<Move> moves;
    for (auto piece : ALL_PIECES) {
        uint64_t pieceBB = board.getPieceBB(piece);
        occupiedSquares(pieceBB, [&](Square src) {
            auto pm = genPieceMoves(src, board, piece);
            moves.insert(moves.end(), pm.begin(), pm.end());
        });
    }
    return moves;
}

bool leavesInCheck(const ChessBoard& board, const Move& move) {
    ChessBoard nb = board.applyMove(move);
    nb.color = ~nb.color; // back to original mover's perspective

    Square myKingSq(static_cast<uint8_t>(
        lsbBitscan(nb.getPieceBB(Piece::KING))));

    Color opp = ~nb.color;

    // Pawn check
    uint64_t oppPawns = nb.getPieceBB(Piece::PAWN, opp);
    if ((PAWN_ATTACKS[colorIdx(nb.color)][myKingSq.index] & oppPawns) != 0ULL)
        return true;

    // Knight check
    uint64_t oppKnights = nb.getPieceBB(Piece::KNIGHT, opp);
    if ((getKnightMovesBB(myKingSq, nb) & oppKnights) != 0ULL)
        return true;

    // King adjacency
    uint64_t oppKing = nb.getPieceBB(Piece::KING, opp);
    if ((getKingMovesBB(myKingSq, nb) & oppKing) != 0ULL)
        return true;

    // Bishop/Queen diagonal
    uint64_t oppBishops = nb.getPieceBB(Piece::BISHOP, opp);
    uint64_t oppQueens  = nb.getPieceBB(Piece::QUEEN,  opp);
    if ((getBishopMovesBB(myKingSq, nb) & (oppBishops | oppQueens)) != 0ULL)
        return true;

    // Rook/Queen straight
    uint64_t oppRooks = nb.getPieceBB(Piece::ROOK, opp);
    if ((getRookMovesBB(myKingSq, nb) & (oppRooks | oppQueens)) != 0ULL)
        return true;

    return false;
}

std::vector<Move> genLegalMoves(const ChessBoard& board) {
    std::vector<Move> legal;
    for (auto& m : genMoves(board))
        if (!leavesInCheck(board, m))
            legal.push_back(m);
    return legal;
}
