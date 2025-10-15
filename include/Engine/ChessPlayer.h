//
// Created by jacks on 22/06/2025.
//

#ifndef CHESSPLAYER_H
#define CHESSPLAYER_H


#include <memory>

#include "Piece.h"
#include "CommandHandlerBase.h"

class CommunicatorBase;

enum PlayerType {
    ENGINE,
    HUMAN
};

class ChessPlayer {
public:

    virtual ~ChessPlayer() = default;
    explicit ChessPlayer(PlayerType playerType);

    virtual bool sendCommand(const std::string& command) = 0;
    virtual std::string readResponse() = 0;


    PlayerType playerType;

    [[nodiscard]] std::string& engineID(){ return engineID_; }
    virtual void setEngineID(const std::string& engineID){ engineID_ = engineID; }

protected:

    UCIParser parser;
    CommandHandlerBase commandHandler;
    std::string engineID_ = "default";
};


#endif //CHESSPLAYER_H