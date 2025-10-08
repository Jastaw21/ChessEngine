//
// Created by jacks on 10/09/2025.
//

#include "Engine/TranspositionTable.h"

#include <ranges>


void TTStats::print() const{
    std::cout << "Entries: " << entries << "\n";
    std::cout << "Empty stores: " << emptyStores << "\n";
    std::cout << "Overwriting stores: " << overWritingStores << "\n";
    std::cout << "Rejected stores: " << rejectedStores << "\n";
    std::cout << "Probes: " << probes << "\n";
    std::cout << "Hits: " << hits << "\n";
    std::cout << "Collisions: " << collisions << "\n";
    std::cout << "Hit rate: " << (probes > 0 ? 100.0 * hits / probes : 0) << "%\n";
    std::cout << "Fill rate: " << (entries > 0 ? 100.0 * (emptyStores + overWritingStores) / entries : 0) << "%\n";
}

TranspositionTable::TranspositionTable(size_t sizeinMB){
    size_t numberEntries;

    numberEntries = 1;

    while (numberEntries * 2 <= (sizeinMB * 1024 * 1024) / sizeof(TTEntry)) { numberEntries *= 2; }

    maxSize = numberEntries;
    vectorTable.resize(numberEntries);
    stats = TTStats();
    stats.entries = numberEntries;
}


void TranspositionTable::storeVector(TTEntry& newEntry){
    auto& entry = vectorTable[newEntry.key & (maxSize - 1)];
    // always newer, more recent search

    if (entry.key == 0) {
        stats.emptyStores++;
        entry = newEntry;
        return;
    }

    if (entry.key == newEntry.key || newEntry.depth >= entry.depth || entry.age < newEntry.age - 2) {
        stats.overWritingStores++;
        entry = newEntry;
    } else { stats.rejectedStores++; }
}


std::optional<TTEntry> TranspositionTable::retrieveVector(uint64_t& key){
    stats.probes++;

    TTEntry& entry = vectorTable[key & (maxSize - 1)];

    if (entry.key == 0) { return std::nullopt; } // empty slot

    if (entry.key == key) {
        stats.hits++;
        return entry;
    } // exact match

    stats.collisions++;
    return std::nullopt;
}

size_t TranspositionTable::populatedEntries() const{
    return std::ranges::count_if(vectorTable, [](const auto& entry) { return entry.key != 0; });
}

void TranspositionTable::clear(){
    vectorTable.clear();
    vectorTable.resize(maxSize);
    stats = TTStats();
}