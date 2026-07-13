cmake_minimum_required(VERSION 3.14)
project(chess_engine CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chess
    src/game.cpp
    src/uci.cpp
    src/bitboard.cpp
    src/tables.cpp
    src/chessboard.cpp
    src/movegen.cpp
    src/evaluation.cpp
    src/search.cpp
)
target_include_directories(chess PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
