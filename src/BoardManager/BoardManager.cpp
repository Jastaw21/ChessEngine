//
// Created by jacks on 18/06/2025.
//

#include "BoardManager/BoardManager.h"
#include "BoardManager/BitBoards.h"

std::string Move::toUCI() const {
    std::string uci;
    const char fileFromChar = 'a' + fileFrom;
    uci += fileFromChar;
    uci += std::to_string(rankFrom);
    const char fileToChar = 'a' + fileTo;
    uci += fileToChar;
    uci += std::to_string(rankTo);

    return uci;
}

BoardManager::BoardManager() {
}

bool BoardManager::moveIsLegal(const Move &move) {
    return true;
}

bool BoardManager::moveIsPossible(const Move &move) {
    const auto moveString = move.toUCI();
    std::cout << "Move: " << moveString << std::endl;
    const bool fileInBounds = move.fileTo < 8 && move.fileTo >= 0;
    const bool rankInBounds = move.rankTo < 8 && move.rankTo >= 0;

    const uint64_t toSquare = move.rankTo * 8 + move.fileTo;

    bool squareEmpty = true;
    for (int pieceIndex = 0; pieceIndex < Piece::PIECE_N; ++pieceIndex) {
        auto piece = static_cast<Piece>(pieceIndex);
        if ((bitboards[piece] & (1ULL << toSquare)) != 0)
            squareEmpty = false;
    }

    if (!(fileInBounds && rankInBounds && squareEmpty)) {
        std::cout << "Move is not possible" << std::endl;
        return false;
    }


    std::cout << "Move is possible" << std::endl;


    return fileInBounds && rankInBounds && squareEmpty;
}
