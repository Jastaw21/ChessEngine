//
// Created by jacks on 10/09/2025.
//

#ifndef CHESS_TRANSPOSITIONTABLE_H
#define CHESS_TRANSPOSITIONTABLE_H
#include "BoardManager/BoardManager.h"

struct TTEntry {
    uint64_t key;
    float eval;
    Move bestMove;
    int depth;
    int age;
};


class TranspositionTable {
public:

    TranspositionTable(size_t size = 1024 * 512);

    std::unordered_map<uint16_t, TTEntry> table;
    std::vector<TTEntry> vectorTable;

    void store(TTEntry& entry);
    void storeVector(TTEntry& entry);

    std::optional<TTEntry> retrieve(uint64_t& key);
    std::optional<TTEntry> retrieveVector(uint64_t& key);


    size_t size() const{ return table.size() * sizeof(TTEntry); }
    size_t entries() const{ return table.size(); }
    size_t maxSize = 1024 * 512; // 512 mb?

    uint16_t getOldestAge() const{
        return std::ranges::min_element(table, [](const auto& a, const auto& b) {
            return a.second.age < b.second.age;
        })->first;
    }
};


#endif //CHESS_TRANSPOSITIONTABLE_H