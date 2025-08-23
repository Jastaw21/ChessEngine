//
// Created by jacks on 14/08/2025.
//

#include "CLIEngine/StandaloneUciPlayer.h"

#include "EngineShared/CommunicatorBase.h"


StandaloneUCIPlayer::StandaloneUCIPlayer(const std::string& enginePath)
    : ChessPlayer(ENGINE),
      wrapper(enginePath),
      running(true),
      readerThread(&StandaloneUCIPlayer::readerLoop, this){
    // Initialize UCI protocol
    wrapper.send("uci");
    // Optionally, you can wait for uciok here (or not if match manager handles it)
    wrapper.send("isready");
}

StandaloneUCIPlayer::~StandaloneUCIPlayer(){
    running = false;
    wrapper.send("quit");
    if (readerThread.joinable())
        readerThread.join();
}

void StandaloneUCIPlayer::parseUCI(const std::string& uci){
    std::cout << uci << std::endl;
    wrapper.send(uci);
}

void StandaloneUCIPlayer::readerLoop(){
    while (running) {
        wrapper.read_line();
        const std::string& line = wrapper.last_line;

        if (!line.empty() && communicator_) { communicator_->sendFromEngine(line); }
    }
}