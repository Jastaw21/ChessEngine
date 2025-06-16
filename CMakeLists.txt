cmake_minimum_required(VERSION 3.31)
project(Chess)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL3/lib/cmake")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL3_image/lib/cmake/SDL3_image")


find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3)
find_package(SDL3_image REQUIRED CONFIG)

add_executable(Chess ${PROJECT_SOURCE_DIR}/src/main.cpp
        src/gui.cpp
        include/gui.h
        src/BoardGui.cpp
        include/BoardGui.h
        src/DrawableEntity.cpp
        include/DrawableEntity.h
        src/PieceGUI.cpp
        include/PieceGUI.h
        include/Fen.h
        src/Vec2D.cpp
        include/Vec2D.h)

include_directories(include)

target_link_libraries(Chess PRIVATE SDL3::SDL3)
target_link_libraries(Chess PRIVATE SDL3_image::SDL3_image)

