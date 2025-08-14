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

    virtual void sendToEngine(const std::string& command) = 0;

    virtual std::string receiveFromEngine() = 0;

    MessageQueue messageQueueToEngine;
    MessageQueue messageQueueFromEngine;
};

class TerminalCommunicator : public CommunicatorBase {
public:

    virtual void sendToEngine(const std::string& command) override;
    virtual std::string receiveFromEngine() override;
};

class MatchManagerCommunicator : public CommunicatorBase {
public:

    explicit MatchManagerCommunicator(MatchManager* match_manager) : manager_(match_manager){}

    virtual void sendToEngine(const std::string& command) override;
    virtual std::string receiveFromEngine() override;

private:

    MatchManager* manager_ = nullptr;
    MatchManagerCommunicator() = default;
};


#endif //COMMUNICATORBASE_H