//
// Created by jacks on 19/07/2025.
//

#ifndef MANAGERCOMMANDHANDLER_H
#define MANAGERCOMMANDHANDLER_H
#include "EngineShared/UCIParsing/UciParser.h"


class MatchManager;


class ManagerCommandHandler final {
public:

    void operator()(const UCICommand& cmd, MatchManager* matchManager);
    void operator()(const StopCommand& cmd, MatchManager* matchManager);
    void operator()(const IsReadyCommand& cmd, MatchManager* matchManager);
    void operator()(const QuitCommand& cmd, MatchManager* matchManager);
    void operator()(const GoCommand& cmd, MatchManager* matchManager);
    void operator()(const PositionCommand& cmd, MatchManager* matchManager);
    static void generateFullPositionCommand(MatchManager* matchManager, std::string& fullPositionCommand);
    void operator()(const BestMoveCommand& cmd, MatchManager* matchManager);
    void operator()(const NewGameCommand& cmd, MatchManager* matchManager);
    void operator()(const IDCommand& cmd, MatchManager* matchManager);
};


#endif //MANAGERCOMMANDHANDLER_H