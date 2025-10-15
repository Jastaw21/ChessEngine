//
// Created by jacks on 19/07/2025.
//

// ReSharper disable CppMemberFunctionMayBeConst
#include "MatchManager/ManagerCommandHandler.h"
#include "MatchManager/MatchManager.h"
#include "Engine/EngineBase.h"
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

void ManagerCommandHandler::operator()(const UCICommand& cmd, MatchManager* matchManager){}
void ManagerCommandHandler::operator()(const StopCommand& cmd, MatchManager* matchManager){}

void ManagerCommandHandler::operator()(const IsReadyCommand& cmd, MatchManager* matchManager){}

void ManagerCommandHandler::operator()(const QuitCommand& cmd, MatchManager* matchManager){}

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
    matchManager->currentPlayer()->sendCommand(fullPositionCommand);
    matchManager->swapPlayers();
}

void ManagerCommandHandler::operator()(const BestMoveCommand& cmd, MatchManager* matchManager){
    if (cmd.move.at(1) == '0') { std::cout << cmd.move << std::endl; }

    matchManager->addMove(cmd.move); // add our history

    // generate the command with move history from our internal state
    std::string fullPositionCommand;
    generateFullPositionCommand(matchManager, fullPositionCommand);

    if (matchManager->currentPlayer()->playerType != HUMAN) {
        matchManager->getGui()->receiveInfoOfEngineMove(matchManager->getMoveHistory().top());
    }

    // we've done receiving from the current player and need to swap to forward the command
    matchManager->swapPlayers();

    matchManager->currentPlayer()->sendCommand(fullPositionCommand); // send to engine

    std::string goCommand = "go ";
    goCommand += "go wtime ";
    goCommand += std::to_string(matchManager->getTimeInfo().whiteTime);
    goCommand += " btime ";
    goCommand += std::to_string(matchManager->getTimeInfo().blackTime);
    goCommand += " winc 1";
    goCommand += " binc 1";

    matchManager->currentPlayer()->sendCommand(goCommand);
}

void ManagerCommandHandler::operator()(const NewGameCommand& cmd, MatchManager* matchManager){}
void ManagerCommandHandler::operator()(const IDCommand& cmd, MatchManager* matchManager){ return; }

void ManagerCommandHandler::operator()(const SetIDCommand& cmd, MatchManager* matchManager){
    // not relevant
}