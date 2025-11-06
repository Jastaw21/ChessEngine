//
// Created by jacks on 05/08/2025.
//


#include "Engine/ChessEngine.h"


int main(){
    auto engine = ChessEngine();
    while (!engine.shouldQuit()) {
        char input[1024];
        std::cin.getline(input, 1024);
        engine.sendCommand(input);
    }
}