//
// Created by jacks on 24/06/2025.
//

#include "Engine/SearchTree.h"

void SearchTreeNode::addChild(SearchTreeNode* child){
    for (int i = 0; i < MAX_CHILDREN; i++) {
        if (children[i] == nullptr) {
            children[i] = child;
            child->parent = this;
            return;
        }
    }
}

size_t SearchTree::getNumberOfNodes() const{
    if (root_ == nullptr) return 0;
    return recursiveCount(root_) + 1;
}

size_t SearchTree::recursiveCount(const SearchTreeNode* node){
    size_t depth = 0;

    for (int child = 0; child < MAX_CHILDREN; child++) {
        if (node->children[child] != nullptr) {
            depth++;
            depth += recursiveCount(node->children[child]);
        }
    }

    return depth;
}