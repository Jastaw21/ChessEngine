//
// Created by jacks on 06/10/2025.
//

#include "Utility/ChessUtility.h"

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