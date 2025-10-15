cmake_minimum_required(VERSION 3.30)
project(Chess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
# === Use SDL3, SDL3_image, SDL3_ttf from vcpkg ===
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(sdl3_ttf REQUIRED CONFIG)


enable_testing()

# === Core Engine Library: engine_lib ===
add_library(
        engine_lib
        src/Utility/Vec2D.cpp
        src/Engine/ChessEngine.cpp
        src/BoardManager/BitBoards.cpp
        src/BoardManager/BoardManager.cpp
        src/Engine/Piece.cpp
        src/Utility/Fen.cpp
        src/Engine/ChessPlayer.cpp
        src/Engine/HumanPlayer.cpp
        include/BoardManager/Rules.h
        src/BoardManager/Rules.cpp
        src/MagicBitboards/MagicBitBoards.cpp
        src/MagicBitboards/MagicBitBoardShared.cpp
        src/MatchManager/MatchManager.cpp
        src/MatchManager/ManagerCommandHandler.cpp
        src/Engine/Evaluation.cpp
        src/Engine/CommandHandlerBase.cpp
        src/UCIParsing/UciParser.cpp
        src/UCIParsing/Tokeniser.cpp
        src/Engine/ZobristHash.cpp
        src/Engine/ProcessChessEngine.cpp
        include/Engine/EvaluationValues.h
        src/Engine/TranspositionTable.cpp
        src/Engine/RepetitionTable.cpp
        src/BoardManager/Referee.cpp
        include/BoardManager/Referee.h
        include/BoardManager/Move.h
        src/BoardManager/Move.cpp
        include/Utility/ChessUtility.h
        src/Utility/ChessUtility.cpp
        src/Engine/SearchHelpers.cpp
        src/Engine/MoveGenerator.cpp
        include/Engine/MoveGenerator.h
)

target_link_libraries(
        engine_lib
        PUBLIC
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
)
target_include_directories(engine_lib PUBLIC include)
target_include_directories(engine_lib PUBLIC include/thirdparty)

# === GUI Library: gui_lib ===
add_library(
        gui_lib
        src/GUI/gui.cpp
        src/GUI/VisualBoard.cpp
        src/GUI/DrawableEntity.cpp
        src/GUI/VisualPiece.cpp
        src/Utility/Vec2D.cpp
        src/GUI/EvaluationBar.cpp
        src/GUI/PromotionPiecePicker.cpp
)
target_include_directories(gui_lib PUBLIC include)
target_include_directories(gui_lib PUBLIC include/thirdparty)

target_link_libraries(
        gui_lib
        PUBLIC
        engine_lib
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
)

# === Main GUI Application ===
add_executable(Chess src/gameMain.cpp)
target_include_directories(Chess PRIVATE include)
target_link_libraries(Chess PRIVATE gui_lib engine_lib)

# === MagicBitBoardGenerator Lib ===
add_library(MagicBitBoardGen STATIC
        src/MagicBitboards/MagicNumberGenerator.cpp
        include/MagicBitboards/MagicNumberGenerator.h
        include/MagicBitboards/MagicBitBoardShared.h
        src/MagicBitboards/MagicBitBoardShared.cpp
)
target_include_directories(MagicBitBoardGen PUBLIC include)
target_link_libraries(MagicBitBoardGen PRIVATE engine_lib)

# === MagicBitBoardGenerator exe ===
add_executable(GenerateMagicNumbers src/MagicBitboards/mbbMain.cpp)
target_include_directories(GenerateMagicNumbers PRIVATE include)
target_link_libraries(GenerateMagicNumbers PRIVATE MagicBitBoardGen)

# === Standalone Engine exe ===
add_executable(StandaloneEngine src/CLIEngine/CLIEngine.cpp)
target_include_directories(StandaloneEngine PRIVATE include)
target_link_libraries(StandaloneEngine PRIVATE engine_lib)

# === FORCE REBUILD OF STANDALONE ENGINE ===
add_dependencies(Chess StandaloneEngine)

# === INCLUDE TESTS ===
add_subdirectory(tests)

# === THIS STORES THE RESOURCE DIRECTORY TO BE REFERRED TO IN CODE ===
set(RESOURCE_DIR_VAR "${CMAKE_CURRENT_SOURCE_DIR}/resources")
add_compile_definitions(RESOURCE_DIR=\"${RESOURCE_DIR_VAR}\")

