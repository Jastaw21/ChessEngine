//
// Created by jacks on 16/06/2025.
//

#include "BoardManager/BitBoards.h"

#include <algorithm>
#include <bitset>
#include <iostream>

#include "Engine/Piece.h"


int rankAndFileToSquare(const int rank, const int file){
    if (rank < 1 || rank > 8) {
        std::cout << rank << "rr " << file << std::endl;
        throw std::invalid_argument("Rank must be between 1 and 8");
    }
    if (file < 1 || file > 8) {
        std::cout << rank << "ff " << file << std::endl;
        throw std::invalid_argument("File must be between 1 and 8");
    }
    const int rankStartSquare = (rank - 1) * 8;
    return rankStartSquare + (file - 1);
}

void squareToRankAndFile(const int square, int& rank, int& file){
    file = square % 8 + 1;
    rank = square / 8 + 1;
}

int squareToFile(const int square){ return square % 8 + 1; }
int squareToRank(const int square){ return square / 8 + 1; }


BitBoards::BitBoards(){ bitboards.fill(0ULL); }

void BitBoards::loadFEN(const std::string& fen){
    fen_ = fen;
    bitboards.fill(0ULL);
    // starting from a8, h8 is 63

    int rank = 8;
    int file = 1;
    size_t i = 0;

    while (i < fen.size() && fen[i] != ' ') {
        if (char c = fen[i]; c == '/') {
            rank--;
            file = 1;
        } // Move to the next rank
        else if (isdigit(c))
            file += c - '0'; // Skip that many empty squares
        else {
            if (auto it = pieceMap.find(c); it != pieceMap.end()) {
                const int square = rankAndFileToSquare(rank, file);
                bitboards[it->second] |= 1ULL << square;
            }
            file++;
        }
        i++;
    }
}

Bitboard BitBoards::getBitboard(const Piece& piece) const{ return bitboards[piece]; }


std::optional<Piece> BitBoards::getPiece(const int rank, const int file) const{
    const int toSquare = rankAndFileToSquare(rank, file);

    // Check if the destination square is empty

    for (int pieceIndex = 0; pieceIndex < PIECE_N; ++pieceIndex) {
        // ReSharper disable once CppTooWideScopeInitStatement
        auto piece = static_cast<Piece>(pieceIndex);
        if ((bitboards[piece] & 1ULL << toSquare) != 0) { return {piece}; }
    }
    return {};
}

std::optional<Piece> BitBoards::getPiece(const int square) const{
    int rank, file;
    squareToRankAndFile(square, rank, file);
    return getPiece(rank, file);
}

void BitBoards::setZero(const int rank, const int file){
    const int toSquare = rankAndFileToSquare(rank, file);
    for (int pieceIndex = 0; pieceIndex < 12; pieceIndex++) {
        const auto piece = static_cast<Piece>(pieceIndex);
        bitboards[piece] &= ~(1ULL << toSquare);
    }
}

void BitBoards::setOne(const Piece& piece, const int rank, const int file){
    const int toSquare = rankAndFileToSquare(rank, file);
    bitboards[piece] |= 1ULL << toSquare;
}


bool BitBoards::testBoard(const Bitboard inBoard) const{
    for (const auto& board: bitboards) {
        if ((board & inBoard) != 0)
            return true;
    }
    return false;
}

bool BitBoards::testSquare(const int square) const{
    for (const auto& board: bitboards) {
        if ((board & 1ULL << square) != 0)
            return true;
    }
    return false;
}

int BitBoards::countPiece(const Piece& pieceToSearch) const{
    if (bitboards[pieceToSearch] != 0) { return std::bitset<64>(bitboards[pieceToSearch]).count(); }
    return 0;
}

std::vector<Piece> BitBoards::getAttackingPieces(const Piece& piece){
    std::vector<Piece> attackingPieces;

    for (int i = 0; i < PIECE_N; ++i) {
        const auto pieceToSearch = static_cast<Piece>(i);
        if (pieceColours[pieceToSearch] == pieceColours[piece]) { attackingPieces.push_back(pieceToSearch); }
    }

    return attackingPieces;
}

Bitboard BitBoards::getOccupancy() const{
    Bitboard result = 0ULL;
    for (int i = 0; i < PIECE_N; ++i) {
        const auto pieceToSearch = static_cast<Piece>(i);
        result |= bitboards[pieceToSearch];
    }
    return result;
}

Bitboard BitBoards::getOccupancy(const Piece& piece) const{
    Bitboard result = 0ULL;
    for (int i = 0; i < PIECE_N; ++i) {
        const auto pieceToSearch = static_cast<Piece>(i);
        if (pieceToSearch == piece) { result |= bitboards[pieceToSearch]; }
    }
    return result;
}

Bitboard BitBoards::getOccupancy(const Colours& colour) const{
    Bitboard result = 0ULL;
    for (int i = 0; i < PIECE_N; ++i) {
        const auto pieceToSearch = static_cast<Piece>(i);
        if (pieceColours[pieceToSearch] == colour) { result |= bitboards[pieceToSearch]; }
    }
    return result;
}

std::string &BitBoards::toFEN(){
    fen_ = "";
    // rank by rank
    for (int rank = 8; rank >= 1; --rank) {
        // will be used to count empty squares
        int numEmpty = 0;

        // file by file
        for (int file = 1; file <= 8; ++file) {
            // square i.e. the bit we'll search
            const int square = rankAndFileToSquare(rank, file);

            bool isPieceHere = false;

            // check each piece
            for (const auto& [key, value]: pieceMap) {
                // if the bit is in the bitboard, and-ed with the square, then we have a piece here
                if (bitboards[value] & 1ULL << square) {
                    if (numEmpty > 0) {
                        fen_ += std::to_string(numEmpty);
                        numEmpty = 0;
                    }
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

