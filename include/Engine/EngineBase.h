//
// Created by jacks on 16/06/2025.
//

#ifndef ENGINEBASE_H
#define ENGINEBASE_H
#include "ChessPlayer.h"
#include "SearchTree.h"


struct PerftResults {
    uint64_t nodes;
    uint64_t captures;
    uint64_t enPassant;
    uint64_t castling;

    PerftResults operator+(const PerftResults& rhs) const{
        return PerftResults{
                    .nodes = this->nodes + rhs.nodes, .captures = this->captures + rhs.captures,
                    .enPassant = this->enPassant + rhs.enPassant, .castling = this->castling + rhs.castling
                };
    }

    PerftResults &operator+=(const PerftResults& rhs){
        nodes += rhs.nodes;
        captures += rhs.captures;
        enPassant += rhs.enPassant;
        castling += rhs.castling;
        return *this;
    }
};


class EngineBase : public ChessPlayer {
public:

    explicit EngineBase(Colours colour = WHITE);

    virtual Move makeMove() override;

private:

    SearchTree searchTree = SearchTree();
};


#endif //ENGINEBASE_H