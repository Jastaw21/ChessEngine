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
        R"(C:\Users\jacks\source\repos\Chess\.venv\Scripts\python.exe C:\Users\jacks\source\repos\Chess\tests\Python\moveGetterSF.py)";

inline const std::string moveCmd =
        R"(C:\Users\jacks\source\repos\Chess\.venv\Scripts\python.exe C:\Users\jacks\source\repos\Chess\tests\Python\getMovesFromPosition.py)";

inline std::unordered_map<int, std::string> insets = {{1, "1----"}, {2, "2---"}, {3, "3--"}, {4, "4-"}, {5, "5"}};

std::vector<PerftResults> getDivideResults(const std::string& filename){
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
        int checks;
        int checkMate;
        std::istringstream iss(line);
        iss >> rootMoveUCI >> nodes >> captures >> enPassant >> castling >> checks >> checkMate;
        results.push_back(PerftResults{nodes, captures, enPassant, castling, checks, checkMate, rootMoveUCI});
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

    //std::cout << "Executing command: " << finalCommand << std::endl;

    [[maybe_unused]] const int result = system(finalCommand.c_str());

    return result == 0;
}

inline bool runDivideTest(const std::string& fen, const std::string& outputFile){
    const std::string file = fileRoot + outputFile;

    const std::string finalCommand =
            divideCmd + " \"" + fen + "\" \"" + file + "\"";

    //std::cout << "Executing command: " << finalCommand << std::endl;

    [[maybe_unused]] const int result = system(finalCommand.c_str());

    return result == 0;
}

inline bool runGetMoveResults(const std::string& fen, const std::string& outputFile){
    const std::string file = fileRoot + outputFile;
    const std::string finalCommand =
            moveCmd + " \"" + fen + "\" \"" + file + "\"";
    //std::cout << "Executing command: " << finalCommand << std::endl;

    [[maybe_unused]] const int result = system(finalCommand.c_str());

    return result == 0;
}


bool divideTest(const std::string& desiredFen, const std::string& outputFile, const int depth,
                const Colours& colourToMove = WHITE){
    if (depth == 0)
        return true;
    bool passing = true;
    const std::string fenAppendage = colourToMove == WHITE ? " w KQkq -" : " b KQkq -";
    const std::string fen = desiredFen + fenAppendage;

    generateExternalEngineMoves(fen, outputFile, depth);
    auto pyMoves = getDivideResults(outputFile); // get our own starting moves
    auto manager = BoardManager();
    manager.setCurrentTurn(colourToMove);
    manager.getBitboards()->loadFEN(desiredFen);
    TestEngine whiteEngine(WHITE);

    std::vector<PerftResults> engineResults = whiteEngine.runDivideTest(manager, depth);

    PerftResults totalPy;
    for (const auto& move: pyMoves) { totalPy += move; }
    PerftResults totalEngine;
    for (const auto& move: engineResults) { totalEngine += move; }

    std::string printInset = insets[depth];

    // Print headers
    std::cout << printInset << "Type\tNodes\tCaptures\tCastles\tEP\tChecks\tCheckMates" << std::endl;

    // Print Python results
    std::cout << printInset << "Python\t" << totalPy.nodes << "\t" << totalPy.captures << "\t"
            << totalPy.castling << "\t" << totalPy.enPassant << "\t" << totalPy.checks << "\t" << totalPy.checkMate <<
            std::endl;

    // Print Engine results
    std::cout << printInset << "Engine\t" << totalEngine.nodes << "\t" << totalEngine.captures << "\t"
            << totalEngine.castling << "\t" << totalEngine.enPassant << "\t" << totalEngine.checks << "\t" <<
            totalEngine.checkMate << std::endl;

    // gather errors in both directions for printing
    std::vector<PerftResults> inEngineNotInPy;
    for (const auto& ourMove: engineResults) {
        auto matching = std::ranges::find_if(pyMoves, [&](const auto& move) { return move.fen == ourMove.fen; });
        if (matching == pyMoves.end())
            passing = false;
        if (*matching != ourMove)
            passing = false;
        if (ourMove != *matching) { inEngineNotInPy.push_back(ourMove); }
    }
    std::vector<PerftResults> inPyNotInEngine;
    for (const auto& pyMove: pyMoves) {
        auto matching = std::ranges::find_if(engineResults, [&](const auto& move) { return move.fen == pyMove.fen; });
        if (matching == engineResults.end())
            passing = false;
        if (*matching != pyMove)
            passing = false;
        if (pyMove != *matching) { inPyNotInEngine.push_back(pyMove); }
    }

    if (!inEngineNotInPy.empty()) { std::cout << printInset << "Our Engine Believes: " << std::endl; }
    for (const auto& move: inEngineNotInPy) {
        auto pyResult = std::ranges::find_if(pyMoves, [&](const auto& pymove) { return pymove.fen == move.fen; });

        std::cout << printInset << move.fen << std::endl << printInset << "Py: " << pyResult->toString() << std::endl <<
                printInset << "En: " <<
                move.toString() <<
                std::endl;

        // divide further
        std::cout << printInset << "============Dividing further=========== " << move.fen << " " << std::endl;
        int iRank, iFile;
        Fen::FenToRankAndFile(move.fen, iRank, iFile);
        auto inferredPiece = manager.getBitboards()->getPiece(iRank, iFile);
        if (inferredPiece.has_value()) {
            auto subMove = createMove(inferredPiece.value(), move.fen);
            manager.tryMove(subMove);
            divideTest(manager.getBitboards()->toFEN(), outputFile, depth - 1, colourToMove == WHITE ? BLACK : WHITE);
        } else { std::cout << printInset << "No inferred piece found for " << move.fen << std::endl; }
    }

    if (!inPyNotInEngine.empty()) { std::cout << printInset << "External Engine Believes: " << std::endl; }
    for (const auto& move: inPyNotInEngine) {
        auto engineResult = std::ranges::find_if(engineResults, [&](const auto& engineMove) {
            return engineMove.fen == move.fen;
        });

        std::cout << printInset << move.fen << std::endl << printInset << "Py: " << move.toString() << std::endl <<
                printInset << "En: " <<
                engineResult->toString() <<
                std::endl;

        // divide further
        std::cout << printInset << "============Dividing further=========== " << move.fen << " " << std::endl;
        int iRank, iFile;
        Fen::FenToRankAndFile(move.fen, iRank, iFile);
        auto inferredPiece = manager.getBitboards()->getPiece(iRank, iFile);
        if (inferredPiece.has_value()) {
            auto subMove = createMove(inferredPiece.value(), move.fen);
            manager.tryMove(subMove);
            divideTest(manager.getBitboards()->toFEN(), outputFile, depth - 1, colourToMove == WHITE ? BLACK : WHITE);
        } else { std::cout << printInset << "No inferred piece found for " << move.fen << std::endl; }
    }

    std::cout << printInset << "==========================================" << std::endl << std::endl << std::endl <<
            std::endl << std::endl;
    return passing;
}

bool compareMoveList(const std::string& startingFen, const Colours& colourtoMove, const std::string& outputFile){
    bool passing = true;

    auto manager = BoardManager(colourtoMove);
    manager.getBitboards()->loadFEN(startingFen);
    auto engine = TestEngine(WHITE, &manager);

    auto ourMoves = engine.generateMoveList(manager);

    auto fenAppendage = colourtoMove == WHITE ? " w KQkq -" : " b KQkq -";
    generateExternalEngineMoves(manager.getBitboards()->toFEN() + fenAppendage, outputFile, 1);
    auto pyMoves = readMoves(outputFile);

    std::vector<std::string> ourMovesToTest;
    for (auto& move: ourMoves) { ourMovesToTest.push_back(move.toUCI()); }

    // in python, not in our engine
    std::vector<std::string> inEngineNotInOurs;
    for (const auto& move: pyMoves) {
        if (!std::ranges::any_of(ourMovesToTest, [&](const auto& ourMove) { return ourMove == move; }))
            inEngineNotInOurs.push_back(move);
    }

    std::vector<std::string> inOursNotInEngine;
    for (const auto& move: ourMovesToTest) {
        if (!std::ranges::any_of(pyMoves, [&](const auto& pyMove) { return pyMove == move; }))
            inOursNotInEngine.push_back(move);
    }

    if (!inEngineNotInOurs.empty()) {
        std::cout << "In external engine, not in ours:" << std::endl;
        for (const auto& move: inEngineNotInOurs) { std::cout << move << std::endl; }
    }

    if (!inOursNotInEngine.empty()) {
        std::cout << "In our engine, not in external engine:" << std::endl;
        for (const auto& move: inOursNotInEngine) { std::cout << move << std::endl; }
    }

    return inEngineNotInOurs.empty() && inOursNotInEngine.empty();
}

// USE THIS FOR COPYING
TEST(Divide, kiwiPeteDivide){
    constexpr int depth = 1;
    // get the moves the actual engine think are possible
    const std::string outputFile = "startPos.txt";
    EXPECT_TRUE(divideTest(Fen::KIWI_PETE_FEN, outputFile, depth));
}

TEST(Divide, kiwiPeteDivide2){
    constexpr int depth = 2;
    // get the moves the actual engine think are possible
    const std::string outputFile = "startPos.txt";
    EXPECT_TRUE(divideTest(Fen::KIWI_PETE_FEN, outputFile, depth));
}

TEST(Divide, kiwiPeteDivide3){
    constexpr int depth = 3;
    // get the moves the actual engine think are possible
    const std::string outputFile = "startPos.txt";
    EXPECT_TRUE(divideTest(Fen::KIWI_PETE_FEN, outputFile, depth));
}


TEST(Divide, perft1Divide){
    const int depth = 1;
    // get the moves the actual engine think are possible
    const std::string outputFile = "startPos.txt";
    EXPECT_TRUE(divideTest(Fen::STARTING_FEN, outputFile, depth));
}

TEST(Divide, perft2Divide){
    constexpr int depth = 2;
    // get the moves the actual engine think are possible
    const std::string outputFile = "startPos.txt";
    EXPECT_TRUE(divideTest(Fen::STARTING_FEN, outputFile, depth));
}

TEST(Divide, perft4Divide){
    constexpr int depth = 4;
    // get the moves the actual engine think are possible
    const std::string outputFile = "startPos.txt";
    EXPECT_TRUE(divideTest(Fen::STARTING_FEN, outputFile, depth));
}

TEST(Divide, position3Divide){
    constexpr int depth = 1;
    // get the moves the actual engine think are possible
    const std::string outputFile = "pos3.txt";
    EXPECT_TRUE(divideTest(Fen::POSITION_3_FEN, outputFile, depth));
}

TEST(Divide, position3Divide2){
    constexpr int depth = 2;
    // get the moves the actual engine think are possible
    const std::string outputFile = "pos3.txt";
    EXPECT_TRUE(divideTest(Fen::POSITION_3_FEN, outputFile, depth));
}


TEST(Perft, perft1){
    const TestEngine blackEngine(BLACK);
    auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 1);
    EXPECT_EQ(blackResultDepth1.nodes, 20);
    EXPECT_EQ(blackResultDepth1.captures, 0);

    const TestEngine whiteEngine(WHITE);
    auto whiteResultDepth1 = whiteEngine.runPerftTest(Fen::STARTING_FEN, 1);
    EXPECT_EQ(whiteResultDepth1.nodes, 20);
    EXPECT_EQ(whiteResultDepth1.captures, 0);
}

TEST(Perft, perft2){
    const TestEngine blackEngine(BLACK);

    auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 2);
    EXPECT_EQ(blackResultDepth1.nodes, 400);
    EXPECT_EQ(blackResultDepth1.captures, 0);

    const TestEngine whiteEngine(WHITE);
    auto whiteResultDepth1 = whiteEngine.runPerftTest(Fen::STARTING_FEN, 2);
    EXPECT_EQ(whiteResultDepth1.nodes, 400);
    EXPECT_EQ(whiteResultDepth1.captures, 0);
}

TEST(Perft, perft3){
    const TestEngine blackEngine(BLACK);

    auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 3);
    EXPECT_EQ(blackResultDepth1.nodes, 8902);
    EXPECT_EQ(blackResultDepth1.captures, 34);
    EXPECT_EQ(blackResultDepth1.enPassant, 0);
    EXPECT_EQ(blackResultDepth1.checks, 12);

    const TestEngine whiteEngine(WHITE);
    auto whiteResultDepth1 = whiteEngine.runPerftTest(Fen::STARTING_FEN, 3);
    EXPECT_EQ(whiteResultDepth1.nodes, 8902);
    EXPECT_EQ(whiteResultDepth1.captures, 34);
    EXPECT_EQ(whiteResultDepth1.enPassant, 0);
    EXPECT_EQ(whiteResultDepth1.checks, 12);
}

TEST(Perft, perft4){
    TestEngine blackEngine(BLACK);

    auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 4);
    EXPECT_EQ(blackResultDepth1.nodes, 197281);
    EXPECT_EQ(blackResultDepth1.captures, 1576);
    EXPECT_EQ(blackResultDepth1.enPassant, 0);
    EXPECT_EQ(blackResultDepth1.castling, 0);
    EXPECT_EQ(blackResultDepth1.checks, 469);
    EXPECT_EQ(blackResultDepth1.checkMate, 8);
}

TEST(Perft, kiwiPete1){
    const TestEngine blackEngine(WHITE);
    auto blackResultDepth1 = blackEngine.runPerftTest(Fen::KIWI_PETE_FEN, 1);
    EXPECT_EQ(blackResultDepth1.nodes, 48);
    EXPECT_EQ(blackResultDepth1.captures, 8);
    EXPECT_EQ(blackResultDepth1.castling, 2);
}

TEST(Perft, kiwiPete2){
    const TestEngine whiteEngine(WHITE);
    auto perftResults = whiteEngine.runPerftTest(Fen::KIWI_PETE_FEN, 2);
    EXPECT_EQ(perftResults.nodes, 2039);
    EXPECT_EQ(perftResults.captures, 351);
    EXPECT_EQ(perftResults.castling, 91);
    EXPECT_EQ(perftResults.enPassant, 1);
    EXPECT_EQ(perftResults.checks, 3);
}

TEST(Perft, kiwiPete3){
    const TestEngine whiteEngine(WHITE);
    auto perftResults = whiteEngine.runPerftTest(Fen::KIWI_PETE_FEN, 3);
    EXPECT_EQ(perftResults.nodes, 97862);
    EXPECT_EQ(perftResults.captures, 17102);
    EXPECT_EQ(perftResults.castling, 3162);
    EXPECT_EQ(perftResults.enPassant, 45);
    EXPECT_EQ(perftResults.checks, 993);
    EXPECT_EQ(perftResults.checkMate, 1);
}


TEST(Perft, position3Depth1){
    const TestEngine engine(WHITE);
    auto perftResults = engine.runPerftTest(Fen::POSITION_3_FEN, 1);
    EXPECT_EQ(perftResults.nodes, 14);
    EXPECT_EQ(perftResults.captures, 1);
    EXPECT_EQ(perftResults.checks, 2);
}

TEST(Perft, position3Depth2){
    const TestEngine engine(WHITE);
    auto perftResults = engine.runPerftTest(Fen::POSITION_3_FEN, 2);
    EXPECT_EQ(perftResults.nodes, 191);
    EXPECT_EQ(perftResults.captures, 14);
    EXPECT_EQ(perftResults.checks, 10);
}


TEST(Engine, EngineEvaluatesCorrectly){
    auto whiteEngine = TestEngine(WHITE);

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

    auto blackEngine = TestEngine(BLACK);
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

TEST(PerftCorrections, KPErrorsFixed){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::KIWI_PETE_FEN);
    auto engine = TestEngine(WHITE, &manager);

    auto e1gi = createMove(WN, "e5f7");
    ASSERT_TRUE(manager.tryMove(e1gi));

    generateExternalEngineMoves(manager.getBitboards()->toFEN() + " b KQkq -", "kiwipete.txt", 2);
    auto pyMoves = readMoves("kiwipete.txt");
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


TEST(PerftCorrections, pos3ErrorsFixed){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::POSITION_3_FEN);
    auto engine = TestEngine(WHITE, &manager);

    auto a5a6 = createMove(WK, "a5a6");
    ASSERT_TRUE(manager.tryMove(a5a6));

    generateExternalEngineMoves(manager.getBitboards()->toFEN() + " b KQkq -", "pos3.txt", 1);
    auto pyMoves = readMoves("pos3.txt");
    EXPECT_EQ(pyMoves.size(), 15);

    auto ourMoves = engine.generateMoveList(manager);
    EXPECT_EQ(ourMoves.size(), 15);

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

            if (childMove.resultBits & PUSH)
                moveResult = "push";
            if (childMove.resultBits & CAPTURE) {
                moveResult = "capture";
                totalCaptures++;
            }
            if (childMove.resultBits & CASTLING)
                moveResult = "castling";
            if (childMove.resultBits & EN_PASSANT)
                moveResult = "en_passant";
            if (childMove.resultBits & CHECK)
                moveResult += "+check";

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

TEST(depr, kiwi3CapturesCorrect){
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

            if (childMove.resultBits & PUSH)
                moveResult = "push";
            if (childMove.resultBits & CAPTURE) {
                moveResult = "capture";
                totalCaptures++;
            }
            if (childMove.resultBits & CASTLING)
                moveResult = "castling";
            if (childMove.resultBits & EN_PASSANT)
                moveResult = "en_passant";
            if (childMove.resultBits & CHECK) {
                moveResult += "+check";
                std::cout << "CHECK" << std::endl;
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

TEST(Divide, Kiwi2FixedCaptures){
    auto manager = BoardManager();
    auto engine = TestEngine(WHITE, &manager);
    manager.getBitboards()->loadFEN(Fen::KIWI_PETE_FEN);
    auto firstMove = createMove(WP, "a2a4");
    ASSERT_TRUE(manager.tryMove(firstMove));
    ASSERT_EQ(manager.getCurrentTurn(), BLACK);

    EXPECT_TRUE(divideTest(manager.getBitboards()->toFEN(),"newTest.txt",1,BLACK));
}

TEST(Divide, kiwiPete3Dividing){
    auto manager = BoardManager();
    auto engine = TestEngine(WHITE, &manager);
    manager.getBitboards()->loadFEN(Fen::KIWI_PETE_FEN);
    auto firstMove = createMove(WN, "e5f7");
    ASSERT_TRUE(manager.tryMove(firstMove));

    auto secondMove = createMove(BP, "h3g2");
    ASSERT_TRUE(manager.tryMove(secondMove));

    EXPECT_TRUE(compareMoveList(manager.getBitboards()->toFEN(), WHITE, "kiwipetedivide.txt"));
}

TEST(Divide, kiwipiete3Afterf3f6){
    auto manager = BoardManager();
    auto engine = TestEngine(WHITE, &manager);
    manager.getBitboards()->loadFEN(Fen::KIWI_PETE_FEN);
    auto firstMove = createMove(WQ, "f3f6");
    ASSERT_TRUE(manager.tryMove(firstMove));

    int depth = 3;
    while (depth > 0) {
        auto moves = engine.generateMoveList(manager);
        for (auto& move: moves) {
            std::cout << std::to_string(depth) << " " << move.toUCI() << std::endl;
            manager.tryMove(move);
            EXPECT_TRUE(compareMoveList(manager.getBitboards()->toFEN(), WHITE, "kiwipetedivide.txt"));
            manager.undoMove();
        }
        depth--;
    }
}

