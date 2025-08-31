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

    virtual bool sendCommand(const std::string& command) override{ return true; };

    virtual std::string readResponse() override{
        auto returnValue = message_;
        message_ = "";
        return returnValue;
    }

    void addMessage(const std::string& message){ message_ = message; }

private:

    Colours colour_;
    std::string message_ = "";
};


#endif //HUMANPLAYER_H