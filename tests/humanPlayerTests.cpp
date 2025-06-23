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

        gui = ChessGui(&whitePlayer, &blackPlayer);

        gui.getBoardManager()->getBitboards()->loadFEN(Fen::STARTING_FEN);
    }

    HumanPlayer whitePlayer{WHITE};
    HumanPlayer blackPlayer{BLACK};
    ChessGui gui{nullptr, nullptr}; // Or default ctor if supported
};


TEST_F(HumanPlayerInteractionTests, InitTests){ EXPECT_TRUE(gui.wasInit()); }

TEST_F(HumanPlayerInteractionTests, ClickingSquareCollectsAndReleasesPiece){
    // the starting point is -1 set
    if (const auto player = gui.getWhitePlayerAsHuman()) { EXPECT_TRUE(player->getHeldPiece() == -1); }

    gui.addMouseClick(710, 710);

    if (const auto player = gui.getWhitePlayerAsHuman()) { EXPECT_EQ(player->getHeldPiece(), 7); }

    gui.addMouseRelease(610, 610);

    // this should be a failed move as occupied, then reset it to -1
    if (const auto player = gui.getWhitePlayerAsHuman()) { EXPECT_EQ(player->getHeldPiece(), -1); }
}

TEST_F(HumanPlayerInteractionTests, CantGoOutOfTurn){
    // the starting point is -1 set
    if (const auto player = gui.getBlackPlayerAsHuman()) { EXPECT_TRUE(player->getHeldPiece() == -1); }

    gui.addMouseClick(15, 150);

    if (const auto player = gui.getBlackPlayerAsHuman()) { EXPECT_EQ(player->getHeldPiece(), -1); }

    gui.addMouseRelease(250, 250);

    // this should be a failed move as occupied, then reset it to -1
    if (const auto player = gui.getBlackPlayerAsHuman()) { EXPECT_EQ(player->getHeldPiece(), -1); }
}

