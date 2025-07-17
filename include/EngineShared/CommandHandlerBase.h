//
// Created by jacks on 17/07/2025.
//

#ifndef COMMANDHANDLERBASE_H
#define COMMANDHANDLERBASE_H
#include "EngineShared/UCIParsing/UciParser.h"


class EngineBase;

class CommandHandlerBase {
public:

    virtual ~CommandHandlerBase() = default;

    virtual void operator()(const UCICommand& cmd, EngineBase* engine);
    virtual void operator()(const StopCommand& cmd, EngineBase* engine);
    void operator()(const IsReadyCommand& cmd, EngineBase* engine);
    virtual void operator()(const QuitCommand& cmd, EngineBase* engine);
    virtual void operator()(const GoCommand& cmd, EngineBase* engine);
    virtual void operator()(const PositionCommand& cmd, EngineBase* engine);
    virtual void operator()(const BestMoveCommand& cmd, EngineBase* engine);
};



#endif //COMMANDHANDLERBASE_H
