//
// Created by jacks on 22/06/2025.
//

#include "../include/HumanPlayer.h"

HumanPlayer::HumanPlayer(const Colours colour): ChessPlayer(colour, PlayerType::HUMAN),
                                                clickedSquare_(-1){}

Move HumanPlayer::makeMove(){}

void HumanPlayer::selectDestination(const int destSquare, BoardManager* manager){
    if (clickedSquare_ == -1)
        return;
    auto candidateMovePiece = manager->getBitboards()->getPiece(clickedSquare_);

    int clickedRank, clickedFile, rank, file;
    squareToRankAndFile(clickedSquare_, clickedRank, clickedFile);
    squareToRankAndFile(destSquare, rank, file);
    auto move = Move{
                .piece = candidateMovePiece.value(),
                .rankFrom = clickedRank,
                .fileFrom = clickedFile,
                .rankTo = rank, .fileTo = file
            };

    manager->tryMove(move);

    clickedSquare_ = -1;
}