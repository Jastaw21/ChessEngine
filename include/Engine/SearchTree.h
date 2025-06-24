//
// Created by jacks on 24/06/2025.
//

#ifndef SEARCHTREE_H
#define SEARCHTREE_H

#include <cstddef>
#include <cstdint>

#include "Piece.h"
#include "BoardManager/BitBoards.h"

constexpr int MAX_DEPTH = 10;
constexpr int MAX_CHILDREN = 4;

struct MoveState {
    int score;
    int depth;
    bool isMaximising;
};

struct SearchTreeNode {
    SearchTreeNode* parent = nullptr;
    SearchTreeNode* children[MAX_CHILDREN]{nullptr};
    MoveState* moveState = nullptr;

    void addChild(SearchTreeNode* child);
};

class SearchTree {
public:

    SearchTreeNode *getRoot() const{ return root_; }

    void setRoot(SearchTreeNode* root){ root_ = root; }
    size_t getNumberOfNodes() const;

private:

    SearchTreeNode* root_ = nullptr;

    static size_t recursiveCount(const SearchTreeNode* node);
};

class Position {
public:

    explicit Position(const BitBoards& boards, const Colours& move_colour)
        : bitboards(std::move(boards)), moveColour(move_colour){}


    BitBoards &getBitboards(){ return bitboards; }

private:

    BitBoards bitboards;
    Colours moveColour;
};


#endif //SEARCHTREE_H