//
// Created by jacks on 22/06/2025.
//

#ifndef CHESSPLAYER_H
#define CHESSPLAYER_H
#include "BoardManager/BoardManager.h"

enum PlayerType {
    ENGINE,
    HUMAN
};

class ChessPlayer {
public:

    virtual ~ChessPlayer() = default;
    explicit ChessPlayer(Colours colour, PlayerType playerType);

    virtual Move makeMove() =0;

    bool setReady(const bool isReady){
        bIsReady = isReady;
        return bIsReady;
    };
    bool isReady() const{ return bIsReady; }
    void setColour(const Colours& colour){ this->colour = colour; }
    Colours getColour() const{ return colour; }

    PlayerType playerType;

protected:

    bool bIsReady = false;

    Colours colour;
};


#endif //CHESSPLAYER_H