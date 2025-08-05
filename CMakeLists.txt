cmake_minimum_required(VERSION 3.30)
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
add_library(
        chess_lib
        src/GUI/gui.cpp
        src/GUI/VisualBoard.cpp
        src/GUI/DrawableEntity.cpp
        src/GUI/VisualPiece.cpp
        src/Utility/Vec2D.cpp
        src/Engine/EngineBase.cpp
        src/Engine/TestEngine.cpp
        src/BoardManager/BitBoards.cpp
        src/BoardManager/BoardManager.cpp
        src/Engine/Piece.cpp
        src/Utility/Fen.cpp
        src/ChessPlayer.cpp
        src/HumanPlayer.cpp
        include/BoardManager/Rules.h
        src/BoardManager/Rules.cpp
        src/MagicBitboards/MagicBitBoards.cpp
        src/MagicBitboards/MagicBitBoardShared.cpp
        src/MatchManager/MatchManager.cpp
        src/MatchManager/ManagerCommandHandler.cpp
)

target_include_directories(chess_lib PUBLIC include)
target_link_libraries(chess_lib PUBLIC SDL3::SDL3 SDL3_image::SDL3_image)

# === Main GUI Application ===
add_executable(Chess src/gameMain.cpp
)
target_link_libraries(Chess PRIVATE chess_lib EngineSharedLib)
target_include_directories(Chess PRIVATE include)


# === Add Tests ===
add_subdirectory(tests)

# === Add MBB tests ===
add_subdirectory(src/MagicBitBoardGen/tests)


# === Add MagicBitBoardGenerator Lib ===
add_library(MagicBitBoardGen STATIC
        src/MagicBitBoardGen/MagicNumberGenerator.cpp
        include/MagicBitBoardGen/MagicNumberGenerator.h
        include/MagicBitboards/MagicBitBoardShared.h
        src/MagicBitboards/MagicBitBoardShared.cpp
)
target_include_directories(MagicBitBoardGen PUBLIC include)
target_link_libraries(MagicBitBoardGen PRIVATE chess_lib)

# === Add MagicBitBoardGenerator exe ===
add_executable(GenerateMagicNumbers
        src/MagicBitBoardGen/mbbMain.cpp
)
target_link_libraries(GenerateMagicNumbers PRIVATE MagicBitBoardGen)
target_include_directories(GenerateMagicNumbers PRIVATE include)

# === Add Standalone Engine exe ===
add_executable(StandaloneEngine src/CLIEngine/CLIEngine.cpp)
target_link_libraries(StandaloneEngine chess_lib EngineSharedLib)
target_include_directories(StandaloneEngine PRIVATE include)

# === Add EngineShared lib ===
add_subdirectory(src/EngineShared)
