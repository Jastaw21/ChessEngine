//
// Created by jacks on 14/08/2025.
//

#include "../../include/EngineShared/StandaloneEngineWrapper.h"

#include <iostream>


StandaloneEngineWrapper::StandaloneEngineWrapper(const std::string& enginePath) : enginePath(enginePath){
    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};

    // Parent read, child write
    CreatePipe(&hRead, &childWrite, &sa, 0);
    SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

    // Child read, parent write
    CreatePipe(&childRead, &hWrite, &sa, 0);
    SetHandleInformation(hWrite, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si = {sizeof(STARTUPINFOA)};
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = childRead;
    si.hStdOutput = childWrite;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    std::string cmd = "\"" + enginePath + "\"";

    if (!CreateProcessA(
        NULL, &cmd[0], NULL, NULL, TRUE,
        0, NULL, NULL, &si, &pi)) {
        std::cerr << "Failed to start engine: " << enginePath << "\n";
        exit(1);
    }

    CloseHandle(childRead);
    CloseHandle(childWrite);
}

StandaloneEngineWrapper::~StandaloneEngineWrapper(){
    CloseHandle(hRead);
    CloseHandle(hWrite);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void StandaloneEngineWrapper::send(const std::string& cmd){
    std::string msg = cmd + "\n";
    DWORD written;
    WriteFile(hWrite, msg.c_str(), msg.size(), &written, NULL);
}

void StandaloneEngineWrapper::read_line(){
    char buffer[1];
    std::string line;
    DWORD read;
    while (true) {
        if (!ReadFile(hRead, buffer, 1, &read, NULL) || read == 0) break;
        if (buffer[0] == '\n') break;
        if (buffer[0] != '\r') line += buffer[0];
    }
    last_line = line;
}