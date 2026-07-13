#pragma once
#include <cstdint>
#include <array>
#include "constants.h"
#include "bitboard.h"
#include "square.h"

// ── Rank / File masks ────────────────────────────────────────────────────────

extern std::array<uint64_t, 8>  RANKS;
extern std::array<uint64_t, 8>  FILES;
extern std::array<uint64_t, 64> RANK_MASKS;
extern std::array<uint64_t, 64> FILE_MASKS;

constexpr uint64_t A1H8_DIAG    = 0x8040201008040201ULL;
constexpr uint64_t H1A8_ANTIDIAG = 0x0102040810204080ULL;
constexpr uint64_t CENTER        = 0x00003C3C3C3C0000ULL;

// ── Diagonal / anti-diagonal masks ──────────────────────────────────────────

extern std::array<uint64_t, 64> DIAG_MASKS;
extern std::array<uint64_t, 64> ANTIDIAG_MASKS;

// ── Piece move tables ────────────────────────────────────────────────────────

extern std::array<uint64_t, 64>    KING_MOVES;
extern std::array<uint64_t, 64>    KNIGHT_MOVES;

// PAWN_QUIETS[color][square], PAWN_ATTACKS[color][square]
extern std::array<std::array<uint64_t, 64>, 2> PAWN_QUIETS;
extern std::array<std::array<uint64_t, 64>, 2> PAWN_ATTACKS;

// FIRST_RANK_MOVES[file_index][occupancy_byte]
extern std::array<std::array<uint8_t, 256>, 8> FIRST_RANK_MOVES;

// ── Initialisation (call once at startup) ───────────────────────────────────
void initTables();
