//
// Created by jacks on 22/06/2025.
//

#ifndef CHESSPLAYER_H
#define CHESSPLAYER_H


#include "EngineShared/CommandHandlerBase.h"

class CommunicatorBase;

enum PlayerType {
    ENGINE,
    HUMAN
};

class ChessPlayer {
public:

    virtual ~ChessPlayer() = default;
    explicit ChessPlayer(PlayerType playerType);

    virtual void parseUCI(const std::string& uci) = 0;

    bool setReady(const bool isReady){ return true; };
    bool isReady() const{ return true; }
    CommunicatorBase *getCommunicator() const{ return communicator_; }
    void setCommunicator(CommunicatorBase* communicator){ communicator_ = communicator; }

    PlayerType playerType;

protected:

    UCIParser parser;
    CommandHandlerBase commandHandler;
    CommunicatorBase* communicator_;
};


#endif //CHESSPLAYER_H