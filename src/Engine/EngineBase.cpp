//
// Created by jacks on 16/06/2025.
//

#include "Engine/EngineBase.h"
#include "EngineShared/PerftResults.h"


EngineBase::EngineBase(const Colours colour): ChessPlayer(colour, ENGINE){}


void EngineBase::parseUCI(const std::string& uci){
    auto command = parser.parse(uci);
    // Create a visitor lambda that captures 'this' and forwards to command handler
    auto visitor = [this](const auto& cmd) { this->commandHandler(cmd, this); };
    std::visit(visitor, *command);
}

void EngineBase::makeReady(){}

PerftResults EngineBase::runPerftTest(const std::string& Fen, int depth){
    auto mgr = BoardManager(colour);
    mgr.getBitboards()->loadFEN(Fen);
    return perft(depth, mgr);
}

std::vector<PerftResults> EngineBase::runDivideTest(const std::string& Fen, int depth){
    auto mgr = BoardManager(colour);
    mgr.getBitboards()->loadFEN(Fen);
    return perftDivide(depth, mgr);
}

std::vector<PerftResults> EngineBase::runDivideTest(BoardManager& mgr, int depth){ return perftDivide(depth, mgr); }
std::vector<Move> EngineBase::generateMoveList(BoardManager& mgr){ return std::vector<Move>(); }

PerftResults EngineBase::perft(int depth, BoardManager& boardManager){
    if (depth == 0) return PerftResults{1, 0, 0, 0, 0, 0};

    PerftResults result{0, 0, 0, 0, 0, 0};
    auto moves = generateMoveList(boardManager);

    for (auto& move: moves) {
        boardManager.forceMove(move); // moves should be checked for legality already at this point so don't even worry
        PerftResults child = perft(depth - 1, boardManager);

        if (depth == 1) {
            result.nodes++;
            // Only increment move result types at leaf depth
            if (move.resultBits & CHECK) { result.checks++; }
            if (move.resultBits & EN_PASSANT) { result.enPassant++; }
            if (move.resultBits & CAPTURE) { result.captures++; }
            if (move.resultBits & CASTLING) { result.castling++; }
            if (move.resultBits & CHECK_MATE) { result.checkMate++; }
        } else { result += child; }

        boardManager.undoMove();
    }
    return result;
}

int EngineBase::simplePerft(int depth, BoardManager& boardManager){
    if (depth == 0)
        return 1;

    int nodes = 0;
    auto moves = generateMoveList(boardManager);
    for (Move& move: moves) {
        boardManager.forceMove(move);
        nodes += simplePerft(depth - 1, boardManager);
        boardManager.undoMove();
    }

    return nodes;
}

std::vector<PerftResults> EngineBase::perftDivide(int depth, BoardManager& boardManager){
    auto moves = generateMoveList(boardManager);
    std::vector<PerftResults> results;

    for (auto& move: moves) {
        auto result = PerftResults();
        result.nodes = 0;
        result.fen = move.toUCI();
        boardManager.forceMove(move);
        result += perft(depth - 1, boardManager);
        boardManager.undoMove();

        results.push_back(result);
    }

    return results;
}

