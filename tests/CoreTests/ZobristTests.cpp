//
// Created by jacks on 01/09/2025.
//

#include  <gtest/gtest.h>
#include "../../include/Engine/ZobristHash.h"
#include "BoardManager/BoardManager.h"

TEST(Zobrist, BasicInitAndUpdateState){
    auto zob = ZobristHash(Fen::FULL_STARTING_FEN);
    const uint64_t initHash = zob.getHash();
    EXPECT_GT(zob.getHash(), 0);

    const auto move = createMove(WP, "a2a4");
    zob.addMove(move);
    const uint64_t followingHash = zob.getHash();

    // the hash should change
    EXPECT_NE(initHash, followingHash);
}

TEST(Zobrist, CaptureMoveUpdatesState){
    std::string startingFen = "rnbqkbnr/pp1ppppp/8/2p5/3P4/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 1";
    auto zob = ZobristHash(startingFen);

    // the white pawn captures on c5
    auto move = createMove(WP, "d4c5");

    // put the move through the manager to get the move result.
    auto manager = BoardManager();
    manager.setFullFen(startingFen);
    manager.tryMove(move);
    zob.addMove(move);

    // this should result in the following fen:
    std::string hardcodedFen = "rnbqkbnr/pp1ppppp/8/2P5/8/8/PPP1PPPP/RNBQKBNR b KQkq - 0 1";

    // the original hash should track that
    auto zobFromScratch = ZobristHash(hardcodedFen);
    EXPECT_EQ(zob.getHash(), zobFromScratch.getHash());
}

TEST(Zobrist, UndoMoveRestoresState){
    std::string startingFen = Fen::FULL_STARTING_FEN;
    auto zob = ZobristHash(startingFen);
    auto initHash = zob.getHash();

    // basic pawn push
    auto move = createMove(WP, "a2a3");

    // put the move through the manager to get the move result.
    auto manager = BoardManager();
    manager.setFullFen(startingFen);
    manager.tryMove(move);

    // and the zob
    zob.addMove(move);
    EXPECT_NE(zob.getHash(), initHash); // this should change the hash
    zob.undoMove(move);

    EXPECT_EQ(zob.getHash(), initHash); // restored state
}

TEST(Zobrist, UndoCaptureRestoresState){
    std::string startingFen = "rnbqkbnr/pp1ppppp/8/2p5/3P4/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 1";
    auto zob = ZobristHash(startingFen);
    auto initHash = zob.getHash();

    // the white pawn captures on c5
    auto move = createMove(WP, "d4c5");

    // put the move through the manager to get the move result.
    auto manager = BoardManager();
    manager.setFullFen(startingFen);
    manager.tryMove(move);
    zob.addMove(move);

    // Capture changes
    EXPECT_NE(initHash, zob.getHash());

    // undo it, we should be back to the start
    zob.undoMove(move);
    EXPECT_EQ(initHash, zob.getHash());
}

TEST(Zobrist, ColourMakesADifference){
    auto whiteZob = ZobristHash(Fen::FULL_STARTING_FEN);
    auto blackZob = ZobristHash("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1");

    EXPECT_NE(whiteZob.getHash(), blackZob.getHash());
}

TEST(Zobrist, EnPassantCorrectlyUpdatesState){
    std::string startingFen = Fen::FULL_STARTING_FEN;
    auto zob = ZobristHash(startingFen);

    // en passant move sequence
    auto whiteMove1 = createMove(WP, "e2e4");
    auto blackMove1 = createMove(BP, "c7c6");
    auto whiteMove2 = createMove(WP, "e4e5");
    auto blackMove2 = createMove(BP, "f7f5");

    auto testEnPassant = createMove(WP, "e5f6");

    auto manager = BoardManager();
    manager.setFullFen(startingFen);

    // ... and the manager
    ASSERT_TRUE(manager.tryMove(whiteMove1));
    ASSERT_TRUE(manager.tryMove(blackMove1));
    ASSERT_TRUE(manager.tryMove(whiteMove2));
    ASSERT_TRUE(manager.tryMove(blackMove2));
    ASSERT_TRUE(manager.tryMove(testEnPassant));

    // put the moves through the zob
    zob.addMove(whiteMove1);
    zob.addMove(blackMove1);
    zob.addMove(whiteMove2);
    zob.addMove(blackMove2);
    zob.addMove(testEnPassant);

    // this should result in the following fen:
    std::string hardcodedFen = "rnbqkbnr/pp1pp1pp/2p2P2/8/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1";

    // the original hash should track that
    auto zobFromScratch = ZobristHash(hardcodedFen);
    EXPECT_EQ(zob.getHash(), zobFromScratch.getHash());

    auto zobFromFen = ZobristHash(manager.getFullFen());
    std::cout << zobFromFen.getHash();
}

TEST(Zobrist, EnPassantCanBeUndone){
    std::string startingFen = Fen::FULL_STARTING_FEN;
    auto zob = ZobristHash(startingFen);

    // en passant move sequence
    auto whiteMove1 = createMove(WP, "e2e4");
    auto blackMove1 = createMove(BP, "c7c6");
    auto whiteMove2 = createMove(WP, "e4e5");
    auto blackMove2 = createMove(BP, "f7f5");

    auto testEnPassant = createMove(WP, "e5f6");

    auto manager = BoardManager();
    manager.setFullFen(startingFen);

    // ... and the manager
    ASSERT_TRUE(manager.tryMove(whiteMove1));
    ASSERT_TRUE(manager.tryMove(blackMove1));
    ASSERT_TRUE(manager.tryMove(whiteMove2));
    ASSERT_TRUE(manager.tryMove(blackMove2));
    ASSERT_TRUE(manager.tryMove(testEnPassant));

    // put the moves through the zob
    zob.addMove(whiteMove1);
    zob.addMove(blackMove1);
    zob.addMove(whiteMove2);
    zob.addMove(blackMove2);
    auto preEnPassantHash = zob.getHash();
    zob.addMove(testEnPassant);

    manager.undoMove(testEnPassant);
    zob.undoMove(testEnPassant);
    EXPECT_EQ(preEnPassantHash, zob.getHash());
}

TEST(Zobrist, PromotionMovesCorrectly){
    // the a pawn can promote
    auto startingFen = "8/P6k/8/8/7r/8/3B4/5K2 w - - 0 1";

    auto zob = ZobristHash(startingFen);
    auto manager = BoardManager();
    manager.setFullFen(startingFen);

    auto move = createMove(WP, "a7a8Q");

    ASSERT_TRUE(manager.tryMove(move));

    zob.addMove(move);

    // this should be the fen after promotion
    auto targetFen = "Q7/7k/8/8/7r/8/3B4/5K2 b - - 0 1";
    auto zobFromScratch = ZobristHash(targetFen);
    EXPECT_EQ(zobFromScratch.getHash(), zob.getHash());
}

TEST(Zobrist, PromotionUndoesCorrectly){
    // the a pawn can promote
    auto startingFen = "8/P6k/8/8/7r/8/3B4/5K2 w - - 0 1";

    auto zob = ZobristHash(startingFen);
    auto manager = BoardManager();
    manager.setFullFen(startingFen);

    auto move = createMove(WP, "a7a8Q");

    auto initialHash = zob.getHash();

    ASSERT_TRUE(manager.tryMove(move));

    zob.addMove(move);

    // this should be the fen after promotion
    auto targetFen = "Q7/7k/8/8/7r/8/3B4/5K2 b - - 0 1";
    auto zobFromScratch = ZobristHash(targetFen);
    ASSERT_EQ(zobFromScratch.getHash(), zob.getHash());

    zob.undoMove(move);

    EXPECT_EQ(initialHash, zob.getHash());
}

TEST(Zobrist, CastlingMovesCorrect){
    auto startingFen = "rnbqkbnr/ppp1pp1p/8/6p1/3p2P1/5N1B/PPPPPP1P/RNBQK2R w KQkq - 0 4";

    auto zob = ZobristHash(startingFen);
    auto manager = BoardManager();
    manager.setFullFen(startingFen);

    auto move = createMove(WK, "e1g1");
    ASSERT_TRUE(manager.tryMove(move));
    zob.addMove(move);

    // this should be the fen after castling
    auto targetFen = "rnbqkbnr/ppp1pp1p/8/6p1/3p2P1/5N1B/PPPPPP1P/RNBQ1RK1 b kq - 1 4";
    auto zobFromScratch = ZobristHash(targetFen);
    EXPECT_EQ(zobFromScratch.getHash(), zob.getHash());
}

TEST(Zobrist, UndoCastlingMoves){
    auto startingFen = "rnbqkbnr/ppp1pp1p/8/6p1/3p2P1/5N1B/PPPPPP1P/RNBQK2R w KQkq - 0 4";

    auto zob = ZobristHash(startingFen);
    auto manager = BoardManager();
    manager.setFullFen(startingFen);

    auto move = createMove(WK, "e1g1");

    auto preMoveHash = zob.getHash();

    ASSERT_TRUE(manager.tryMove(move));
    zob.addMove(move);

    // this should be the fen after castling
    auto targetFen = "rnbqkbnr/ppp1pp1p/8/6p1/3p2P1/5N1B/PPPPPP1P/RNBQ1RK1 b kq - 1 4";
    auto zobFromScratch = ZobristHash(targetFen);
    EXPECT_EQ(zobFromScratch.getHash(), zob.getHash());

    manager.undoMove(move);
    zob.undoMove(move);

    EXPECT_EQ(zob.getHash(), preMoveHash);
}