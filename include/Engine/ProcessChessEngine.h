//
// Created by jacks on 31/08/2025.
//

#ifndef CHESS_PROCESSCHESSENGINE_H
#define CHESS_PROCESSCHESSENGINE_H
#include <filesystem>
#include <iostream>

#include "ChessPlayer.h"
#include <windows.h>
#include <fstream>

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
    ProcessChessEngine(const std::string& exePath, const std::string& commsLog);

    virtual void setEngineID(const std::string& engineID) override;

    std::filesystem::path path;
    std::string pathSuffix = "";
    std::ofstream outStream;
};


#endif //CHESS_PROCESSCHESSENGINE_H