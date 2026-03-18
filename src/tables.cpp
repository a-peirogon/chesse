#include "tables.h"
#include <cstdint>
#include <array>

// ── Storage ──────────────────────────────────────────────────────────────────

std::array<uint64_t, 8>  RANKS;
std::array<uint64_t, 8>  FILES;
std::array<uint64_t, 64> RANK_MASKS;
std::array<uint64_t, 64> FILE_MASKS;

std::array<uint64_t, 64> DIAG_MASKS;
std::array<uint64_t, 64> ANTIDIAG_MASKS;

std::array<uint64_t, 64>    KING_MOVES;
std::array<uint64_t, 64>    KNIGHT_MOVES;

std::array<std::array<uint64_t, 64>, 2> PAWN_QUIETS;
std::array<std::array<uint64_t, 64>, 2> PAWN_ATTACKS;

std::array<std::array<uint8_t, 256>, 8> FIRST_RANK_MOVES;

// ── Helpers ───────────────────────────────────────────────────────────────────

static uint64_t computeDiagMask(int i) {
    int diag  = 8 * (i & 7) - (i & 56);
    int north = (-diag) & (diag >> 31);
    int south = diag    & (-diag >> 31);
    return (A1H8_DIAG >> (uint8_t)south) << (uint8_t)north;
}

static uint64_t computeAntidiagMask(int i) {
    int diag  = 56 - 8 * (i & 7) - (i & 56);
    int north = (-diag) & (diag >> 31);
    int south = diag    & (-diag >> 31);
    return (H1A8_ANTIDIAG >> (uint8_t)south) << (uint8_t)north;
}

static uint64_t computeKingMoves(int i) {
    uint64_t bb = uint64_t(1) << i;
    uint64_t fa = FILES[static_cast<int>(File::A)];
    uint64_t fh = FILES[static_cast<int>(File::H)];

    uint64_t nw = (bb & ~fa) << 7;
    uint64_t n  =  bb        << 8;
    uint64_t ne = (bb & ~fh) << 9;
    uint64_t e  = (bb & ~fh) << 1;
    uint64_t se = (bb & ~fh) >> 7;
    uint64_t s  =  bb        >> 8;
    uint64_t sw = (bb & ~fa) >> 9;
    uint64_t w  = (bb & ~fa) >> 1;
    return nw | n | ne | e | se | s | sw | w;
}

static uint64_t computeKnightMoves(int i) {
    uint64_t bb = uint64_t(1) << i;
    uint64_t fa = FILES[static_cast<int>(File::A)];
    uint64_t fb = FILES[static_cast<int>(File::B)];
    uint64_t fg = FILES[static_cast<int>(File::G)];
    uint64_t fh = FILES[static_cast<int>(File::H)];

    uint64_t m1 = ~(fa | fb);
    uint64_t m2 = ~fa;
    uint64_t m3 = ~fh;
    uint64_t m4 = ~(fh | fg);

    uint64_t s1 = (bb & m1) << 6;
    uint64_t s2 = (bb & m2) << 15;
    uint64_t s3 = (bb & m3) << 17;
    uint64_t s4 = (bb & m4) << 10;
    uint64_t s5 = (bb & m4) >> 6;
    uint64_t s6 = (bb & m3) >> 15;
    uint64_t s7 = (bb & m2) >> 17;
    uint64_t s8 = (bb & m1) >> 10;
    return s1 | s2 | s3 | s4 | s5 | s6 | s7 | s8;
}

static uint64_t computePawnQuiet(int color, int i) {
    uint64_t bb = uint64_t(1) << i;
    uint64_t startRank = (color == static_cast<int>(Color::WHITE))
                       ? RANKS[static_cast<int>(Rank::TWO)]
                       : RANKS[static_cast<int>(Rank::SEVEN)];
    uint64_t s1, s2;
    if (color == static_cast<int>(Color::WHITE)) {
        s1 = bb << 8;
        s2 = (bb & startRank) << 16;
    } else {
        s1 = bb >> 8;
        s2 = (bb & startRank) >> 16;
    }
    return s1 | s2;
}

static uint64_t computePawnAttack(int color, int i) {
    uint64_t bb = uint64_t(1) << i;
    uint64_t fa = FILES[static_cast<int>(File::A)];
    uint64_t fh = FILES[static_cast<int>(File::H)];
    uint64_t s1, s2;
    if (color == static_cast<int>(Color::WHITE)) {
        s1 = (bb & ~fa) << 7;
        s2 = (bb & ~fh) << 9;
    } else {
        s1 = (bb & ~fa) >> 9;
        s2 = (bb & ~fh) >> 7;
    }
    return s1 | s2;
}

static uint8_t computeFirstRankMoves(int squareFile, int occ) {
    uint8_t x     = uint8_t(1) << squareFile;
    uint8_t o     = uint8_t(occ);

    // Left ray
    uint8_t leftAtk = uint8_t(x - uint8_t(1));
    uint8_t leftBlk = leftAtk & o;
    if (leftBlk) {
        int msb = 0;
        uint8_t tmp = leftBlk;
        while (tmp >>= 1) ++msb;
        // msb is the index of the highest set bit in leftBlk
        // Recompute properly:
        msb = 0;
        for (int b = 7; b >= 0; --b) {
            if (leftBlk & (uint8_t(1) << b)) { msb = b; break; }
        }
        uint8_t leftmost = uint8_t(1) << msb;
        uint8_t garbage  = uint8_t(leftmost - uint8_t(1));
        leftAtk ^= garbage;
    }

    // Right ray
    uint8_t rightAtk = ~x & ~uint8_t(x - uint8_t(1));
    uint8_t rightBlk = rightAtk & o;
    if (rightBlk) {
        int lsb = 0;
        for (int b = 0; b < 8; ++b) {
            if (rightBlk & (uint8_t(1) << b)) { lsb = b; break; }
        }
        uint8_t rightmost = uint8_t(1) << lsb;
        uint8_t garbage   = ~rightmost & ~uint8_t(rightmost - uint8_t(1));
        rightAtk ^= garbage;
    }

    return leftAtk ^ rightAtk;
}

// ── Public init ───────────────────────────────────────────────────────────────

void initTables() {
    // Ranks
    for (int i = 0; i < 8; ++i)
        RANKS[i] = uint64_t(0xFFULL) << (8 * i);

    // Files
    for (int i = 0; i < 8; ++i)
        FILES[i] = uint64_t(0x0101010101010101ULL) << i;

    // Rank / file masks per square
    for (int i = 0; i < 64; ++i) {
        RANK_MASKS[i] = RANKS[i / 8];
        FILE_MASKS[i] = FILES[i % 8];
    }

    // Diagonal masks
    for (int i = 0; i < 64; ++i) {
        DIAG_MASKS[i]     = computeDiagMask(i);
        ANTIDIAG_MASKS[i] = computeAntidiagMask(i);
    }

    // King / knight
    for (int i = 0; i < 64; ++i) {
        KING_MOVES[i]   = computeKingMoves(i);
        KNIGHT_MOVES[i] = computeKnightMoves(i);
    }

    // Pawn quiets / attacks
    for (int c = 0; c < 2; ++c)
        for (int i = 0; i < 64; ++i) {
            PAWN_QUIETS[c][i]  = computePawnQuiet(c, i);
            PAWN_ATTACKS[c][i] = computePawnAttack(c, i);
        }

    // First rank moves
    for (int f = 0; f < 8; ++f)
        for (int occ = 0; occ < 256; ++occ)
            FIRST_RANK_MOVES[f][occ] = computeFirstRankMoves(f, occ);
}
