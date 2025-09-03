//
// Created by jacks on 05/08/2025.
//


#include "../../include/Engine/MainEngine.h"


int main(){
    auto engine = MainEngine();
    engine.setEvaluator(std::make_shared<GoodEvaluator>(engine.boardManager()));
    while (!engine.shouldQuit()) {
        char input[1024];
        std::cin.getline(input, 1024);
        engine.sendCommand(input);
    }
}