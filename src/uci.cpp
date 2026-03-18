#include "uci.h"
#include "search.h"
#include "tables.h"
#include "constants.h"
#include "movegen.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <cctype>

// ── Square helpers ────────────────────────────────────────────────────────────

std::string squareToUCI(Square sq) {
    char buf[3];
    buf[0] = static_cast<char>('a' + (sq.index % 8));
    buf[1] = static_cast<char>('1' + (sq.index / 8));
    buf[2] = '\0';
    return std::string(buf);
}

Square squareFromUCI(const std::string& s) {
    uint8_t f = static_cast<uint8_t>(s[0] - 'a');
    uint8_t r = static_cast<uint8_t>(s[1] - '1');
    return Square(static_cast<uint8_t>((r << 3) | f));
}

// ── Move helpers ──────────────────────────────────────────────────────────────

std::string moveToUCI(const Move& m) {
    std::string s = squareToUCI(m.src) + squareToUCI(m.dest);
    if (m.promo.has_value())
        s += pieceToChar(m.promo.value()); // always lowercase
        return s;
}

Move moveFromUCI(const std::string& s) {
    if (s.size() < 4)
        return Move(Square(0), Square(0));

    Square src  = squareFromUCI(s.substr(0, 2));
    Square dest = squareFromUCI(s.substr(2, 2));
    std::optional<Piece> promo = std::nullopt;
    if (s.size() >= 5)
        promo = charToPiece(static_cast<char>(std::tolower(s[4])));
    return Move(src, dest, promo);
}

// ── FEN parser ────────────────────────────────────────────────────────────────

ChessBoard parseFEN(const std::string& fen) {
    ChessBoard board;

    std::istringstream ss(fen);
    std::string placement, activeColor, castling, enpassant;
    int halfmove = 0, fullmove = 1;
    ss >> placement >> activeColor >> castling >> enpassant >> halfmove >> fullmove;

    // Piece placement (rank 8 first in FEN)
    int rank = 7, file = 0;
    for (char c : placement) {
        if (c == '/') {
            --rank;
            file = 0;
        } else if (c >= '1' && c <= '8') {
            file += c - '0';
        } else {
            Color col = std::isupper(static_cast<unsigned char>(c))
            ? Color::WHITE : Color::BLACK;
            Piece p   = charToPiece(static_cast<char>(std::tolower(c)));
            Square sq(static_cast<uint8_t>((rank << 3) | file));
            board.setSquare(sq, p, col);
            ++file;
        }
    }

    board.color = (activeColor == "w") ? Color::WHITE : Color::BLACK;

    // Castling and en-passant are not yet implemented in the engine;
    // they are parsed and silently ignored here.

    return board;
}

// ── UCI loop ──────────────────────────────────────────────────────────────────

void runUCI() {
    ChessBoard board;
    board.initGame();

    // Disable sync for faster I/O
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string line;
    while (std::getline(std::cin, line)) {
        // Trim trailing whitespace
        while (!line.empty() && std::isspace(static_cast<unsigned char>(line.back())))
            line.pop_back();

        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;

        // ── uci ──────────────────────────────────────────────────────────────
        if (cmd == "uci") {
            std::cout << "id name ChessEngine\n"
            << "id author Author\n"
            // Exposed options
            << "option name Hash type spin default 16 min 1 max 512\n"
            << "option name Depth type spin default 4 min 1 max 10\n"
            << "uciok\n"
            << std::flush;
        }

        // ── isready ───────────────────────────────────────────────────────────
        else if (cmd == "isready") {
            std::cout << "readyok\n" << std::flush;
        }

        // ── ucinewgame ────────────────────────────────────────────────────────
        else if (cmd == "ucinewgame") {
            board = ChessBoard();
            board.initGame();
        }

        // ── setoption ─────────────────────────────────────────────────────────
        else if (cmd == "setoption") {
            // Silently accepted; no options affect behaviour yet.
        }

        // ── position ──────────────────────────────────────────────────────────
        else if (cmd == "position") {
            std::string token;
            ss >> token;

            if (token == "startpos") {
                board = ChessBoard();
                board.initGame();
                // Consume optional "moves" keyword
                ss >> token; // may be "moves" or EOF
            } else if (token == "fen") {
                // Collect up to 6 FEN fields (stop at "moves")
                std::string fenParts[6];
                int count = 0;
                while (count < 6 && ss >> token) {
                    if (token == "moves") break;
                    fenParts[count++] = token;
                }
                std::string fenStr;
                for (int i = 0; i < count; ++i) {
                    if (i) fenStr += ' ';
                    fenStr += fenParts[i];
                }
                board = parseFEN(fenStr);
                // token is either "moves" or the last FEN field
                if (token != "moves")
                    ss >> token; // try to read "moves"
            }

            // Apply move list (token should be "moves" here)
            if (token == "moves") {
                std::string moveStr;
                while (ss >> moveStr)
                    board = board.applyMove(moveFromUCI(moveStr));
            }
        }

        // ── go ────────────────────────────────────────────────────────────────
        else if (cmd == "go") {
            int depth = 4;           // default search depth
            long long movetime = -1; // ms; -1 = not set

            std::string token;
            while (ss >> token) {
                if (token == "depth" && ss >> token) {
                    depth = std::stoi(token);
                } else if (token == "movetime" && ss >> token) {
                    movetime = std::stoll(token);
                } else if (token == "infinite") {
                    depth = 6; // cap to avoid infinite loop (no time mgmt yet)
                }
                // wtime / btime / winc / binc are ignored for now
            }
            (void)movetime; // time management not implemented

            auto best = bestMove(board, depth);
            if (best.has_value()) {
                std::cout << "bestmove " << moveToUCI(best.value()) << "\n";
            } else {
                // No legal moves (checkmate / stalemate)
                std::cout << "bestmove 0000\n";
            }
            std::cout << std::flush;
        }

        // ── stop ─────────────────────────────────────────────────────────────
        else if (cmd == "stop") {
            // No pondering implemented; nothing to stop.
        }

        // ── quit ─────────────────────────────────────────────────────────────
        else if (cmd == "quit") {
            break;
        }

        // ── debug helper: print board (non-standard) ──────────────────────────
        else if (cmd == "d") {
            std::cout << board.toString() << "\n" << std::flush;
        }

        // ── debug helper: list legal moves (non-standard) ────────────────────
        else if (cmd == "moves") {
            auto legal = genLegalMoves(board);
            for (auto& m : legal)
                std::cout << moveToUCI(m) << " ";
            std::cout << "\n" << std::flush;
        }
    }
}
