//
// Created by jacks on 06/11/2025.
//

#include <gtest/gtest.h>
#include "GUI/OpeningBook.h"


TEST(OpeningBookTests, GetRandomFen){
    auto openingBook = OpeningBook();

    auto fen = openingBook.GetRandomFen();
    EXPECT_TRUE(fen.size() > 0);
}