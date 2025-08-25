cmake_minimum_required(VERSION 3.30)
project(Chess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
# === Use SDL3, SDL3_image, SDL3_ttf from vcpkg ===
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(sdl3_ttf REQUIRED CONFIG)


# === Enable testing (GoogleTest still in tests/ folder) ===
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
        src/Engine/MainEngine.cpp
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
        src/EngineShared/StandaloneEngineWrapper.cpp
        src/CLIEngine/StandaloneUciPlayer.cpp
        src/Engine/Evaluation.cpp
        src/GUI/EvaluationBar.cpp
        src/EngineShared/CommandHandlerBase.cpp
        src/EngineShared/UCIParsing/UciParser.cpp
        src/EngineShared/UCIParsing/Tokeniser.cpp
        src/EngineShared/CommunicatorBase.cpp
        src/EngineShared/ZobristHash.cpp
        src/GUI/PromotionPiecePicker.cpp
        include/GUI/PromotionPiecePicker.h
)

target_include_directories(chess_lib PUBLIC include)
target_include_directories(chess_lib PUBLIC include/thirdparty)

target_link_libraries(chess_lib
        PUBLIC
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
)

# === Main GUI Application ===
add_executable(Chess src/gameMain.cpp)
target_link_libraries(Chess PRIVATE chess_lib)
target_include_directories(Chess PRIVATE include)

# === Add Tests ===
add_subdirectory(tests)


# === MagicBitBoardGenerator Lib ===
add_library(MagicBitBoardGen STATIC
        src/MagicBitBoardGen/MagicNumberGenerator.cpp
        include/MagicBitBoardGen/MagicNumberGenerator.h
        include/MagicBitboards/MagicBitBoardShared.h
        src/MagicBitboards/MagicBitBoardShared.cpp
)
target_include_directories(MagicBitBoardGen PUBLIC include)
target_link_libraries(MagicBitBoardGen PRIVATE chess_lib)

# === MagicBitBoardGenerator exe ===
add_executable(GenerateMagicNumbers src/MagicBitBoardGen/mbbMain.cpp)
target_link_libraries(GenerateMagicNumbers PRIVATE MagicBitBoardGen)
target_include_directories(GenerateMagicNumbers PRIVATE include)

# === Standalone Engine exe ===
add_executable(StandaloneEngine src/CLIEngine/CLIEngine.cpp)
target_link_libraries(StandaloneEngine chess_lib)
target_include_directories(StandaloneEngine PRIVATE include)


set(RESOURCE_DIR_VAR "${CMAKE_CURRENT_SOURCE_DIR}/resources")
add_compile_definitions(RESOURCE_DIR=\"${RESOURCE_DIR_VAR}\")

