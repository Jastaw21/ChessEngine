cmake_minimum_required(VERSION 3.31)
project(Chess)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL3/lib/cmake")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL3_image/lib/cmake/SDL3_image")


find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3)
find_package(SDL3_image REQUIRED CONFIG)



add_executable(Chess ${PROJECT_SOURCE_DIR}/src/main.cpp
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
        include/Engine/Piece.cpp
        include/Engine/Piece.h)

include_directories(include)

target_link_libraries(Chess PRIVATE SDL3::SDL3)
target_link_libraries(Chess PRIVATE SDL3_image::SDL3_image)


