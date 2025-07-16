//
// Created by jacks on 14/07/2025.
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

inline std::vector<PerftResults> getDivideResults(const std::string& filename){
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

inline std::vector<std::string> readMoves(const std::string& filename){
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


inline std::vector<std::pair<std::string, std::string> > readMovesResults(const std::string& filename){
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

inline bool generateExternalEngineMoves(const std::string& fen, const std::string& outputFile, const int depth = 1){
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


inline bool divideTest(const std::string& desiredFen, const std::string& outputFile, const int depth,
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

inline bool compareMoveList(const std::string& startingFen, const Colours& colourtoMove, const std::string& outputFile){
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