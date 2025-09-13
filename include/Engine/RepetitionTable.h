//
// Created by jacks on 13/09/2025.
//

#ifndef CHESS_REPETITIONTABLE_H
#define CHESS_REPETITIONTABLE_H
#include <cstdint>
#include <vector>

class RepetitionTable {
public:

    RepetitionTable(){
        // 200 plies should be enough to start with
        table.reserve(200);
    };


    void addMove(uint64_t key){ table.push_back(key); }
    void popMove(){ table.pop_back(); }
    void clear(){ table.clear(); }
    size_t size(){ return table.size(); }

    bool checkForRepetition() const;

private:

    std::vector<uint64_t> table;
};


#endif //CHESS_REPETITIONTABLE_H