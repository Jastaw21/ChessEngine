# tests/CMakeLists.txt

# Define the test executable
add_executable(chess_tests
        CoreTests/bitboardTests.cpp
        CoreTests/FenTests.cpp
        CoreTests/boardManagerTests.cpp
        CoreTests/humanPlayerTests.cpp
        CoreTests/engineTests.cpp
        CoreTests/rulesTests.cpp
        MagicBitBoards/mbbTests.cpp
        CoreTests/perftTestUtility.h
        CoreTests/Perft.cpp
        UCIParserTests.cpp
        CoreTests/utilityTests.cpp
        # Add more test files here
)

# Link against core chess logic and Google Test
target_link_libraries(chess_tests
        PRIVATE
        chess_lib
        gtest_main
        EngineSharedLib
)

# Enable test discovery
include(googletest)

gtest_discover_tests(chess_tests)
