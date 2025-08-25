//
// Created by jacks on 19/07/2025.
//

// ReSharper disable CppMemberFunctionMayBeConst
#include "MatchManager/ManagerCommandHandler.h"
#include "MatchManager/MatchManager.h"
#include "Engine/EngineBase.h"
#include "EngineShared/CommunicatorBase.h"
#include "GUI/gui.h"


void ManagerCommandHandler::generateFullPositionCommand(MatchManager* matchManager, std::string& fullPositionCommand){
    std::vector<std::string> movesString;

    fullPositionCommand = "position " + matchManager->startingFen();

    auto& moveHistory = matchManager->getMoveHistory();

    if (!moveHistory.empty()) {
        fullPositionCommand += " moves ";

        std::vector<std::string> moves;
        auto tempStack = moveHistory;
        while (!tempStack.empty()) {
            moves.push_back(tempStack.top().toUCI());
            tempStack.pop();
        }

        for (auto it = moves.rbegin(); it != moves.rend(); ++it) { fullPositionCommand += *it + " "; }
    }
}

void ManagerCommandHandler::operator()(const UCICommand& cmd, MatchManager* matchManager){
    matchManager->getMessageQueueOutbound()->push("uci");
    //matchManager->swapPlayers();
}

void ManagerCommandHandler::operator()(const StopCommand& cmd, MatchManager* matchManager){
    matchManager->getMessageQueueOutbound()->push("stop");
    //matchManager->swapPlayers();
}

void ManagerCommandHandler::operator()(const IsReadyCommand& cmd, MatchManager* matchManager){
    matchManager->getMessageQueueOutbound()->push("isready");
    //matchManager->swapPlayers();
}

void ManagerCommandHandler::operator()(const QuitCommand& cmd, MatchManager* matchManager){
    matchManager->getMessageQueueOutbound()->push("quit");
    //matchManager->swapPlayers();
}

void ManagerCommandHandler::operator()(const GoCommand& cmd, MatchManager* matchManager){
    return;
    // not implemented
}

void ManagerCommandHandler::operator()(const PositionCommand& cmd, MatchManager* matchManager){
    std::string fullPositionCommand;
    if (cmd.isStartPos) { fullPositionCommand = "position startpos"; } else {
        fullPositionCommand = "position fen " + cmd.fen;
    }

    if (!cmd.moves.empty()) {
        fullPositionCommand += " moves ";
        for (auto& move: cmd.moves) {
            fullPositionCommand += move;
            fullPositionCommand += " ";
        }
    }
    matchManager->currentPlayer()->getCommunicator()->sendToEngine(fullPositionCommand);
    matchManager->swapPlayers();
}


void ManagerCommandHandler::operator()(const BestMoveCommand& cmd, MatchManager* matchManager){
    matchManager->addMove(cmd.move); // add our history

    // generate the command with move history from our internal state
    std::string fullPositionCommand;
    generateFullPositionCommand(matchManager, fullPositionCommand);

    if (matchManager->currentPlayer()->playerType != HUMAN) {
        matchManager->getGui()->receiveInfoOfEngineMove(matchManager->getMoveHistory().top());
    }

    // we've done receiving from the current player and need to swap to forward the command
    matchManager->swapPlayers();

    matchManager->currentPlayer()->getCommunicator()->sendToEngine(fullPositionCommand); // send to engine
    matchManager->currentPlayer()->getCommunicator()->sendToEngine("go");
}

void ManagerCommandHandler::operator()(const NewGameCommand& cmd, MatchManager* matchManager){}
void ManagerCommandHandler::operator()(const IDCommand& cmd, MatchManager* matchManager){ return; }