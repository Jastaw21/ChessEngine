//
// Created by jacks on 16/06/2025.
//

#include "../include/TestEngine.h"

bool TestEngine::moveIsLegal(const std::string &move) {
    return true;
}

bool TestEngine::moveIsPossible(const std::string &move) {
    return true;
}

bool TestEngine::checkMove(const std::string &move) {
    return moveIsLegal(move) && moveIsPossible(move);
}

void TestEngine::evaluate() {
    return;
}

void TestEngine::search() {
    return;
}
