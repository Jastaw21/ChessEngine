//
// Created by jacks on 06/07/2025.
//

#include  <gtest/gtest.h>
#include "BoardManager/Rules.h"

TEST(rulesHeader, rules){
    const auto rules = Rules();
    auto result = rules.rankAttacks;

    std::cout << 1 << std::endl;
}