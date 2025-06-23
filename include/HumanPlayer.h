//
// Created by jacks on 22/06/2025.
//

#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H
#include <iostream>
#include <bits/ostream.tcc>

#include "ChessPlayer.h"


class HumanPlayer final : public ChessPlayer {
public:

    explicit HumanPlayer(const Colours colour = Colours::WHITE);

    virtual Move makeMove() override;

    void pickUpPiece(const int clickedSquare){ clickedSquare_ = clickedSquare; }
    [[nodiscard]] int getHeldPiece() const{ return clickedSquare_; }
    void dropPiece(){ clickedSquare_ = -1; }
    void selectDestination(const int destSquare, BoardManager* manager);

private:

    int clickedSquare_;;

};


#endif //HUMANPLAYER_H