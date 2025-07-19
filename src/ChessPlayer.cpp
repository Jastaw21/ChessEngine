//
// Created by jacks on 22/06/2025.
//

#include "../include/ChessPlayer.h"

ChessPlayer::ChessPlayer(const Colours colour, const PlayerType playerType) : playerType(playerType),
                                                                              colour(colour){}

void ChessPlayer::parseUCI(const std::string& uci){}