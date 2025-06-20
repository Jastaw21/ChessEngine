//
// Created by jacks on 18/06/2025.
//

#ifndef BOARDMANAGER_H
#define BOARDMANAGER_H

#include "BitBoards.h"
#include "Engine/Piece.h"

class BitBoards;

enum MoveResult {
    MOVE_TO_EMPTY_SQUARE,
    ILLEGAL_MOVE,
    PIECE_CAPTURE
};

struct Move {
    Piece piece;
    int rankFrom;
    int fileFrom;
    int rankTo;
    int fileTo;

    MoveResult result;

    std::string toUCI() const;
};


class BoardManager {
public:

    explicit BoardManager();

    BitBoards *getBitboards(){ return &bitboards; }
    [[nodiscard]] bool prelimCheckMove(Move& move) const;
    bool tryMove(Move& move);

private:

    static bool moveIsLegal(const Move& move);
    static bool moveIsPossible(const Move& move);
    [[nodiscard]] bool moveDestinationIsEmpty(const Move& move) const;
    [[nodiscard]] bool moveDestinationIsOccupiedWithOpponent(const Move& move) const;
    bool moveDestOccupiedByColour(const std::string& testColour, const Move& move) const;

    void makeMove(const Move& move);

private:

    BitBoards bitboards{};
};


#endif //BOARDMANAGER_H
