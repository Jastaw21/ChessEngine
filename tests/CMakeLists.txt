# tests/CMakeLists.txt

# Define the test executable
add_executable(chess_tests
        bitboardTests.cpp
        FenTests.cpp
        boardManagerTests.cpp
        humanPlayerTests.cpp
        searchTreeTests.cpp
        engineTests.cpp
        rulesTests.cpp
        mbbTests.cpp
        # Add more test files here
)

# Link against core chess logic and Google Test
target_link_libraries(chess_tests
        PRIVATE
        chess_lib
        gtest_main
)

# Enable test discovery
include(googletest)
gtest_discover_tests(chess_tests)
