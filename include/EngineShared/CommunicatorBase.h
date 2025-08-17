//
// Created by jacks on 19/07/2025.
//

#ifndef COMMUNICATORBASE_H
#define COMMUNICATORBASE_H

#include <string>

#include "MatchManager/MatchManager.h"
class MatchManager;
using MessageQueue = std::queue<std::string>;


class CommunicatorBase {
public:

    virtual ~CommunicatorBase() = default;
    void resetQueues(){ while (!messageQueueFromEngine.empty()) { messageQueueFromEngine.pop(); } };

    CommunicatorBase(MatchManager* manager, ChessPlayer* player){
        manager_ = manager;
        player_ = player;
    }

    virtual void sendFromEngine(const std::string& command){ messageQueueFromEngine.push(command); }

    virtual void sendToEngine(const std::string& command){
        if (player_ == nullptr) return;
        player_->parseUCI(command);
    }

    virtual std::optional<std::string> consumeEngineMessage(){
        if (messageQueueFromEngine.empty()) return {};
        auto returnValue = messageQueueFromEngine.front();
        messageQueueFromEngine.pop();
        return returnValue;
    }


    MessageQueue getMessageQueueFromEngine() const{ return messageQueueFromEngine; }
    MatchManager *getManager() const{ return manager_; }
    void setManager(MatchManager* manager){ this->manager_ = manager; }
    ChessPlayer *getPlayer() const{ return player_; }
    void setPlayer(ChessPlayer* player){ this->player_ = player; }

private:

    MessageQueue messageQueueFromEngine{};

    MatchManager* manager_ = nullptr;
    ChessPlayer* player_ = nullptr;
};

class TerminalCommunicator : public CommunicatorBase {
public:

    virtual std::optional<std::string> consumeEngineMessage() override;
};


#endif //COMMUNICATORBASE_H