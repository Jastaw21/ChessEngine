//
// Created by jacks on 13/09/2025.
//

#include "Engine/RepetitionTable.h"

bool RepetitionTable::checkForRepetition() const{
    const auto currentState = table.back();
    int matchCounter = 0;
    for (int i = table.size() - 3; i >= 0; i -= 2) {
        if (table[i] == currentState) { matchCounter++; }
        if (matchCounter == 2) { return true; }
    }
    return false;
}