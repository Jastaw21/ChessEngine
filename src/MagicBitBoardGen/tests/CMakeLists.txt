# tests/CMakeLists.txt

# Define the test executable
add_executable(mbGen mbbGenTests.cpp)

# Link against required libraries
target_link_libraries(mbGen
        PRIVATE
        chess_lib
        MagicBitBoardGen
        GTest::gtest
        GTest::gtest_main
)

# Enable test discovery
include(GoogleTest)
gtest_discover_tests(mbGen)
