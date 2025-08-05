//
// Created by jacks on 05/08/2025.
//


#include "../../include/Engine/TestEngine.h"
#include "EngineShared/CommunicatorBase.h"

int main(){
    auto engine = TestEngine();

    while (true) {
        char input[1024];

        std::cin.getline(input, 1024);
        engine.parseUCI(input);
        engine.setCommunicator(new TerminalCommunicator());
    }
}