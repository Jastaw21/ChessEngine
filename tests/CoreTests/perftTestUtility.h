//
// Created by jacks on 14/07/2025.
//

#ifndef PERFT_TEST_UTILITY_H
#define PERFT_TEST_UTILITY_H

#include <algorithm>
#include <fstream>
#include <ranges>
#include "Engine/MainEngine.h"
#include "Utility/Fen.h"
#include "EngineShared/PerftResults.h"

inline const std::string fileRoot = R"(C:\Users\jacks\source\repos\Chess\tests\output\)";
inline const std::string divideCmd =
        R"(C:\Users\jacks\source\repos\Chess\.venv\Scripts\python.exe C:\Users\jacks\source\repos\Chess\tests\Python\ExternalEnginePerftDivide.py)";

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
        int nodes = 0;
        int captures = 0;
        int enPassant = 0;
        int castling = 0;
        int checks = 0;
        int checkMate = 0;
        int promotions = 0;
        std::istringstream iss(line);
        iss >> rootMoveUCI >> nodes >> captures >> enPassant >> castling >> checks >> checkMate >> promotions;
        results.push_back(
            PerftResults{nodes, captures, enPassant, castling, checks, checkMate, promotions, rootMoveUCI});
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

    std::cout << "Executing command: " << finalCommand << std::endl;

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


inline void printMoveResults(const PerftResults& totalPy, const PerftResults& totalEngine,
                             const std::string& printInset){
    std::cout << printInset << "Type\tNodes\tCaptures\tCastles\tEP\tChecks\tCheckMates\tPromotions" << std::endl;

    // Print Python results
    std::cout << printInset << "Python\t"
            << totalPy.nodes << "\t" << totalPy.captures << "\t"
            << totalPy.castling << "\t" << totalPy.enPassant << "\t"
            << totalPy.checks << "\t" << totalPy.checkMate << "\t"
            << totalPy.promotions << std::endl;

    // Print Engine results
    std::cout << printInset << "Engine\t"
            << totalEngine.nodes << "\t" << totalEngine.captures << "\t"
            << totalEngine.castling << "\t" << totalEngine.enPassant << "\t"
            << totalEngine.checks << "\t" << totalEngine.checkMate << "\t"
            << totalEngine.promotions << std::endl;
}

inline void printHeadLineResults(const int depth, const std::vector<PerftResults>& pyMoves,
                                 const std::vector<PerftResults>& engineResults, const std::string& printInset){
    PerftResults totalPy;
    for (const auto& move: pyMoves) { totalPy += move; }
    PerftResults totalEngine;
    for (const auto& move: engineResults) { totalEngine += move; }
    printMoveResults(totalPy, totalEngine, printInset);
}

inline void getMovesInOurEngineNotExternal(bool& passing, std::vector<PerftResults>& externalEngineMoves,
                                           const std::vector<PerftResults>& engineResults,
                                           std::vector<PerftResults>& inOurEngineNotExternal){
    // loop through each move in our engine result
    for (const auto& ourMove: engineResults) {
        auto correspondingMove = std::ranges::find_if(externalEngineMoves, [&](const auto& move) {
            return move.fen == ourMove.fen;
        });
        if (correspondingMove == externalEngineMoves.end()) {
            passing = false; // this move doesn't exist at all
        }
        if (ourMove != *correspondingMove) {
            // this move exists but doesn't have the same results
            passing = false;
            std::cout << "Move: " << ourMove.fen << std::endl << "Our: " << ourMove.toString() << std::endl <<
                    "Ext: " <<
                    correspondingMove->toString() << std::endl;
            inOurEngineNotExternal.push_back(ourMove);
        }
    }
}

inline void getMovesInExternalEngineNotOurs(bool& passing, const std::vector<PerftResults>& externalEngineMoves,
                                            std::vector<PerftResults>& engineResults,
                                            std::vector<PerftResults>& inExternalEngineNotOurs){
    for (const auto& externalEngineMove: externalEngineMoves) {
        auto matching = std::ranges::find_if(engineResults, [&](const auto& move) {
            return move.fen == externalEngineMove.fen;
        });
        if (matching == engineResults.end()) {
            passing = false; // this move doesn't exist at all
        }
        if (externalEngineMove != *matching) {
            // this move exists but doesn't have the same results
            passing = false;
            inExternalEngineNotOurs.push_back(externalEngineMove);
        }
    }
}

inline std::vector<PerftResults> getErrors(bool passing, std::vector<PerftResults> externalEngineMoves,
                                           std::vector<PerftResults> engineResults,
                                           std::vector<PerftResults>& inOurEngineNotExternal,
                                           std::vector<PerftResults>& inExternalEngineNotOurs){
    std::vector<PerftResults> erroringMoves;

    getMovesInOurEngineNotExternal(passing, externalEngineMoves, engineResults, inOurEngineNotExternal);
    getMovesInExternalEngineNotOurs(passing, externalEngineMoves, engineResults, inExternalEngineNotOurs);

    // now zip them into one vector
    erroringMoves.insert(erroringMoves.end(), inOurEngineNotExternal.begin(), inOurEngineNotExternal.end());

    for (const auto& move: inExternalEngineNotOurs) {
        if (std::ranges::any_of(erroringMoves, [&](const auto& errorMove) { return errorMove.fen != move.fen; })) {
            erroringMoves.push_back(move);
        }
    }

    return erroringMoves;
}

inline bool divideTest(const FenString& desiredFen, const std::string& outputFile, const int depth){
    if (depth == 0)
        return true;
    bool passing = true;

    // generate and read the external engine's moves
    generateExternalEngineMoves(desiredFen, outputFile, depth);
    auto externalEngineMoves = getDivideResults(outputFile);

    // get our own starting moves
    MainEngine whiteEngine;
    whiteEngine.setFullFen(desiredFen);
    std::vector<PerftResults> engineResults = whiteEngine.runDivideTest(depth);

    std::string printInset = insets[depth];
    printHeadLineResults(depth, externalEngineMoves, engineResults, printInset);

    std::vector<PerftResults> inOurEngineNotExternal;
    std::vector<PerftResults> inExternalEngineNotOurs;
    auto errorResults = getErrors(passing, externalEngineMoves, engineResults, inOurEngineNotExternal,
                                  inExternalEngineNotOurs);

    if (errorResults.empty()) { passing = true; } else { passing = false; }

    // we can divide further
    if (depth > 1) {
        for (const auto& errorMove: errorResults) {
            int iRank, iFile;
            Fen::FenToRankAndFile(errorMove.fen, iRank, iFile);
            auto pieceAtLocation = whiteEngine.boardManager()->getBitboards()->getPiece(iRank, iFile);
            auto engineReadableErrorMove = createMove(pieceAtLocation.value(), errorMove.fen);
            whiteEngine.boardManager()->tryMove(engineReadableErrorMove);
            std::cout << "Move: " << errorMove.fen << " Dividing Further" << std::endl;

            divideTest(whiteEngine.boardManager()->getFullFen(), outputFile, depth - 1);
            whiteEngine.boardManager()->undoMove();

            // temporary just do the first one
            break;
        }
    }

    // otherwise just get the moves
    else if (!passing) {
        runGetMoveResults(whiteEngine.boardManager()->getFullFen(), "movesOutput.txt");
        auto externalMoveResults = readMovesResults("movesOutput.txt");
        auto ourMoveResults = whiteEngine.generateMoveList();

        for (const auto& externalMoveResult: externalMoveResults) {
            auto correspondingMove = std::ranges::find_if(ourMoveResults, [&](const auto ourMove) {
                return ourMove.toUCI() == externalMoveResult.first;
            });

            if (std::to_string(correspondingMove->resultBits) != externalMoveResult.second) {
                passing = false;
                std::cout << "Move: " << std::endl << externalMoveResult.first << " Oudr: " << correspondingMove->
                        resultBits <<
                        " External: " << externalMoveResult.second << std::endl;
            }

            // temporary just do the first one
            break;
        }
    }

    return passing;
}

inline bool compareMoveList(const FenString& startingFen, const Colours& colourtoMove, const std::string& outputFile){
    auto engine = MainEngine();
    engine.setFullFen(startingFen);

    auto ourMoves = engine.generateMoveList();

    generateExternalEngineMoves(engine.boardManager()->getFullFen(), outputFile, 1);
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

#endif //PERFT_TEST_UTILITY_H