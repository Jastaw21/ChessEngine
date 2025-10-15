//
// Created by jacks on 17/07/2025.
//

#ifndef COMMANDHANDLERBASE_H
#define COMMANDHANDLERBASE_H
#include "UCIParsing/UciParser.h"


class ChessEngine;

class CommandHandlerBase {
public:

    virtual ~CommandHandlerBase() = default;

    virtual void operator()(const UCICommand& cmd, ChessEngine* engine);
    virtual void operator()(const StopCommand& cmd, ChessEngine* engine);
    void operator()(const IsReadyCommand& cmd, ChessEngine* engine);
    virtual void operator()(const QuitCommand& cmd, ChessEngine* engine);
    virtual void operator()(const GoCommand& cmd, ChessEngine* engine);
    virtual void operator()(const PositionCommand& cmd, ChessEngine* engine);
    virtual void operator()(const BestMoveCommand& cmd, ChessEngine* engine);
    virtual void operator()(const NewGameCommand& cmd, ChessEngine* engine);
    void operator()(const IDCommand& cmd, ChessEngine* engine);
    void operator()(const SetIDCommand& cmd, ChessEngine* engine);
};


#endif //COMMANDHANDLERBASE_H