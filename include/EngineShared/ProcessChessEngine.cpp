//
// Created by jacks on 31/08/2025.
//

#include "ProcessChessEngine.h"

void ProcessChessEngine::stopProcess(){
    // Try graceful shutdown first
    if (hChildStdInWrite != NULL) {
        sendCommand("QUIT"); // Send quit command to engine

        // Give the process a moment to exit gracefully
        DWORD waitResult = WaitForSingleObject(processInfo.hProcess, 2000); // 2 second timeout

        if (waitResult != WAIT_OBJECT_0) {
            // Process didn't exit gracefully, force terminate
            TerminateProcess(processInfo.hProcess, 1);
        }
    } else {
        // No stdin pipe, just force terminate
        TerminateProcess(processInfo.hProcess, 1);
    }

    // Clean up handles
    if (hChildStdInWrite != NULL) {
        CloseHandle(hChildStdInWrite);
        hChildStdInWrite = NULL;
    }
    if (hChildStdOutRead != NULL) {
        CloseHandle(hChildStdOutRead);
        hChildStdOutRead = NULL;
    }

    if (processInfo.hProcess != NULL) {
        CloseHandle(processInfo.hProcess);
        processInfo.hProcess = NULL;
    }

    if (processInfo.hThread != NULL) {
        CloseHandle(processInfo.hThread);
        processInfo.hThread = NULL;
    }
}

ProcessChessEngine::ProcessChessEngine(const std::string& exePath) : ChessPlayer(ENGINE){
    enginePath = exePath;
    startEngine();
}

bool ProcessChessEngine::startEngine(){
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create pipes for stdin/stdout
    HANDLE hChildStdOutWrite, hChildStdInRead;

    if (!CreatePipe(&hChildStdOutRead, &hChildStdOutWrite, &saAttr, 0) ||
        !CreatePipe(&hChildStdInRead, &hChildStdInWrite, &saAttr, 0)) { return false; }

    // Ensure the read/write handles are not inherited
    SetHandleInformation(hChildStdOutRead, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hChildStdInWrite, HANDLE_FLAG_INHERIT, 0);

    // Set up process startup info
    STARTUPINFOA siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFOA));
    siStartInfo.cb = sizeof(STARTUPINFOA);
    siStartInfo.hStdError = hChildStdOutWrite;
    siStartInfo.hStdOutput = hChildStdOutWrite;
    siStartInfo.hStdInput = hChildStdInRead;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Create the child process
    BOOL success = CreateProcessA(
        enginePath.c_str(),
        NULL,
        NULL, NULL,
        TRUE, // handles are inherited
        0,
        NULL, NULL,
        &siStartInfo,
        &processInfo
    );

    // Close handles we don't need
    CloseHandle(hChildStdOutWrite);
    CloseHandle(hChildStdInRead);

    return success != 0;
}

bool ProcessChessEngine::sendCommand(const std::string& command){
    std::string cmd = command + '\n';
    DWORD bytesWritten;

    return WriteFile(hChildStdInWrite, cmd.c_str(),
                     cmd.length(), &bytesWritten, NULL) != 0;
}

std::string ProcessChessEngine::readResponse(){
    std::string response;
    char buffer[1024];
    DWORD bytesRead;

    while (true) {
        if (!ReadFile(hChildStdOutRead, buffer, sizeof(buffer) - 1,
                      &bytesRead, NULL) || bytesRead == 0) { break; }

        buffer[bytesRead] = '\0';
        response += buffer;
        // Check if we have a complete line/response
        if (response.find('\n') != std::string::npos) { break; }
    }

    return response;
}