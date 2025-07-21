//
// Created by jacks on 22/06/2025.
//

#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "ChessPlayer.h"
#include "BoardManager/BoardManager.h"

class BoardManager;

class HumanPlayer final : public ChessPlayer {
public:

    explicit HumanPlayer(Colours colour = WHITE);

    void pickUpPiece(const int clickedSquare){ clickedSquare_ = clickedSquare; }
    [[nodiscard]] int getHeldPiece() const{ return clickedSquare_; }
    void dropPiece(){ clickedSquare_ = -1; }
    Move selectDestination(int destSquare, BoardManager* manager);

    virtual void parseUCI(const std::string& uci) override;

private:

    int clickedSquare_;
    Colours colour_;
};


#endif //HUMANPLAYER_H