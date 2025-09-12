//
// Created by jacks on 10/09/2025.
//

#include "../../include/Engine/TranspositionTable.h"

void TranspositionTable::store(TTEntry entry){
    auto last16Bits = MathUtility::getXBits(entry.key, 16);

    if (size() + sizeof(TTEntry) < maxSize) { table[last16Bits] = entry; } else {
        auto oldestEntry = getOldestAge();
        table.erase(oldestEntry);
    }
}

std::optional<TTEntry> TranspositionTable::retrieve(uint64_t& key){
    auto last16Bits = MathUtility::getXBits(key, 16);

    if (table.find(last16Bits) == table.end()) { return std::nullopt; }

    if (table[last16Bits].key == key) { return table[last16Bits]; }
    return std::nullopt;
}