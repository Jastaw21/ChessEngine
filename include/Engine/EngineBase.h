//
// Created by jacks on 16/06/2025.
//

#ifndef ENGINEBASE_H
#define ENGINEBASE_H
#include "ChessPlayer.h"
#include "SearchTree.h"


class EngineBase : public ChessPlayer {
public:

    explicit EngineBase(Colours colour = WHITE);

    virtual Move makeMove() override;

private:
    SearchTree searchTree = SearchTree();
};


#endif //ENGINEBASE_H