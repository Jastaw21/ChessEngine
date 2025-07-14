//
// Created by jacks on 22/06/2025.
//

#include "../include/HumanPlayer.h"

HumanPlayer::HumanPlayer(const Colours colour): ChessPlayer(colour, HUMAN),
                                                clickedSquare_(-1){}

Move HumanPlayer::makeMove(){
    return Move();
}

void HumanPlayer::selectDestination(const int destSquare, BoardManager* manager){
    if (clickedSquare_ == -1)
        return;
    const auto candidateMovePiece = manager->getBitboards()->getPiece(clickedSquare_);

    int clickedRank, clickedFile, rank, file;
    squareToRankAndFile(clickedSquare_, clickedRank, clickedFile);
    squareToRankAndFile(destSquare, rank, file);
    auto move = Move(candidateMovePiece.value(), clickedSquare_, destSquare);

    manager->tryMove(move);

    clickedSquare_ = -1;
}