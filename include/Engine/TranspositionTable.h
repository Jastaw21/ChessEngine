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

struct TTStats {
    uint64_t entries = 0;
    uint64_t emptyStores = 0;
    uint64_t overWritingStores = 0;
    uint64_t hits = 0;
    uint64_t rejectedStores = 0;
    uint64_t probes = 0;
    uint64_t collisions = 0;


    void print() const;
};

class TranspositionTable {
public:

    TranspositionTable(size_t sizeinMB = 128);

    void storeVector(TTEntry& entry);
    std::optional<TTEntry> retrieveVector(uint64_t& key);

    size_t size() const{ return vectorTable.size() * sizeof(TTEntry); }
    size_t entries() const{ return vectorTable.size(); }
    size_t populatedEntries() const;

    auto getPopulatedEntries() const{
        std::vector<TTEntry> result;
        for (const auto entry: vectorTable) { if (entry.key != 0) { result.push_back(entry); } }
        return result;
    }

    TTStats& getStats(){ return stats; }

    void clear();

private:

    size_t maxSize;
    std::vector<TTEntry> vectorTable;
    TTStats stats;
};


#endif //CHESS_TRANSPOSITIONTABLE_H