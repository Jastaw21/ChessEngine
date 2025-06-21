//
// Created by jacks on 16/06/2025.
//

#include "BoardManager/BitBoards.h"

#include <algorithm>
#include <bitset>
#include <iostream>

#include "Engine/Piece.h"


int rankAndFileToSquare(const int rank, const int file){
    if (rank < 1 || rank > 8) { throw std::invalid_argument("Rank must be between 1 and 8"); }
    if (file < 1 || file > 8) { throw std::invalid_argument("File must be between 1 and 8"); }
    const int rankStartSquare = (rank - 1) * 8;
    return rankStartSquare + (file - 1);
}

void squareToRankAndFile(const int square, int& rank, int& file){
    file = square % 8 + 1;
    rank = square / 8 + 1;
}

BitBoards::BitBoards(){ bitboards.fill(0ULL); }

void BitBoards::loadFEN(const std::string& fen){
    fen_ = fen;
    bitboards.fill(0ULL);
    // starting from a8, h8 is 63

    int rank = 8;
    int file = 1;
    size_t i = 0;

    while (i < fen.size() && fen[i] != ' ') {
        char c = fen[i];

        if (c == '/') {
            rank--;
            file = 1;
        } // Move to the next rank
        else if (isdigit(c))
            file += (c - '0'); // Skip that many empty squares
        else {
            auto it = pieceMap.find(c);
            if (it != pieceMap.end()) {
                const int square = rankAndFileToSquare(rank, file);
                bitboards[it->second] |= (1ULL << square);
            }
            file++;
        }
        i++;
    }
}

uint64_t BitBoards::getBitboard(const Piece& piece) const{ return bitboards[piece]; }


std::optional<Piece> BitBoards::getPiece(const int rank, const int file) const{
    const int toSquare = rankAndFileToSquare(rank, file);

    // Check if the destination square is empty

    for (int pieceIndex = 0; pieceIndex < Piece::PIECE_N; ++pieceIndex) {
        auto piece = static_cast<Piece>(pieceIndex);
        if ((bitboards[piece] & (1ULL << toSquare)) != 0) { return {piece}; }
    }
    return {};
}

void BitBoards::setZero(const int rank, const int file){
    const int toSquare = rankAndFileToSquare(rank, file);
    for (int pieceIndex = 0; pieceIndex < 12; pieceIndex++) {
        auto piece = static_cast<Piece>(pieceIndex);
        bitboards[piece] &= ~(1ULL << toSquare);
    }
}

bool BitBoards::test(const uint64_t inBoard) const{
    for (const auto& board: bitboards) {
        if ((board & inBoard) != 0)
            return true;
    }
    return false;
}

std::string &BitBoards::toFEN(){
    fen_ = "";
    // rank by rank
    for (int rank = 8; rank > 0; --rank) {
        // will be used to count empty squares
        int numEmpty = 0;

        // file by file
        for (int file = 1; file <= 8; ++file) {
            // square i.e., the bit we'll search
            const int square = rankAndFileToSquare(rank, file);

            bool isPieceHere = false;

            // check each piece
            for (const auto& [key, value]: pieceMap) {
                // if the bit is in the bitboard, and-ed with the square, then we have a piece here
                if (bitboards[value] & (1ULL << square)) {
                    isPieceHere = true;
                    fen_ += key;
                    break;
                }
            }
            if (!isPieceHere)
                numEmpty++;
        }

        if (numEmpty > 0) {
            fen_ += std::to_string(numEmpty);
            numEmpty = 0;
        }
        if (rank != 1)
            fen_ += '/';
    }

    return fen_;
}
