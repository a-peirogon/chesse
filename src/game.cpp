#include <iostream>
#include <string>
#include <optional>
#include "chessboard.h"
#include "move.h"
#include "square.h"
#include "constants.h"
#include "search.h"
#include "tables.h"
#include "uci.h"

// ── Interactive (human vs engine) mode ───────────────────────────────────────

static Move getMove() {
    std::string src, dest, promo;
    std::cout << "From: "; std::cin >> src;
    std::cout << "To:   "; std::cin >> dest;
    std::cout << "Promo (- for none): "; std::cin >> promo;

    std::optional<Piece> promoPiece = std::nullopt;
    if (!promo.empty() && promo != "-") {
        char c = static_cast<char>(std::tolower(promo[0]));
        for (auto p : ALL_PIECES)
            if (pieceToChar(p) == c)
                promoPiece = p;
    }
    // Squares entered in algebraic notation: A1-H8 (case-insensitive)
    for (auto& ch : src)  ch = static_cast<char>(std::toupper(ch));
    for (auto& ch : dest) ch = static_cast<char>(std::toupper(ch));
    return Move(Square::fromStr(src), Square::fromStr(dest), promoPiece);
}

static void runInteractive() {
    ChessBoard board;
    board.initGame();

    std::cout << "=== Chess Engine – Interactive Mode ===\n\n"
    << board.toString() << "\n\n";

    while (true) {
        std::cout << "Your move (e.g. E2 E4, promo - for none)\n";
        Move playerMove = getMove();
        board = board.applyMove(playerMove);
        std::cout << "\n" << board.toString() << "\n\n";

        auto engineMoveOpt = bestMove(board, 4);
        if (!engineMoveOpt.has_value()) {
            std::cout << "No legal moves — game over.\n";
            break;
        }
        Move engineMove = engineMoveOpt.value();
        std::cout << "Engine plays: " << engineMove.toString() << "\n\n";
        board = board.applyMove(engineMove);
        std::cout << board.toString() << "\n\n";
    }
}

// ── Entry point ───────────────────────────────────────────────────────────────

int main(int argc, char* argv[]) {
    initTables();

    // Pass --interactive (or -i) to play against the engine in the terminal.
    // Default: run the UCI protocol (for GUIs such as Arena, Cutechess, etc.)
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--interactive" || arg == "-i") {
            runInteractive();
            return 0;
        }
    }

    runUCI();
    return 0;
}
