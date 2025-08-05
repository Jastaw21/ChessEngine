//
// Created by jacks on 04/08/2025.
//

#ifndef ZOBRISTHASH_H
#define ZOBRISTHASH_H
#include <random>


class ZobristHash {
public:

    ZobristHash();


    uint64_t hash(const std::string& fen);

private:

    std::mt19937_64 rng;
    uint64_t whitePawnRNG;
    uint64_t blackPawnRNG;
};


#endif //ZOBRISTHASH_H
