//
// Created by jacks on 16/06/2025.
//

#include "Engine/EngineBase.h"

#include "EngineShared/CommunicatorBase.h"
#include "EngineShared/PerftResults.h"


EngineBase::EngineBase(): ChessPlayer(ENGINE){}

void EngineBase::go(const int depth){
    auto move = search(depth);
    communicator_->send("bestmove " + move.toUCI());
}

void EngineBase::makeReady(){}

void EngineBase::parseUCI(const std::string& uci){
    auto command = parser.parse(uci);
    // Create a visitor lambda that captures 'this' and forwards to command handler
    auto visitor = [this](const auto& cmd) { this->commandHandler(cmd, this); };
    std::visit(visitor, *command);
}

PerftResults EngineBase::runPerftTest(const std::string& Fen, const int depth){
    internalBoardManager_.getBitboards()->loadFEN(Fen);
    return perft(depth);
}

std::vector<PerftResults> EngineBase::runDivideTest(const std::string& Fen, const int depth){
    internalBoardManager_.getBitboards()->loadFEN(Fen);
    return perftDivide(depth);
}

std::vector<PerftResults> EngineBase::runDivideTest(const int depth){ return perftDivide(depth); }

std::vector<Move> EngineBase::generateMoveList(){ return std::vector<Move>(); }

void EngineBase::loadFEN(const std::string& fen){
    // std::string firstPartOfFen;
    // for (int i = 0; i < fen.size(); i++) {
    //     if (fen[i] == ' ') { break; }
    //     firstPartOfFen += fen[i];
    // }
    // internalBoardManager_.getBitboards()->loadFEN(firstPartOfFen);
    boardManager()->setFen(fen);
}

PerftResults EngineBase::perft(const int depth){
    if (depth == 0) return PerftResults{1, 0, 0, 0, 0, 0};

    PerftResults result{0, 0, 0, 0, 0, 0};
    auto moves = generateMoveList();

    for (auto& move: moves) {
        internalBoardManager_.forceMove(move);
        // moves should be checked for legality already at this point so don't even worry
        PerftResults child = perft(depth - 1);

        if (depth == 1) {
            result.nodes++;
            // Only increment move result types at leaf depth
            if (move.resultBits & CHECK) { result.checks++; }
            if (move.resultBits & EN_PASSANT) { result.enPassant++; }
            if (move.resultBits & CAPTURE) { result.captures++; }
            if (move.resultBits & CASTLING) { result.castling++; }
            if (move.resultBits & CHECK_MATE) { result.checkMate++; }
        } else { result += child; }

        internalBoardManager_.undoMove();
    }
    return result;
}

int EngineBase::simplePerft(const int depth){
    if (depth == 0)
        return 1;

    int nodes = 0;
    auto moves = generateMoveList();
    for (Move& move: moves) {
        internalBoardManager_.forceMove(move);
        nodes += simplePerft(depth - 1);
        internalBoardManager_.undoMove();
    }

    return nodes;
}

std::vector<PerftResults> EngineBase::perftDivide(const int depth){
    auto moves = generateMoveList();
    std::vector<PerftResults> results;

    for (auto& move: moves) {
        auto result = PerftResults();
        result.nodes = 0;
        result.fen = move.toUCI();
        internalBoardManager_.forceMove(move);
        result += perft(depth - 1);
        internalBoardManager_.undoMove();

        results.push_back(result);
    }

    return results;
}

