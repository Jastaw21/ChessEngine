//
// Created by jacks on 24/06/2025.
//

#include <gtest/gtest.h>
#include "Engine/SearchTree.h"


TEST(SearchTreeBasics, GetNumNodesWorksOnePly){
    auto tree = SearchTree();

    tree.setRoot(new SearchTreeNode());
    EXPECT_EQ(tree.getNumberOfNodes(), 1);
}

TEST(SearchTreeBasics, GetNumNodesWorksTwoPly){
    auto tree = SearchTree();

    tree.setRoot(new SearchTreeNode());
    for (int i = 0; i < MAX_CHILDREN; i++) { tree.getRoot()->addChild(new SearchTreeNode()); }

    EXPECT_EQ(tree.getNumberOfNodes(), 5);
}

TEST(SearchTreeBasics, GetNumNodesWorksThreePly){
    auto tree = SearchTree();

    tree.setRoot(new SearchTreeNode());
    for (int i = 0; i < MAX_CHILDREN; i++) {
        tree.getRoot()->addChild(new SearchTreeNode());
        for (int j = 0; j < MAX_CHILDREN; j++) { tree.getRoot()->children[i]->addChild(new SearchTreeNode()); }
    }

    EXPECT_EQ(tree.getNumberOfNodes(), 21);
}