
//
// Created by jacks on 26/06/2025.
//

#include <algorithm>
#include <fstream>
#include <ranges>
#include <unordered_set>
#include <gtest/gtest.h>
#include "Engine/TestEngine.h"
#include "Utility/Fen.h"

inline const std::string fileRoot = R"(C:\Users\jacks\source\repos\Chess\tests\output\)";
inline const std::string divideCmd =
        R"(C:\Users\jacks\source\repos\Chess\.venv\Scripts\python.exe C:\Users\jacks\source\repos\Chess\tests\moveGetterSF.py)";

inline const std::string moveCmd =
        R"(C:\Users\jacks\source\repos\Chess\.venv\Scripts\python.exe C:\Users\jacks\source\repos\Chess\tests\getMovesFromPosition.py)";


std::vector<PerftResults> getPerftDivideResults(const std::string& filename){
    const std::string filePath = fileRoot + filename;
    std::ifstream file(filePath);
    std::vector<PerftResults> results;
    std::string line;
    while (std::getline(file, line)) {
        std::string rootMoveUCI;
        int nodes;
        int captures;
        int enPassant;
        int castling;
        std::istringstream iss(line);
        iss >> rootMoveUCI >> nodes >> captures >> enPassant >> castling;
        results.push_back(PerftResults{nodes, captures, enPassant, castling, rootMoveUCI});
    }

    return results;
}

std::vector<std::string> readMoves(const std::string& filename){
    const std::string filePath = fileRoot + filename;
    std::ifstream file(filePath);
    std::vector<std::string> results;
    std::string line;
    while (std::getline(file, line)) {
        std::string rootMoveUCI;
        std::istringstream iss(line);
        iss >> rootMoveUCI;
        results.push_back(rootMoveUCI);
    }

    return results;
}


std::vector<std::pair<std::string, std::string> > readMovesResults(const std::string& filename){
    const std::string filePath = fileRoot + filename;
    std::ifstream file(filePath);
    std::vector<std::pair<std::string, std::string> > results;
    std::string line;
    while (std::getline(file, line)) {
        std::string rootMoveUCI;
        std::string result;
        std::istringstream iss(line);
        iss >> rootMoveUCI >> result;
        results.push_back({rootMoveUCI, result});
    }

    return results;
}

bool generateExternalEngineMoves(const std::string& fen, const std::string& outputFile, const int depth = 1){
    const std::string file = fileRoot + outputFile;

    const std::string finalCommand =
            divideCmd + " \"" + fen + "\" \"" + file + "\"" + " " + std::to_string(depth);

    std::cout << "Executing command: " << finalCommand << std::endl;

    [[maybe_unused]] const int result = system(finalCommand.c_str());

    return result == 0;
}

inline bool runDivideTest(const std::string& fen, const std::string& outputFile){
    const std::string file = fileRoot + outputFile;

    const std::string finalCommand =
            divideCmd + " \"" + fen + "\" \"" + file + "\"";

    std::cout << "Executing command: " << finalCommand << std::endl;

    [[maybe_unused]] const int result = system(finalCommand.c_str());

    return result == 0;
}

inline bool runGetMoveResults(const std::string& fen, const std::string& outputFile){
    const std::string file = fileRoot + outputFile;
    const std::string finalCommand =
            moveCmd + " \"" + fen + "\" \"" + file + "\"";
    std::cout << "Executing command: " << finalCommand << std::endl;

    [[maybe_unused]] const int result = system(finalCommand.c_str());

    return result == 0;
}

bool divideTest(const std::string& desiredFen, const std::string& outputFile, const int depth,
                const Colours& colourToMove = WHITE){
    bool passing = true;
    const std::string fenAppendage = colourToMove == WHITE ? " w KQkq -" : " b KQkq -";
    const std::string fen = desiredFen + fenAppendage;

    generateExternalEngineMoves(fen, outputFile, depth);
    const auto pyMoves = getPerftDivideResults(outputFile);
    PerftResults totalPy;
    for (const auto& move: pyMoves) { totalPy += move; }

    std::cout << "Total Py Nodes: " << totalPy.nodes << std::endl;
    std::cout << "Total Py Captures: " << totalPy.captures << std::endl;
    std::cout << "Total Py Castles: " << totalPy.castling << std::endl;
    std::cout << "Total Py EP: " << totalPy.enPassant << std::endl;

    // get our own starting moves
    auto manager = BoardManager();
    manager.setCurrentTurn(colourToMove);
    manager.getBitboards()->loadFEN(desiredFen);
    TestEngine whiteEngine(WHITE);

    std::vector<PerftResults> engineResults = whiteEngine.runDivideTest(manager, depth);

    std::vector<PerftResults> inPyNotInEngine;
    for (const auto& pyMove: pyMoves) {
        const auto matching = std::ranges::find_if(engineResults, [&](const auto& move) {
            return move.fen == pyMove.fen;
        });
        if (matching == engineResults.end())
            passing = false;
        if (*matching != pyMove)
            passing = false;
        if (pyMove != *matching)
            inPyNotInEngine.push_back(pyMove);
    }

    std::cout << "External Engine Believes: " << std::endl;
    for (const auto& move: inPyNotInEngine) {
        const auto engineResult = std::ranges::find_if(engineResults, [&](const auto& engineMove) {
            return engineMove.fen == move.fen;
        });

        std::cout << move.fen << std::endl << "Py: " << move.toString() << std::endl << "En: " <<
                engineResult->toString() <<
                std::endl;
    }

    return passing;
}

// USE THIS FOR COPYING
TEST(PerftDivide, kiwiPeteDivide){
    constexpr int depth = 1;
    // get the moves the actual engine think are possible
    const std::string outputFile = "startPos.txt";
    EXPECT_TRUE(divideTest(Fen::KIWI_PETE_FEN, outputFile, depth));
}

TEST(PerftDivide, kiwiPeteDivide2){
    constexpr int depth = 2;
    // get the moves the actual engine think are possible
    const std::string outputFile = "startPos.txt";
    EXPECT_TRUE(divideTest(Fen::KIWI_PETE_FEN, outputFile, depth));
}

TEST(PerftDivide, perft1Divide){
    int depth = 1;
    // get the moves the actual engine think are possible
    const std::string outputFile = "startPos.txt";
    EXPECT_TRUE(divideTest(Fen::STARTING_FEN, outputFile, depth));
}

TEST(PerftDivide, perft2Divide){
    constexpr int depth = 2;
    // get the moves the actual engine think are possible
    const std::string outputFile = "startPos.txt";
    EXPECT_TRUE(divideTest(Fen::STARTING_FEN, outputFile, depth));
}

TEST(PerftDivide, position3Divide){
    constexpr int depth = 1;
    // get the moves the actual engine think are possible
    const std::string outputFile = "pos3.txt";
    EXPECT_TRUE(divideTest(Fen::POSITION_3_FEN, outputFile, depth));
}

TEST(PerftDivide, position3Divide2){
    constexpr int depth = 2;
    // get the moves the actual engine think are possible
    const std::string outputFile = "pos3.txt";
    EXPECT_TRUE(divideTest(Fen::POSITION_3_FEN, outputFile, depth));
}


TEST(Perft, perft1){
    TestEngine blackEngine(BLACK);
    const auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 1);
    EXPECT_EQ(blackResultDepth1.nodes, 20);
    EXPECT_EQ(blackResultDepth1.captures, 0);

    TestEngine whiteEngine(WHITE);
    const auto whiteResultDepth1 = whiteEngine.runPerftTest(Fen::STARTING_FEN, 1);
    EXPECT_EQ(whiteResultDepth1.nodes, 20);
    EXPECT_EQ(whiteResultDepth1.captures, 0);
}

TEST(Perft, perft2){
    TestEngine blackEngine(BLACK);

    const auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 2);
    EXPECT_EQ(blackResultDepth1.nodes, 400);
    EXPECT_EQ(blackResultDepth1.captures, 0);

    TestEngine whiteEngine(WHITE);
    const auto whiteResultDepth1 = whiteEngine.runPerftTest(Fen::STARTING_FEN, 2);
    EXPECT_EQ(whiteResultDepth1.nodes, 400);
    EXPECT_EQ(whiteResultDepth1.captures, 0);
}

TEST(Perft, perft3){
    TestEngine blackEngine(BLACK);

    const auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 3);
    EXPECT_EQ(blackResultDepth1.nodes, 8902);
    EXPECT_EQ(blackResultDepth1.captures, 34);
    EXPECT_EQ(blackResultDepth1.enPassant, 0);

    TestEngine whiteEngine(WHITE);
    const auto whiteResultDepth1 = whiteEngine.runPerftTest(Fen::STARTING_FEN, 3);
    EXPECT_EQ(whiteResultDepth1.nodes, 8902);
    EXPECT_EQ(whiteResultDepth1.captures, 34);
    EXPECT_EQ(whiteResultDepth1.enPassant, 0);
}

TEST(depr, perft4){
    TestEngine blackEngine(BLACK);

    const auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 4);
    EXPECT_EQ(blackResultDepth1.nodes, 197281);
    EXPECT_EQ(blackResultDepth1.captures, 1576);
    EXPECT_EQ(blackResultDepth1.enPassant, 0);

    TestEngine whiteEngine(WHITE);
    const auto whiteResultDepth1 = whiteEngine.runPerftTest(Fen::STARTING_FEN, 4);
    EXPECT_EQ(whiteResultDepth1.nodes, 197281);
    EXPECT_EQ(whiteResultDepth1.captures, 1576);
    EXPECT_EQ(whiteResultDepth1.enPassant, 0);
}

TEST(Perft, kiwiPete1){
    TestEngine blackEngine(WHITE);
    const auto blackResultDepth1 = blackEngine.runPerftTest(Fen::KIWI_PETE_FEN, 1);
    EXPECT_EQ(blackResultDepth1.nodes, 48);
    EXPECT_EQ(blackResultDepth1.captures, 8);
    EXPECT_EQ(blackResultDepth1.castling, 2);
}

TEST(Perft, kiwiPete2){
    TestEngine whiteEngine(WHITE);
    const auto perftResults = whiteEngine.runPerftTest(Fen::KIWI_PETE_FEN, 2);
    EXPECT_EQ(perftResults.nodes, 2039);
    EXPECT_EQ(perftResults.captures, 351);
    EXPECT_EQ(perftResults.castling, 91);
    EXPECT_EQ(perftResults.enPassant, 1);
}

TEST(Perft, position3Depth1){
    TestEngine engine(WHITE);
    const auto perftResults = engine.runPerftTest(Fen::POSITION_3_FEN, 1);
    EXPECT_EQ(perftResults.nodes, 14);
    EXPECT_EQ(perftResults.captures, 1);
}

TEST(Perft, position3Depth2){
    TestEngine engine(WHITE);
    const auto perftResults = engine.runPerftTest(Fen::POSITION_3_FEN, 2);
    EXPECT_EQ(perftResults.nodes, 191);
    EXPECT_EQ(perftResults.captures, 14);
}


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
    manager.getBitboards()->loadFEN("8/8/8/2dq1p2/3R4/8/8/N7");
    auto engine = TestEngine(BLACK, &manager);

    auto firstWhiteMove = createMove(WN, "a1c2");
    ASSERT_TRUE(manager.tryMove(firstWhiteMove));

    EXPECT_EQ(engine.search(2).toUCI(), "c5d4");
}

TEST(Engine, GeneratedMovesIncludeCastling){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::KIWI_PETE_FEN);
    auto engine = TestEngine(WHITE, &manager);

    const auto moves = engine.generateMoveList(manager);
    std::vector<std::string> movesToTest;
    for (auto& move: moves) { movesToTest.push_back(move.toUCI()); }
    EXPECT_TRUE(std::ranges::any_of(movesToTest, [&](const auto& move) { return move == "e1g1"; }));
    EXPECT_TRUE(std::ranges::any_of(movesToTest, [&](const auto& move) { return move == "e1c1"; }));
}

TEST(PerftCorrections, KPErrorsFixed){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::KIWI_PETE_FEN);
    auto engine = TestEngine(WHITE, &manager);

    auto e1gi = createMove(WN, "e5f7");
    ASSERT_TRUE(manager.tryMove(e1gi));

    generateExternalEngineMoves(manager.getBitboards()->toFEN() + " b KQkq -", "kiwipete.txt", 1);
    const auto pyMoves = readMoves("kiwipete.txt");
    ASSERT_EQ(pyMoves.size(), 44);

    auto ourMoves = engine.generateMoveList(manager);
    EXPECT_EQ(ourMoves.size(), 44);

    std::vector<std::string> ourMovesToTest;
    for (auto& move: ourMoves) { ourMovesToTest.push_back(move.toUCI()); }

    // in python, not in our engine
    std::vector<std::string> inEngineNotInOurs;
    for (const auto& move: pyMoves) {
        EXPECT_TRUE(std::ranges::any_of(ourMovesToTest, [&](const auto& ourMove) { return ourMove == move; }));
        if (!std::ranges::any_of(ourMovesToTest, [&](const auto& ourMove) { return ourMove == move; }))
            inEngineNotInOurs.push_back(move);
    }

    std::vector<std::string> inOursNotInEngine;
    for (const auto& move: ourMovesToTest) {
        EXPECT_TRUE(std::ranges::any_of(pyMoves, [&](const auto& pyMove) { return pyMove == move; }));
        if (!std::ranges::any_of(pyMoves, [&](const auto& pyMove) { return pyMove == move; }))
            inOursNotInEngine.push_back(move);
    }

    std::cout << "In external engine, not in ours:" << std::endl;
    for (const auto& move: inEngineNotInOurs) { std::cout << move << std::endl; }

    std::cout << "In our engine, not in external engine:" << std::endl;
    for (const auto& move: inOursNotInEngine) { std::cout << move << std::endl; }
}

TEST(Engine, MoveGenerationDoesNotChangeState){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::KIWI_PETE_FEN);
    auto engine = TestEngine(WHITE, &manager);

    auto move = createMove(WB, "e2a6");
    ASSERT_TRUE(manager.tryMove(move));

    auto board = manager.getBitboards()->toFEN();
    auto moves = engine.generateMoveList(manager);
    auto newBoard = manager.getBitboards()->toFEN();

    ASSERT_EQ(board, newBoard);
}

TEST(depr, pos3CapturesCorrect){
    // get the moves the actual engine thinks are possible
    const std::string outputFile = "pos3.txt";
    const std::string fen = Fen::POSITION_3_FEN + " w KQkq -";

    // get our own starting moves
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::POSITION_3_FEN);
    TestEngine whiteEngine(WHITE);
    auto startingMoves = whiteEngine.generateMoveList(manager);

    int totalCaptures = 0;

    // iterate through each move, getting all it's child moves
    for (auto& move: startingMoves) {
        if (manager.getCurrentTurn() != WHITE) {
            std::cout << "Starting Move breaks turn changes: " << move.toUCI() << std::endl;
            continue;
        }
        manager.tryMove(move); // try it

        if (manager.getCurrentTurn() != BLACK) {
            std::cout << "Move breaks turn changes: " << move.toUCI() << std::endl;
        }
        // what does the external engine expect?
        runGetMoveResults(manager.getBitboards()->toFEN() + " b KQkq -", outputFile);
        auto pyMoves = readMovesResults(outputFile);

        // what do we get?
        std::vector<std::pair<std::string, std::string> > moveResults;
        auto childMoves = whiteEngine.generateMoveList(manager);
        for (auto& childMove: childMoves) {
            manager.checkMove(childMove);
            std::string moveResult;

            switch (childMove.result) {
                case PUSH:
                    moveResult = "push";
                    break;
                case CAPTURE:
                    moveResult = "capture";
                    totalCaptures++;
                    break;
                case CASTLING:
                    moveResult = "castling";
                    break;
                case EN_PASSANT:
                    moveResult = "en_passant";
                    break;
                default:
                    moveResult = "unknown";
            }

            moveResults.push_back({childMove.toUCI(), moveResult});
        }

        std::set<std::pair<std::string, std::string> > moveSet;
        for (const auto& subMove: moveResults) { moveSet.insert(subMove); }

        std::vector<std::pair<std::string, std::string> > engineDisagreesWithPy;
        for (const auto& pyMove: pyMoves) {
            const bool matching = moveSet.contains(pyMove);
            EXPECT_TRUE(matching) << "Move mismatch: " << pyMove.first;
            if (!matching) { engineDisagreesWithPy.push_back(pyMove); }
        }

        // log it
        std::cout << "Our Root Move: " << move.toUCI() << std::endl;
        for (const auto& pyMove: engineDisagreesWithPy) {
            auto engineResult = std::ranges::find_if(moveResults,
                                                     [&](const auto& otherMove) {
                                                         return otherMove.first == pyMove.first;
                                                     });

            if (engineResult != moveResults.end()) {
                std::cout << "Py: " << pyMove.first << " " << pyMove.second
                        << " Engine: " << engineResult->second << std::endl;
            } else {
                std::cout << "Py: " << pyMove.first << " " << pyMove.second
                        << " Engine: move not found" << std::endl;
            }
        }

        manager.undoMove();
        std::cout << totalCaptures << std::endl;
    }
    EXPECT_EQ(totalCaptures, 14);
}


TEST(depr, kiwi2CapturesCorrect){
    // get the moves the actual engine thinks are possible
    const std::string outputFile = "kiwipetedivide.txt";
    const std::string fen = Fen::KIWI_PETE_FEN + " w KQkq -";

    // get our own starting moves
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::KIWI_PETE_FEN);
    TestEngine whiteEngine(WHITE);
    auto startingMoves = whiteEngine.generateMoveList(manager);

    int totalCaptures = 0;

    // iterate through each move, getting all it's child moves
    for (auto& move: startingMoves) {
        if (manager.getCurrentTurn() != WHITE) {
            std::cout << "Starting Move breaks turn changes: " << move.toUCI() << std::endl;
            continue;
        }
        manager.tryMove(move); // try it

        if (manager.getCurrentTurn() != BLACK) {
            std::cout << "Move breaks turn changes: " << move.toUCI() << std::endl;
        }
        // what does the external engine expect?
        runGetMoveResults(manager.getBitboards()->toFEN() + " b KQkq -", outputFile);
        auto pyMoves = readMovesResults(outputFile);

        // what do we get?
        std::vector<std::pair<std::string, std::string> > moveResults;
        auto childMoves = whiteEngine.generateMoveList(manager);
        for (auto& childMove: childMoves) {
            manager.checkMove(childMove);
            std::string moveResult;

            switch (childMove.result) {
                case PUSH:
                    moveResult = "push";
                    break;
                case CAPTURE:
                    moveResult = "capture";
                    totalCaptures++;
                    break;
                case CASTLING:
                    moveResult = "castling";
                    break;
                case EN_PASSANT:
                    moveResult = "en_passant";
                    break;
                default:
                    moveResult = "unknown";
            }

            moveResults.push_back({childMove.toUCI(), moveResult});
        }

        std::set<std::pair<std::string, std::string> > moveSet;
        for (const auto& subMove: moveResults) { moveSet.insert(subMove); }

        std::vector<std::pair<std::string, std::string> > engineDisagreesWithPy;
        for (const auto& pyMove: pyMoves) {
            const bool matching = moveSet.contains(pyMove);
            EXPECT_TRUE(matching) << "Move mismatch: " << pyMove.first;
            if (!matching) { engineDisagreesWithPy.push_back(pyMove); }
        }

        // log it
        std::cout << "Our Root Move: " << move.toUCI() << std::endl;
        // log it
        std::cout << "Our Root Move: " << move.toUCI() << std::endl;
        for (const auto& pyMove: engineDisagreesWithPy) {
            auto engineResult = std::ranges::find_if(moveResults,
                                                     [&](const auto& otherMove) {
                                                         return otherMove.first == pyMove.first;
                                                     });

            if (engineResult != moveResults.end()) {
                std::cout << "Py: " << pyMove.first << " " << pyMove.second
                        << " Engine: " << engineResult->second << std::endl;
            } else {
                std::cout << "Py: " << pyMove.first << " " << pyMove.second
                        << " Engine: move not found" << std::endl;
            }
        }

        manager.undoMove();
        std::cout << totalCaptures << std::endl;
    }
    EXPECT_EQ(totalCaptures, 351);
}

TEST(PerftDivide, Kiwi2FixedCaptures){
    auto manager = BoardManager();
    auto engine = TestEngine(WHITE, &manager);
    manager.getBitboards()->loadFEN(Fen::KIWI_PETE_FEN);
    auto firstMove = createMove(WP, "a2a4");
    ASSERT_TRUE(manager.tryMove(firstMove));
    ASSERT_EQ(manager.getCurrentTurn(), BLACK);

    EXPECT_TRUE(divideTest(manager.getBitboards()->toFEN(),"newTest.txt",1,BLACK));
}