cmake_minimum_required(VERSION 3.31)
project(Chess)

set(CMAKE_CXX_STANDARD 20)

# === SDL Setup ===
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL3/lib/cmake")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL3_image/lib/cmake/SDL3_image")

find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3)
find_package(SDL3_image REQUIRED CONFIG)

# === GoogleTest Setup ===
include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# === Core Library ===
add_library(chess_lib
        src/GUI/gui.cpp
        include/GUI/gui.h
        src/GUI/VisualBoard.cpp
        include/GUI/VisualBoard.h
        src/GUI/DrawableEntity.cpp
        include/GUI/DrawableEntity.h
        src/GUI/VisualPiece.cpp
        include/GUI/VisualPiece.h
        include/Utility/Fen.h
        src/Utility/Vec2D.cpp
        include/Utility/Vec2D.h
        src/Engine/EngineBase.cpp
        include/Engine/EngineBase.h
        src/Engine/TestEngine.cpp
        include/Engine/TestEngine.h
        src/BoardManager/BitBoards.cpp
        include/BoardManager/BitBoards.h
        src/BoardManager/BoardManager.cpp
        include/BoardManager/BoardManager.h
        src/Engine/Piece.cpp
        include/Engine/Piece.h
        src/Utility/Fen.cpp
)

target_include_directories(chess_lib PUBLIC include)
target_link_libraries(chess_lib PUBLIC SDL3::SDL3 SDL3_image::SDL3_image)

# === Main Application ===
add_executable(Chess src/main.cpp
        include/Utility/math.h)
target_link_libraries(Chess PRIVATE chess_lib)

# === Add Tests ===
add_subdirectory(tests)
