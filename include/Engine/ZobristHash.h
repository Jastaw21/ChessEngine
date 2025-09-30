//
// Created by jacks on 04/08/2025.
//

#ifndef ZOBRISTHASH_H
#define ZOBRISTHASH_H
#include <array>
#include <random>


#include "Utility/Fen.h"
struct Move;

struct RepetitionTableEntry {
    uint64_t hash;
    int count;
};


class ZobristHash {
public:

    ZobristHash(){ initZobrist(); }
    void initializeHashFromFen(FenString fenString);
    explicit ZobristHash(const FenString& fenString);

    void setFen(const FenString& fenString);
    const uint64_t& getHash() const{ return hashValue; }
    void addMove(const Move& move);
    void undoMove(const Move& move);

private:

    std::array<uint64_t, 64>& getArray(char pieceIndex);
    void fillRandomArray(std::array<uint64_t, 64>& array);
    void initZobrist();

    std::mt19937_64 rng;
    uint64_t seed = 123999;
    uint64_t hashValue = 0;

    std::array<uint64_t, 64> whitePawn;
    std::array<uint64_t, 64> whiteKnight;
    std::array<uint64_t, 64> whiteBishop;
    std::array<uint64_t, 64> whiteRook;
    std::array<uint64_t, 64> whiteQueen;
    std::array<uint64_t, 64> whiteKing;


    std::array<uint64_t, 64> blackPawn;
    std::array<uint64_t, 64> blackKnight;
    std::array<uint64_t, 64> blackBishop;
    std::array<uint64_t, 64> blackRook;
    std::array<uint64_t, 64> blackQueen;
    std::array<uint64_t, 64> blackKing;


    uint64_t blackToMove;
};


#endif //ZOBRISTHASH_H