//
// Created by jacks on 22/06/2025.
//

#include "../include/ChessPlayer.h"

#include "EngineShared/CommunicatorBase.h"

ChessPlayer::ChessPlayer(const PlayerType playerType) : playerType(playerType){}

void ChessPlayer::parseUCI(const std::string& uci){}


std::optional<std::string> ChessPlayer::consumeMessage() const{
    if (getCommunicator() == nullptr) return {};

    return getCommunicator()->consumeEngineMessage();
}