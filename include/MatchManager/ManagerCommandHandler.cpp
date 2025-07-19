//
// Created by jacks on 19/07/2025.
//

// ReSharper disable CppMemberFunctionMayBeConst
#include "../BoardManager/ManagerCommandHandler.h"

#include "MatchManager.h"

void ManagerCommandHandler::operator()(const UCICommand& cmd, MatchManager* matchManager){
    matchManager->otherPlayer->parseUCI("uci");
    matchManager->swapPlayers();
}

void ManagerCommandHandler::operator()(const StopCommand& cmd, MatchManager* matchManager){
    matchManager->otherPlayer->parseUCI("stop");
    matchManager->swapPlayers();
}

void ManagerCommandHandler::operator()(const IsReadyCommand& cmd, MatchManager* matchManager){
    matchManager->otherPlayer->parseUCI("isready");
    matchManager->swapPlayers();
}

void ManagerCommandHandler::operator()(const QuitCommand& cmd, MatchManager* matchManager){
    matchManager->otherPlayer->parseUCI("quit");
    matchManager->swapPlayers();
}

void ManagerCommandHandler::operator()(const GoCommand& cmd, MatchManager* matchManager){
    // std::string fullGoCommand = "go";
    // if (cmd.depth.has_value()) { fullGoCommand += " depth " + std::to_string(*cmd.depth); }
    // matchManager->otherPlayer->parseUCI(fullGoCommand);
    // matchManager->swapPlayers();
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
    matchManager->otherPlayer->parseUCI(fullPositionCommand);
    matchManager->swapPlayers();
}

void ManagerCommandHandler::operator()(const BestMoveCommand& cmd, MatchManager* matchManager){
    matchManager->addMove(cmd.move);
    matchManager->swapPlayers();
    std::vector<std::string> movesString;

    std::string fullPositionCommand = "position " + matchManager->startingFen();

    auto& moveHistory = matchManager->getMoveHistory();

    if (!moveHistory.empty()) {
        fullPositionCommand += " moves ";

        auto tempStack = moveHistory;
        while (!tempStack.empty()) {
            fullPositionCommand += tempStack.top().toUCI();
            fullPositionCommand += " ";
            tempStack.pop();
        }
    }
    matchManager->currentPlayer->parseUCI(fullPositionCommand);
    matchManager->currentPlayer->parseUCI("go depth 2");
}
