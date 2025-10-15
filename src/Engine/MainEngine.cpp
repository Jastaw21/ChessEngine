//
// Created by jacks on 16/06/2025.
//

#include "Engine/MainEngine.h"


bool MainEngine::sendCommand(const std::string& command){
    parseUCI(command);
    return true;
}

std::string MainEngine::readResponse(){ return ""; }