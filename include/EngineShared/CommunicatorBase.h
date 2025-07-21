//
// Created by jacks on 19/07/2025.
//

#ifndef COMMUNICATORBASE_H
#define COMMUNICATORBASE_H

#include <string>

#include "MatchManager/MatchManager.h"
class MatchManager;

class CommunicatorBase {
public:

    virtual ~CommunicatorBase() = default;

    virtual void send(const std::string& command) = 0;

    virtual std::string receive() = 0;
};

class TerminalCommunicator : public CommunicatorBase {
public:

    virtual void send(const std::string& command) override;
    virtual std::string receive() override;
};

class MatchManagerCommunicator : public CommunicatorBase {
public:

    explicit MatchManagerCommunicator(MatchManager* match_manager) : manager_(match_manager){}

    virtual void send(const std::string& command) override;
    virtual std::string receive() override;

private:

    MatchManager* manager_ = nullptr;
    MatchManagerCommunicator() = default;
};


#endif //COMMUNICATORBASE_H
