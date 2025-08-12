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

    [[nodiscard]] std::string &engineID(){ return engineID_; }
    void setEngineID(const std::string& engineID){ engineID_ = engineID; }

protected:

    UCIParser parser;
    CommandHandlerBase commandHandler;
    CommunicatorBase* communicator_;

    std::string engineID_ = "default";
};


#endif //CHESSPLAYER_H