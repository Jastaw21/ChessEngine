//
// Created by jacks on 17/07/2025.
//

#include "../../include/EngineShared/CommandHandlerBase.h"
#include "Engine/EngineBase.h"

void CommandHandlerBase::operator()(const UCICommand& cmd, EngineBase* engine){ std::cout << "uciok"; }
void CommandHandlerBase::operator()(const StopCommand& cmd, EngineBase* engine){ engine->stop(); }

void CommandHandlerBase::operator()(const IsReadyCommand& cmd, EngineBase* engine){
    engine->makeReady();
    if (engine->isReady()) { std::cout << "readyok" << std::endl; }
}

void CommandHandlerBase::operator()(const QuitCommand& cmd, EngineBase* engine){ engine->quit(); }

void CommandHandlerBase::operator()(const GoCommand& cmd, EngineBase* engine){
    if (cmd.depth.has_value()) { engine->go(cmd.depth.value()); } else { engine->go(2); }
}

void CommandHandlerBase::operator()(const PositionCommand& cmd, EngineBase* engine){
    engine->boardManager()->setFen(cmd.fen);
    for (auto& move: cmd.moves) { engine->boardManager()->tryMove(move); }
}

void CommandHandlerBase::operator()(const BestMoveCommand& cmd, EngineBase* engine){}
