//
// Created by jacks on 16/06/2025.
//

#ifndef BITBOARDS_H
#define BITBOARDS_H

#include <string>
#include <cstdint>
#include <array>
#include <iostream>

#include "Engine/Piece.h"


constexpr uint64_t buildFileBoard(const char file) {
    const int index = tolower(file) - 'a';
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

    [[nodiscard]] uint64_t getBitboard(const Piece &piece) const;

    void printBitboard() const;

    std::string &toFEN();

    std::array<uint64_t, 8> files;
    std::array<uint64_t, 8> ranks;

    uint64_t &operator[](const Piece piece) { return bitboards[piece]; }

private:
    // array of PIECE_N length bitboards
    std::array<uint64_t, Piece::PIECE_N> bitboards;


    std::string fen_{};
};


#endif //BITBOARDS_H
