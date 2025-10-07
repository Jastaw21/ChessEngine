//
// Created by jacks on 06/10/2025.
//

#include <gtest/gtest.h>

#include "BoardManager/BoardManager.h"
#include "BoardManager/Move.h"
#include "BoardManager/Referee.h"


TEST(RefereeTests, ChecksAreSameInRef){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("7k/4R3/8/8/8/8/8/K5R1");
    auto whiteMove = createMove(WR, "e7e8");
    ASSERT_TRUE(manager.tryMove(whiteMove));
    EXPECT_TRUE(whiteMove.resultBits & CHECK);

    auto status = Referee::checkBoardStatus(
        *manager.getBitboards(),
        *manager.getMagicBitBoards(),
        manager.getCurrentTurn()
    );

    EXPECT_TRUE(BoardStatus::BLACK_CHECK & status);
}

TEST(RefereeTests, CheckmatesAreSameInRef){
    auto manager = BoardManager(WHITE);
    manager.getBitboards()->setFenPositionOnly("8/4N1pk/8/8/8/4R3/8/6K1"); // pre-check state

    auto checkMateMove = createMove(WR, "e3h3");
    ASSERT_TRUE(manager.tryMove(checkMateMove));

    auto status = Referee::checkBoardStatus(
        *manager.getBitboards(),
        *manager.getMagicBitBoards(),
        manager.getCurrentTurn()
    );

    manager.setCurrentTurn(BLACK);
    manager.getBitboards()->setFenPositionOnly("rnbqkbnr/pppp1ppp/4p3/8/6P1/5P2/PPPPP2P/RNBQKBNR");
    auto checkMateMove2 = createMove(BQ, "d8h4");

    ASSERT_TRUE(manager.tryMove(checkMateMove2));

    EXPECT_TRUE(checkMateMove2.resultBits & CHECK_MATE);

    status = Referee::checkBoardStatus(
        *manager.getBitboards(),
        *manager.getMagicBitBoards(),
        manager.getCurrentTurn()
    );

    EXPECT_TRUE(status &BoardStatus::WHITE_CHECKMATE);

    manager.setCurrentTurn(WHITE);
    manager.getBitboards()->setFenPositionOnly("r2qk2r/p1pp1pb1/bn2pQp1/3PN3/1p2P3/2N4p/PPPBBPPP/R3K2R");
    auto checkMateMove3 = createMove(WQ, "f6f7");
    ASSERT_TRUE(manager.tryMove(checkMateMove3));
    EXPECT_TRUE(checkMateMove3.resultBits & CHECK_MATE);
    EXPECT_TRUE(checkMateMove3.resultBits & CHECK);

    status = Referee::checkBoardStatus(
        *manager.getBitboards(),
        *manager.getMagicBitBoards(),
        manager.getCurrentTurn()
    );

    EXPECT_TRUE(status& BoardStatus::BLACK_CHECKMATE);
}

TEST(RefereeTests, IsCheckMate){
    auto manager = BoardManager();
    manager.setFullFen("5k1Q/4pp1p/p5pN/1p6/8/PP5P/2P5/7K b KQkq - 0 1"); // pre-check state

    auto status = Referee::checkBoardStatus(
        *manager.getBitboards(),
        *manager.getMagicBitBoards(),
        manager.getCurrentTurn()
    );

    EXPECT_TRUE(status & BoardStatus::BLACK_CHECKMATE);
}