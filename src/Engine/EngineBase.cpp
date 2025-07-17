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

