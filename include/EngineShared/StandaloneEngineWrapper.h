//
// Created by jacks on 14/08/2025.
//

#ifndef CHESS_CHESSPLAYERADAPTER_H
#define CHESS_CHESSPLAYERADAPTER_H

#include <memory>
#include <windows.h>

class StandaloneEngineWrapper {
public:

    explicit StandaloneEngineWrapper(const std::string& enginePath);
    ~StandaloneEngineWrapper();

    void send(const std::string& cmd);
    void read_line();

    std::string last_line = "";

private:

    HANDLE hRead;
    HANDLE hWrite;
    HANDLE childRead;
    HANDLE childWrite;
    PROCESS_INFORMATION pi;
    std::string enginePath;
};


#endif //CHESS_CHESSPLAYERADAPTER_H