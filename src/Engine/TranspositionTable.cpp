//
// Created by jacks on 10/09/2025.
//

#include "Engine/TranspositionTable.h"

#include <ranges>


TranspositionTable::TranspositionTable(size_t sizeinMB){
    size_t numberEntries;

    numberEntries = 1;

    while (numberEntries * 2 <= (sizeinMB * 1024 * 1024) / sizeof(TTEntry)) { numberEntries *= 2; }

    maxSize = numberEntries;
    vectorTable.resize(numberEntries);
}


void TranspositionTable::storeVector(TTEntry& newEntry){
    auto& entry = vectorTable[newEntry.key & maxSize - 1];
    // always newer, more recent search
    if (entry.key == 0 || entry.key == newEntry.key || newEntry.depth >= entry.depth) { entry = newEntry; }
}


std::optional<TTEntry> TranspositionTable::retrieveVector(uint64_t& key){
    TTEntry& entry = vectorTable[key & (maxSize - 1)];
    if (entry.key == key && entry.key != 0) {
        //std::cout << "retrieved entry" << std::endl;
        return entry; // exact match
    }
    return std::nullopt; // either empty slot or collision
}

size_t TranspositionTable::populatedEntries() const{
    return std::ranges::count_if(vectorTable, [](const auto& entry) { return entry.key != 0; });
}

void TranspositionTable::clear(){
    vectorTable.clear();
    vectorTable.resize(maxSize);
}