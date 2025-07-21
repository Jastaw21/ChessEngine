//
// Created by jacks on 19/07/2025.
//

#include "../../include/MatchManager/MatchManager.h"
#include "EngineShared/CommunicatorBase.h"

MatchManager::MatchManager(ChessPlayer* startingPlayer, ChessPlayer* otherPlayer){
    currentPlayer_ = startingPlayer;
    otherPlayer_ = otherPlayer;
    const auto currentCommunicator = new MatchManagerCommunicator(this);
    currentPlayer_->setCommunicator(currentCommunicator);
    const auto otherCommunicator = new MatchManagerCommunicator(this);
    otherPlayer_->setCommunicator(otherCommunicator);
}


void MatchManager::tick(){}

void MatchManager::parseUCI(const std::string& uci){
    auto command = parser.parse(uci);
    // Create a visitor lambda that captures 'this' and forwards to command handler
    auto visitor = [this](const auto& cmd) { this->commandHandler(cmd, this); };
    std::visit(visitor, *command);
}


std::stack<Move> &MatchManager::getMoveHistory(){ return boardManager.getMoveHistory(); }
void MatchManager::addMove(const std::string& moveUCI){ boardManager.tryMove(moveUCI); }