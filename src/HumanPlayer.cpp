//
// Created by jacks on 22/06/2025.
//

#include "../include/HumanPlayer.h"
#include "BoardManager/BoardManager.h"

HumanPlayer::HumanPlayer(const Colours colour) : ChessPlayer(HUMAN){ colour_ = colour; }


void HumanPlayer::parseUCI(const std::string& uci){}

std::optional<std::string> HumanPlayer::consumeMessage(){
    if (getMessage().empty()) return {};
    auto returnValue = getMessage();
    message_ = "";
    return returnValue;
}