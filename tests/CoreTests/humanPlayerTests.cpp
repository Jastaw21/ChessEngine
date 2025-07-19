//
// Created by jacks on 23/06/2025.
//

#include <gtest/gtest.h>

#include "HumanPlayer.h"
#include "GUI/gui.h"
#include "Utility/Fen.h"

class HumanPlayerInteractionTests : public testing::Test {
protected:

    virtual void SetUp() override{
        whitePlayer = HumanPlayer(WHITE);
        blackPlayer = HumanPlayer(BLACK);

        gui.getMatchManager()->getBitboards()->loadFEN(Fen::STARTING_FEN);
    }

    HumanPlayer whitePlayer{WHITE};
    HumanPlayer blackPlayer{BLACK};
    ChessGui gui{&whitePlayer, &blackPlayer};
};


TEST_F(HumanPlayerInteractionTests, InitTests){ EXPECT_TRUE(gui.wasInit()); }


