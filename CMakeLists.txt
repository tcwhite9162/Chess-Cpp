cmake_minimum_required(VERSION 3.16)
project(ChessEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

file(GLOB SRC_FILES
    "src/*.cpp"
    "src/bitboard/*.cpp"
    "src/interface/*.cpp"
    "tests/*.cpp"
)

add_executable(main ${SRC_FILES})

target_include_directories(main PRIVATE
    src
    src/bitboard
    src/interface
    data
    tests
)
