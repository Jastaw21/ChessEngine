//
// Created by jacks on 17/07/2025.
//

#include "../../include/Engine/CommandHandlerBase.h"
#include "Engine/ChessEngine.h"

void CommandHandlerBase::operator()(const UCICommand& cmd, ChessEngine* engine){
    std::cout << "uciok" << std::endl;
    std::cout << "id " << engine->engineID() << std::endl;
}

void CommandHandlerBase::operator()(const StopCommand& cmd, ChessEngine* engine){ engine->stop(); }

void CommandHandlerBase::operator()(const IsReadyCommand& cmd, ChessEngine* engine){
    std::cout << "readyok" << std::endl;
}

void CommandHandlerBase::operator()(const QuitCommand& cmd, ChessEngine* engine){
    // space handler
    engine->quit();
}

void CommandHandlerBase::operator()(const GoCommand& cmd, ChessEngine* engine){
    const auto depth = cmd.depth.value_or(engine->getSearchDepth());
    const auto hasTimeCommands = cmd.wtime.has_value() && cmd.btime.has_value();

    if (hasTimeCommands) { engine->go(depth, cmd.wtime.value(), cmd.btime.value()); } else { engine->go(depth); }
}

void CommandHandlerBase::operator()(const PositionCommand& cmd, ChessEngine* engine){
    engine->reset();
    engine->boardManager()->setFullFen(cmd.fen);
    for (auto& move: cmd.moves) {
        if (const bool result = engine->boardManager()->tryMove(move); !result) {
            engine->logError("Invalid move: " + move + "\n");
        }
    }
}

void CommandHandlerBase::operator()(const BestMoveCommand& cmd, ChessEngine* engine){}

void CommandHandlerBase::operator()(const NewGameCommand& cmd, ChessEngine* engine){ engine->reset(); }

void CommandHandlerBase::operator()(const IDCommand& cmd, ChessEngine* engine){ return; }

void CommandHandlerBase::operator()(const SetIDCommand& cmd, ChessEngine* engine){
    // spacing comment for breakpoint
    engine->setEngineID(cmd.id);
}