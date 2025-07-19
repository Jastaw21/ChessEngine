//
// Created by jacks on 22/06/2025.
//

#include "../include/HumanPlayer.h"
#include "BoardManager/BoardManager.h"

HumanPlayer::HumanPlayer(const Colours colour): ChessPlayer(colour, HUMAN),
                                                clickedSquare_(-1){}


Move HumanPlayer::selectDestination(const int destSquare, BoardManager* manager){
    if (clickedSquare_ == -1)
        return Move();
    const auto candidateMovePiece = manager->getBitboards()->getPiece(clickedSquare_);

    int clickedRank, clickedFile, rank, file;
    squareToRankAndFile(clickedSquare_, clickedRank, clickedFile);
    squareToRankAndFile(destSquare, rank, file);
    const auto move = Move(candidateMovePiece.value(), clickedSquare_, destSquare);
    clickedSquare_ = -1;
    return move;
}

void HumanPlayer::parseUCI(const std::string& uci){}