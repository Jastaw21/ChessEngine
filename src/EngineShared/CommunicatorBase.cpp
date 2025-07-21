//
// Created by jacks on 19/07/2025.
//

#include "../../include/EngineShared/CommunicatorBase.h"

#include <iostream>

#include "MatchManager/MatchManager.h"

void TerminalCommunicator::send(const std::string& command){ std::cout << command << std::endl; }

std::string TerminalCommunicator::receive(){
    std::string input;
    std::getline(std::cin, input);
    return input;
}

void MatchManagerCommunicator::send(const std::string& command){
    if (manager_ == nullptr)
        return;

    manager_->receiveCommand(command);
}

std::string MatchManagerCommunicator::receive()
{
    return std::string();
}