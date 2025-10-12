//
// Created by jacks on 17/07/2025.
//

#include "../../include/Engine/CommandHandlerBase.h"
#include "Engine/EngineBase.h"

void CommandHandlerBase::operator()(const UCICommand& cmd, EngineBase* engine){
    std::cout << "uciok" << std::endl;
    std::cout << "id " << engine->engineID() << std::endl;
}

void CommandHandlerBase::operator()(const StopCommand& cmd, EngineBase* engine){ engine->stop(); }

void CommandHandlerBase::operator()(const IsReadyCommand& cmd, EngineBase* engine){
    std::cout << "readyok" << std::endl;
}

void CommandHandlerBase::operator()(const QuitCommand& cmd, EngineBase* engine){
    // space handler
    engine->quit();
}

void CommandHandlerBase::operator()(const GoCommand& cmd, EngineBase* engine){
    const auto depth = cmd.depth.value_or(engine->getSearchDepth());
    const auto hasTimeCommands = cmd.wtime.has_value() && cmd.btime.has_value();

    if (hasTimeCommands) { engine->go(depth, cmd.wtime.value(), cmd.btime.value()); } else { engine->go(depth); }
}

void CommandHandlerBase::operator()(const PositionCommand& cmd, EngineBase* engine){
    engine->reset();
    engine->boardManager()->setFullFen(cmd.fen);
    for (auto& move: cmd.moves) {
        if (const bool result = engine->boardManager()->tryMove(move); !result) {
            engine->logError("Invalid move: " + move + "\n");
        }
    }
}

void CommandHandlerBase::operator()(const BestMoveCommand& cmd, EngineBase* engine){}

void CommandHandlerBase::operator()(const NewGameCommand& cmd, EngineBase* engine){ engine->reset(); }

void CommandHandlerBase::operator()(const IDCommand& cmd, EngineBase* engine){ return; }

void CommandHandlerBase::operator()(const SetIDCommand& cmd, EngineBase* engine){
    // spacing comment for breakpoint
    engine->setEngineID(cmd.id);
}