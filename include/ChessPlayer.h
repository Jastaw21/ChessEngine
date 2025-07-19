//
// Created by jacks on 22/06/2025.
//

#ifndef CHESSPLAYER_H
#define CHESSPLAYER_H


#include "Engine/Piece.h"
#include "EngineShared/CommandHandlerBase.h"

enum PlayerType {
    ENGINE,
    HUMAN
};

class ChessPlayer {
public:

    virtual ~ChessPlayer() = default;
    explicit ChessPlayer(Colours colour, PlayerType playerType);

    virtual void parseUCI(const std::string& uci) = 0;

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

    UCIParser parser;
    CommandHandlerBase commandHandler;
};


#endif //CHESSPLAYER_H