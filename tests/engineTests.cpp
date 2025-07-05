//
// Created by jacks on 26/06/2025.
//

#include <algorithm>
#include <fstream>
#include <ranges>
#include <gtest/gtest.h>
#include "Engine/TestEngine.h"
#include "Utility/Fen.h"

inline const std::string fileRoot = "C:\\Users\\jacks\\source\\repos\\Chess\\tests\\output\\";
inline const std::string rawCmd =
        "C:\\Users\\jacks\\source\\repos\\Chess\\.venv\\Scripts\\python.exe C:\\Users\\jacks\\source\\repos\\Chess\\tests\\moveGetterSF.py";

std::vector<std::string> readMoves(const std::string& filename){
    const std::string filePath = fileRoot + filename;
    std::ifstream file(filePath);
    std::vector<std::string> moves;
    std::string line;
    while (std::getline(file, line)) { moves.push_back(line); }
    return moves;
}

bool runPythonScript(const std::string& fen, const std::string& outputFile){
    const std::string file = fileRoot + outputFile;

    const std::string finalCommand =
            rawCmd + " \"" + fen + "\" \"" + file + "\"";

    std::cout << "Executing command: " << finalCommand << std::endl;

    [[maybe_unused]] const int result = system(finalCommand.c_str());

    return result == 0;
}

TEST(Perft, perft1){
    const TestEngine blackEngine(BLACK);
    const auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 1);
    EXPECT_EQ(blackResultDepth1.nodes, 20);
    EXPECT_EQ(blackResultDepth1.captures, 0);

    const TestEngine whiteEngine(WHITE);
    const auto whiteResultDepth1 = whiteEngine.runPerftTest(Fen::STARTING_FEN, 1);
    EXPECT_EQ(whiteResultDepth1.nodes, 20);
    EXPECT_EQ(whiteResultDepth1.captures, 0);
}

TEST(Perft, perft2){
    const TestEngine blackEngine(BLACK);

    const auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 2);
    EXPECT_EQ(blackResultDepth1.nodes, 400);
    EXPECT_EQ(blackResultDepth1.captures, 0);

    const TestEngine whiteEngine(WHITE);
    const auto whiteResultDepth1 = whiteEngine.runPerftTest(Fen::STARTING_FEN, 2);
    EXPECT_EQ(whiteResultDepth1.nodes, 400);
    EXPECT_EQ(whiteResultDepth1.captures, 0);
}

TEST(Perft, perft3){
    const TestEngine blackEngine(BLACK);

    const auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 3);
    EXPECT_EQ(blackResultDepth1.nodes, 8902);
    EXPECT_EQ(blackResultDepth1.captures, 34);
    EXPECT_EQ(blackResultDepth1.enPassant, 0);

    const TestEngine whiteEngine(WHITE);
    const auto whiteResultDepth1 = whiteEngine.runPerftTest(Fen::STARTING_FEN, 3);
    EXPECT_EQ(whiteResultDepth1.nodes, 8902);
    EXPECT_EQ(whiteResultDepth1.captures, 34);
    EXPECT_EQ(whiteResultDepth1.enPassant, 0);
}

TEST(Perft, kiwiPete1){
    const TestEngine blackEngine(WHITE);
    const auto blackResultDepth1 = blackEngine.runPerftTest(Fen::KIWI_PETE_FEN, 1);
    EXPECT_EQ(blackResultDepth1.nodes, 48);
    EXPECT_EQ(blackResultDepth1.captures, 8);
    EXPECT_EQ(blackResultDepth1.castling, 2);
}

TEST(Perft, kiwiPeteDivide){
    // run the python script

    // get the moves the actual engine think are possible
    const std::string outputFile = "kiwipete.txt";
    const std::string fen = Fen::KIWI_PETE_FEN + " w KQkq -";
    runPythonScript(fen, outputFile);
    const auto pyMoves = readMoves(outputFile);

    // get our own moves and convert them to UCI
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::KIWI_PETE_FEN);
    TestEngine blackEngine(WHITE);
    const auto moves = blackEngine.generateMoveList(manager);

    std::vector<std::string> movesToTest;
    for (auto& move: moves) { movesToTest.push_back(move.toUCI()); }

    std::vector<std::string> inPyNotInEngine;
    for (const auto& pyMove: pyMoves) {
        EXPECT_TRUE(std::ranges::any_of(movesToTest, [&](const auto& move){ return move == pyMove; }));
        if (!std::ranges::any_of(movesToTest, [&](const auto& move) { return move == pyMove; }))
            inPyNotInEngine.push_back(pyMove);
    }

    std::vector<std::string> inEngineNotInPy;
    for (const auto& engineMove: movesToTest) {
        EXPECT_TRUE(std::ranges::any_of(pyMoves, [&](const auto& move) { return move == engineMove; }));
        if (!std::ranges::any_of(pyMoves, [&](const auto& move) { return move == engineMove; }))
            inEngineNotInPy.push_back(engineMove);
    }

    std::cout << "Moves in engine but not in python: " << std::endl;
    for (const auto& move: inEngineNotInPy) { std::cout << move << std::endl; }
    std::cout << std::endl << "Moves in python but not in engine: " << std::endl;
    for (const auto& move: inPyNotInEngine) { std::cout << move << std::endl; }
}

// TEST(Perft, perft4){
//     TestEngine blackEngine(BLACK);
//
//     const auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 4);
//     EXPECT_EQ(blackResultDepth1.nodes, 197281);
//     EXPECT_EQ(blackResultDepth1.captures, 1576);
//     EXPECT_EQ(blackResultDepth1.enPassant, 0);
//
//     TestEngine whiteEngine(WHITE);
//     const auto whiteResultDepth1 = whiteEngine.runPerftTest(Fen::STARTING_FEN, 4);
//     EXPECT_EQ(whiteResultDepth1.nodes, 197281);
//     EXPECT_EQ(whiteResultDepth1.captures, 1576);
//     EXPECT_EQ(whiteResultDepth1.enPassant, 0);
// }

TEST(Engine, EngineEvaluatesCorrectly){
    const auto whiteEngine = TestEngine(WHITE);

    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::STARTING_FEN);

    // at start the engine should evaluate neutral
    EXPECT_EQ(whiteEngine.evaluate(manager), 0);

    // obvious white advantage should evaluate positive
    manager.getBitboards()->loadFEN("2p5/8/8/8/8/8/8/QQRBN3");
    EXPECT_GT(whiteEngine.evaluate(manager), 0);

    // obvious black advantage should evaluate negative
    manager.getBitboards()->loadFEN("rrn5/qr6/8/8/8/8/8/P7");
    EXPECT_LT(whiteEngine.evaluate(manager), 0);

    const auto blackEngine = TestEngine(BLACK);
    manager.getBitboards()->loadFEN(Fen::STARTING_FEN);
    EXPECT_EQ(blackEngine.evaluate(manager), 0);
    manager.getBitboards()->loadFEN("2p5/8/8/8/8/8/8/QQRBN3");
    EXPECT_LT(blackEngine.evaluate(manager), 0);
    manager.getBitboards()->loadFEN("rrn5/qr6/8/8/8/8/8/P7");
    EXPECT_GT(blackEngine.evaluate(manager), 0);
}

TEST(Engine, EnginePicksObviouslyBestMoves){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/1p1q1p2/3R4/8/2N5/8");
    auto engine = TestEngine(BLACK, &manager);

    auto firstWhiteMove = createMove(WN, "c2a3");
    ASSERT_TRUE(manager.tryMove(firstWhiteMove));

    EXPECT_EQ(engine.search().toUCI(), "d5d4");
}

TEST(Engine, EngineThinksAheadWhenPickingMoves){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/2pq1p2/3R4/8/8/N7");
    auto engine = TestEngine(BLACK, &manager);

    auto firstWhiteMove = createMove(WN, "a1c2");
    ASSERT_TRUE(manager.tryMove(firstWhiteMove));

    EXPECT_EQ(engine.search(2).toUCI(), "c5d4");
}

TEST(Engine, GeneratedMovesIncludeCastling){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::KIWI_PETE_FEN);
    auto engine = TestEngine(WHITE, &manager);

    auto moves = engine.generateMoveList(manager);
    std::vector<std::string> movesToTest;
    for (auto& move: moves) { movesToTest.push_back(move.toUCI()); }
    EXPECT_TRUE(std::ranges::any_of(movesToTest, [&](const auto& move) { return move == "e1g1"; }));
    EXPECT_TRUE(std::ranges::any_of(movesToTest, [&](const auto& move) { return move == "e1c1"; }));
}