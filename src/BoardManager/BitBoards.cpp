//
// Created by jacks on 16/06/2025.
//

#include "BoardManager/BitBoards.h"

#include <algorithm>
#include <iostream>
#include <numeric>

#include "Engine/Piece.h"
#include <functional>


/**
 * Converts a given rank and file in a chessboard to the corresponding square index.
 *
 * @param rank The rank of the chessboard (1-based, valid values are 1 through 8).
 * @param file The file of the chessboard (1-based, valid values are 1 through 8).
 * @return The corresponding zero-based square index (0 to 63).
 * @throws std::invalid_argument If the rank is not in the range [1, 8].
 * @throws std::invalid_argument If the file is not in the range [1, 8].
 */
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

RankAndFile squareToRankAndFileStruct(const int square){
    RankAndFile result;
    squareToRankAndFile(square, result.rank, result.file);
    return result;
}

int squareToFile(const int square){ return square % 8 + 1; }
int squareToRank(const int square){ return square / 8 + 1; }


int getLowestSetBit(const Bitboard& inBoard){ return std::countr_zero(inBoard); }

int popLowestSetBit(Bitboard& inBoard){
    const int lsb = getLowestSetBit(inBoard);
    inBoard &= ~(1ULL << lsb);
    return lsb;
}

BitBoards::BitBoards(){ bitboards.fill(0ULL); }

void BitBoards::setFenPositionOnly(const FenString& fen){
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
            if (auto it = CHAR_TO_PIECE_MAP.find(c); it != CHAR_TO_PIECE_MAP.end()) {
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
    bitboards[piece] |= 1ULL << rankAndFileToSquare(rank, file);
}


bool BitBoards::testBoard(const Bitboard inBoard) const{
    const bool anyBoardMatches = std::ranges::any_of(bitboards, [&](const Bitboard& board) {
        return (board & inBoard) != 0;
    });
    return anyBoardMatches;
}

bool BitBoards::testSquare(const int square) const{
    const bool anyBoardMatches = std::ranges::any_of(bitboards, [&](const Bitboard& board) {
        return (board & (1ULL << square)) != 0;
    });
    return anyBoardMatches;
}

int BitBoards::countPiece(const Piece& pieceToSearch) const{ return std::popcount(bitboards[pieceToSearch]); }

Bitboard BitBoards::getOccupancy() const{
    const auto accumulated = std::accumulate(bitboards.begin(), bitboards.end(), 0ULL, std::bit_or<>());
    return accumulated;
}

Bitboard BitBoards::getOccupancy(const Piece& piece) const{ return bitboards[piece]; }

Bitboard BitBoards::getOccupancy(const Colours& colour) const{
    Bitboard result = 0ULL;
    for (const auto& piece: filteredPieces[colour]) { result |= bitboards[piece]; }
    return result;
}

std::string& BitBoards::toFEN(){
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
            for (const auto& [key, value]: CHAR_TO_PIECE_MAP) {
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