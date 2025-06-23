//
// Created by jacks on 22/06/2025.
//

#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "ChessPlayer.h"


class HumanPlayer final : public ChessPlayer {
public:

    explicit HumanPlayer(Colours colour = Colours::WHITE);

    virtual Move makeMove() override;

    void pickUpPiece(const int clickedSquare){ clickedSquare_ = clickedSquare; }
    [[nodiscard]] int getHeldPiece() const{ return clickedSquare_; }
    void dropPiece(){ clickedSquare_ = -1; }
    void selectDestination(int destSquare, BoardManager* manager);

private:

    int clickedSquare_;

};


#endif //HUMANPLAYER_H