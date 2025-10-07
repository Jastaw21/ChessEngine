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

    TranspositionTable(size_t sizeinMB = 64);

    void storeVector(TTEntry& entry);
    std::optional<TTEntry> retrieveVector(uint64_t& key);

    size_t size() const{ return vectorTable.size() * sizeof(TTEntry); }
    size_t entries() const{ return vectorTable.size(); }
    size_t populatedEntries() const;

    auto getPopulatedEntries(){
        std::vector<TTEntry> result;
        for (const auto entry: vectorTable) { if (entry.key != 0) { result.push_back(entry); } }
        return result;
    }

    void clear();

private:

    size_t maxSize;
    std::vector<TTEntry> vectorTable;
};


#endif //CHESS_TRANSPOSITIONTABLE_H