//
// Created by jacks on 10/09/2025.
//

#include "../../include/Engine/TranspositionTable.h"

TranspositionTable::TranspositionTable(size_t size) : maxSize(size), vectorTable(size){}

void TranspositionTable::store(TTEntry& entry){
    auto last16Bits = MathUtility::getXBits(entry.key, 16);

    table[last16Bits] = entry;

    if (table.size() > maxSize / sizeof(TTEntry)) { table.erase(table.begin()); }
}

void TranspositionTable::storeVector(TTEntry& newEntry){
    // index the table by this mask. This mask always falls within the table's range, so we overwrite, for now just on age
    auto& entry = vectorTable[newEntry.key & maxSize - 1];

    if (newEntry.age < entry.age) { entry = newEntry; }
}


std::optional<TTEntry> TranspositionTable::retrieve(uint64_t& key){
    auto last16Bits = MathUtility::getXBits(key, 16);

    if (table.find(last16Bits) == table.end()) { return std::nullopt; }

    if (table[last16Bits].key == key) { return table[last16Bits]; }
    return std::nullopt;
}

std::optional<TTEntry> TranspositionTable::retrieveVector(uint64_t& key){
    TTEntry& entry = vectorTable[key & (maxSize - 1)];
    if (entry.key == key) {
        return entry; // exact match
    }
    return std::nullopt; // either empty slot or collision
}