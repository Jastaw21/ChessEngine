include(FetchContent)

# Fetch GoogleTest from GitHub
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# Prevent GTest from using dynamic CRT on Windows
set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

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
        CoreTests/UCIParserTests.cpp
        CoreTests/utilityTests.cpp
        CoreTests/ZobristTests.cpp
        CoreTests/EPDParser.h
        CoreTests/EPDTesting.cpp
        CoreTests/RefereeTests.cpp
)

set_target_properties(chess_tests PROPERTIES CXX_STANDARD 20)

target_link_libraries(chess_tests
        PRIVATE
        engine_lib
        gui_lib
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(chess_tests)
