//
// Created by jacks on 14/08/2025.
//

#ifndef CHESS_STANDALONEUCIPLAYER_H
#define CHESS_STANDALONEUCIPLAYER_H

#include <atomic>
#include <thread>
#include "ChessPlayer.h"
#include "EngineShared/StandaloneEngineWrapper.h"

class StandaloneUCIPlayer : public ChessPlayer {
public:

    explicit StandaloneUCIPlayer(const std::string& enginePath);
    ~StandaloneUCIPlayer() override;

    void parseUCI(const std::string& uci) override;
    StandaloneEngineWrapper *getWrapper(){ return &wrapper; }

private:

    StandaloneEngineWrapper wrapper;
    std::atomic<bool> running;
    std::thread readerThread;

    void readerLoop();
};


#endif //CHESS_STANDALONEUCIPLAYER_H