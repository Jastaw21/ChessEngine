//
// Created by jacks on 19/07/2025.
//

#include "../../include/EngineShared/CommunicatorBase.h"

#include "CLIEngine/StandaloneUciPlayer.h"

std::optional<std::string> TerminalCommunicator::consumeEngineMessage(){
    auto playerAsStandaloneEngine = static_cast<StandaloneUCIPlayer *>(getPlayer());

    playerAsStandaloneEngine->getWrapper()->read_line();

    if (playerAsStandaloneEngine->getWrapper()->last_line == "") return {};

    auto returnValue = playerAsStandaloneEngine->getWrapper()->last_line;
    playerAsStandaloneEngine->getWrapper()->last_line = "";
    return playerAsStandaloneEngine->getWrapper()->last_line;
}