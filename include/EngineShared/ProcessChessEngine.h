//
// Created by jacks on 31/08/2025.
//

#ifndef CHESS_PROCESSCHESSENGINE_H
#define CHESS_PROCESSCHESSENGINE_H
#include <iostream>

#include "ChessPlayer.h"
#include <windows.h>


class ProcessChessEngine final : public ChessPlayer {
private:

    HANDLE hChildStdInWrite;
    HANDLE hChildStdOutRead;
    PROCESS_INFORMATION processInfo;
    std::string enginePath;

public:

    ~ProcessChessEngine(){
        std::cout << "deleting process" << std::endl;
        stopProcess();
    }

    ProcessChessEngine(const std::string& exePath);
    bool startEngine();
    virtual bool sendCommand(const std::string& command) override;
    virtual std::string readResponse() override;

    void stopProcess();
};


#endif //CHESS_PROCESSCHESSENGINE_H