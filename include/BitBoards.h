//
// Created by jacks on 16/06/2025.
//

#ifndef BITBOARDS_H
#define BITBOARDS_H

#include <string>
#include <unordered_map>
#include <cstdint>
#include <array>
#include <iostream>


#include "Fen.h"


constexpr uint64_t buildFileBoard(char file) {
    int index = tolower(file) - 'a';
    uint64_t result = 0ULL;
    for (int i = 0; i < 8; ++i) {
        result |= 1ULL << (index + 8 * i);
    }
    return result;
}
inline constexpr uint64_t buildRankBoard(const int rank) {
    uint64_t result = 0ULL;
    const int base = (rank - 1) * 8;

    for (int i = 0; i < 8; ++i) {
        result |= 1ULL << (base + i);
    }
    return result;
}


class BitBoards {
public:
    BitBoards();

    void loadFEN(const std::string &fen);

    uint64_t getBitboard(Fen::Piece piece) const;

    void printBitboard() const;
    std::string_view toFEN() const;

    std::array<uint64_t, 8> files;
    std::array<uint64_t, 8> ranks;



private:
    // array of PIECE_N length bitboards
    std::array<uint64_t, Fen::Piece::PIECE_N> bitboards;


    std::unordered_map<char, Fen::Piece> pieceMap;

    void initPieceMap();
};


#endif //BITBOARDS_H
