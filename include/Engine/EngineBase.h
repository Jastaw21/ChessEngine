//
// Created by jacks on 16/06/2025.
//

#ifndef ENGINEBASE_H
#define ENGINEBASE_H
#include "ChessPlayer.h"

class EngineBase : public ChessPlayer {
public:

    explicit EngineBase(Colours colour = WHITE);
    virtual void evaluate() = 0;
    virtual void search() = 0;

    virtual Move makeMove() override;


};


#endif //ENGINEBASE_H
