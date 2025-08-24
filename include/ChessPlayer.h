//
// Created by jacks on 22/06/2025.
//

#ifndef CHESSPLAYER_H
#define CHESSPLAYER_H


#include <memory>

#include "Engine/Piece.h"
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
    std::shared_ptr<CommunicatorBase> getCommunicator() const{ return communicator_; }
    void setCommunicator(const std::shared_ptr<CommunicatorBase>& communicator){ communicator_ = communicator; }

    virtual std::optional<std::string> consumeMessage();

    PlayerType playerType;

    [[nodiscard]] std::string &engineID(){ return engineID_; }
    void setEngineID(const std::string& engineID){ engineID_ = engineID; }

protected:

    UCIParser parser;
    CommandHandlerBase commandHandler;
    std::shared_ptr<CommunicatorBase> communicator_;

    std::string engineID_ = "default";
};


#endif //CHESSPLAYER_H