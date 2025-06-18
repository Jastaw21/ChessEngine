//
// Created by jacks on 18/06/2025.
//

#ifndef BOARDMANAGER_H
#define BOARDMANAGER_H

#include "BitBoards.h"
#include "Engine/Piece.h"

class BitBoards;

struct Move {
    Piece piece;
    int rankFrom;
    int fileFrom;
    int rankTo;
    int fileTo;

    std::string toUCI() const;
};


class BoardManager {
public:

    explicit BoardManager();

    BitBoards *getBitboards(){ return &bitboards; }

    bool moveIsLegal(const Move &move);
    bool moveIsPossible(const Move &move);
    bool checkMove(const Move &move){ return (moveIsLegal(move) && moveIsPossible(move)); }

private:

    BitBoards bitboards{};
};


#endif //BOARDMANAGER_H
