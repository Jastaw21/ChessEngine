cmake_minimum_required(VERSION 3.30)
project(EngineShared)

set(CMAKE_CXX_STANDARD 20)


add_library(EngineSharedLib
        UCIParsing/UciParser.cpp
        ../../include/EngineShared/UCIParsing/UciParser.h
        ../../include/EngineShared/UCIParsing/Tokeniser.h
        UCIParsing/Tokeniser.cpp
        CommandHandlerBase.cpp
        ../../include/EngineShared/CommandHandlerBase.h
        CommunicatorBase.cpp
        ../../include/EngineShared/CommunicatorBase.h
)


target_link_libraries(EngineSharedLib chess_lib)