//
// Created by jacks on 19/07/2025.
//

// ReSharper disable CppMemberFunctionMayBeConst
#include "../../include/MatchManager/ManagerCommandHandler.h"

#include "../../include/MatchManager/MatchManager.h"
#include "Engine/EngineBase.h"

void ManagerCommandHandler::operator()(const UCICommand& cmd, MatchManager* matchManager){
    matchManager->getMessageQueueOutbound()->push("uci");
    matchManager->swapPlayers();
}

void ManagerCommandHandler::operator()(const StopCommand& cmd, MatchManager* matchManager){
    matchManager->getMessageQueueOutbound()->push("stop");
    matchManager->swapPlayers();
}

void ManagerCommandHandler::operator()(const IsReadyCommand& cmd, MatchManager* matchManager){
    matchManager->getMessageQueueOutbound()->push("isready");
    matchManager->swapPlayers();
}

void ManagerCommandHandler::operator()(const QuitCommand& cmd, MatchManager* matchManager){
    matchManager->getMessageQueueOutbound()->push("quit");
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
    matchManager->getMessageQueueOutbound()->push(fullPositionCommand);
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

        std::vector<std::string> moves;
        auto tempStack = moveHistory;
        while (!tempStack.empty()) {
            moves.push_back(tempStack.top().toUCI());
            tempStack.pop();
        }

        for (auto it = moves.rbegin(); it != moves.rend(); ++it) { fullPositionCommand += *it + " "; }
    }
    matchManager->getMessageQueueOutbound()->push(fullPositionCommand);
    const auto engine = static_cast<EngineBase *>(matchManager->currentPlayer());
    const auto depth = matchManager->currentPlayer()->playerType == HUMAN ? 2 : engine->getSearchDepth();
    matchManager->getMessageQueueOutbound()->push("go depth " + std::to_string(depth));
}

void ManagerCommandHandler::operator()(const NewGameCommand& cmd, MatchManager* matchManager){}
void ManagerCommandHandler::operator()(const IDCommand& cmd, MatchManager* matchManager){ return; }