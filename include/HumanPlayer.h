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


    void addMessage(const std::string& message){ message_ = message; }
    [[nodiscard]] std::string getMessage() const{ return message_; }

    virtual void parseUCI(const std::string& uci) override;
    virtual std::optional<std::string> consumeMessage() override;

private:

    Colours colour_;
    std::string message_ = "";
};


#endif //HUMANPLAYER_H