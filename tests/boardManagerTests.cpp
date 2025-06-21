//
// Created by jacks on 21/06/2025.
//

#include <gtest/gtest.h>
#include "BoardManager/BoardManager.h"
#include "Utility/Fen.h"

TEST(BoardManagerLegality, WhitePawnLegalityCorrect){
    auto manager = BoardManager();
    // load a board with pawn in d4
    manager.getBitboards()->loadFEN("8/8/8/8/3P4/8/8/8");

    // white pawn can only move north, move should fail in all previous cases
    Move southMove{.piece = Piece::WP, .rankFrom = 4, .fileFrom = 4, .rankTo = 3, .fileTo = 4};
    Move eastMove{.piece = Piece::WP, .rankFrom = 4, .fileFrom = 4, .rankTo = 4, .fileTo = 5};
    Move westMove{.piece = Piece::WP, .rankFrom = 4, .fileFrom = 4, .rankTo = 4, .fileTo = 3};
    Move northMove{.piece = Piece::WP, .rankFrom = 4, .fileFrom = 4, .rankTo = 5, .fileTo = 4};

    EXPECT_FALSE(manager.checkMove(southMove));
    EXPECT_FALSE(manager.checkMove(eastMove));
    EXPECT_FALSE(manager.checkMove(westMove));

    EXPECT_TRUE(manager.checkMove(northMove));

    EXPECT_EQ(southMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    EXPECT_EQ(eastMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    EXPECT_EQ(westMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);

    EXPECT_EQ(northMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    // if not on the first rank, move only 1 rank.
    Move northMoveTwoSquares{.piece = Piece::WP, .rankFrom = 4, .fileFrom = 4, .rankTo = 6, .fileTo = 4};
    EXPECT_FALSE(manager.checkMove(northMoveTwoSquares));

    manager.getBitboards()->loadFEN("8/8/8/8/8/8/3P4/8");

    // if on the starting pawn rank, move 1 or 2 ranks.
    Move northMoveFromSecondRankOneSquare{.piece = Piece::WP, .rankFrom = 2, .fileFrom = 4, .rankTo = 3, .fileTo = 4};
    Move northMoveFromSecondRankTwoSquares{.piece = Piece::WP, .rankFrom = 2, .fileFrom = 4, .rankTo = 4, .fileTo = 4};
    Move northMoveFromSecondRankThreeSquares{
                .piece = Piece::WP, .rankFrom = 2, .fileFrom = 4, .rankTo = 5, .fileTo = 4
            };
    EXPECT_TRUE(manager.checkMove(northMoveFromSecondRankOneSquare));
    EXPECT_TRUE(manager.checkMove(northMoveFromSecondRankTwoSquares));
    EXPECT_FALSE(manager.checkMove(northMoveFromSecondRankThreeSquares));
}

TEST(BoardManagerLegality, BlackPawnLegalityCorrect){
    auto manager = BoardManager();
    // load a board with pawn in d4
    manager.getBitboards()->loadFEN("8/8/8/8/3p4/8/8/8");

    // black pawn can only move south, move should fail in all other cases
    Move southMove{.piece = Piece::BP, .rankFrom = 4, .fileFrom = 4, .rankTo = 3, .fileTo = 4};
    Move eastMove{.piece = Piece::BP, .rankFrom = 4, .fileFrom = 4, .rankTo = 4, .fileTo = 5};
    Move westMove{.piece = Piece::BP, .rankFrom = 4, .fileFrom = 4, .rankTo = 4, .fileTo = 3};
    Move northMove{.piece = Piece::BP, .rankFrom = 4, .fileFrom = 4, .rankTo = 5, .fileTo = 4};

    EXPECT_FALSE(manager.checkMove(northMove));
    EXPECT_FALSE(manager.checkMove(eastMove));
    EXPECT_FALSE(manager.checkMove(westMove));

    EXPECT_TRUE(manager.checkMove(southMove));

    // if not on the first rank, move only 1 rank.
    Move southTwoSquares{.piece = Piece::BP, .rankFrom = 4, .fileFrom = 4, .rankTo = 2, .fileTo = 4};
    EXPECT_FALSE(manager.checkMove(southTwoSquares));

    manager.getBitboards()->loadFEN("8/3p4/8/8/8/8/8/8");

    // if on the starting pawn rank, move 1 or 2 ranks.
    Move southMoveFromSecondRankOneSquare{.piece = Piece::BP, .rankFrom = 7, .fileFrom = 4, .rankTo = 6, .fileTo = 4};
    Move northMoveFromSecondRankTwoSquares{.piece = Piece::BP, .rankFrom = 7, .fileFrom = 4, .rankTo = 5, .fileTo = 4};
    Move southMoveFromSecondRankThreeSquares{
                .piece = Piece::BP, .rankFrom = 7, .fileFrom = 4, .rankTo = 4, .fileTo = 4
            };
    EXPECT_TRUE(manager.checkMove(southMoveFromSecondRankOneSquare));
    EXPECT_TRUE(manager.checkMove(northMoveFromSecondRankTwoSquares));
    EXPECT_FALSE(manager.checkMove(southMoveFromSecondRankThreeSquares));
}

TEST(BoardManagerLegality, RookLegalityCorrect){
    auto manager = BoardManager();
    // load a board with white rook in a1
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/R7");

    // cant move diagonally
    for (int i = 1; i < 8; i++) {
        Move northEastMove{.piece = Piece::WR, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = i + 1};
        EXPECT_FALSE(manager.checkMove(northEastMove));
        EXPECT_EQ(northEastMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    }

    // can move vertically
    for (int i = 1; i < 8; i++) {
        Move verticalMove{.piece = Piece::WR, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = 1};
        EXPECT_TRUE(manager.checkMove(verticalMove));
        EXPECT_EQ(verticalMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
    }

    // can move horizontally
    for (int i = 1; i < 8; i++) {
        Move horizMove{.piece = Piece::WR, .rankFrom = 1, .fileFrom = 1, .rankTo = 1, .fileTo = i + 1};
        EXPECT_TRUE(manager.checkMove(horizMove));
        EXPECT_EQ(horizMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
    }
}

TEST(BoardManagerLegality, QueenLegalityCorrect){
    auto manager = BoardManager();
    // load a board with black queen in a1
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/q7");

    // can move diagonally
    for (int i = 1; i < 8; i++) {
        Move northEastMove{.piece = Piece::WQ, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = i + 1};
        EXPECT_TRUE(manager.checkMove(northEastMove));
        EXPECT_EQ(northEastMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
    }

    // can move vertically
    for (int i = 1; i < 8; i++) {
        Move verticalMove{.piece = Piece::WQ, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = 1};
        EXPECT_TRUE(manager.checkMove(verticalMove));
        EXPECT_EQ(verticalMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
    }

    // can move horizontally
    for (int i = 1; i < 8; i++) {
        Move horizMove{.piece = Piece::WQ, .rankFrom = 1, .fileFrom = 1, .rankTo = 1, .fileTo = i + 1};
        EXPECT_TRUE(manager.checkMove(horizMove));
        EXPECT_EQ(horizMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
    }
}

TEST(BoardManagerLegality, KingLegalityCorrect){
    auto manager = BoardManager();
    // load a board with black king in a1
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/k7");

    // can diagonally move one square only
    for (int i = 1; i < 8; i++) {
        Move northEastMove{.piece = Piece::BK, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = i + 1};
        if (i == 1) {
            EXPECT_TRUE(manager.checkMove(northEastMove));
            EXPECT_EQ(northEastMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
        } else {
            EXPECT_FALSE(manager.checkMove(northEastMove));
            EXPECT_EQ(northEastMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
        }
    }

    // can move vertically one square only
    for (int i = 1; i < 8; i++) {
        Move verticalMove{.piece = Piece::BK, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = 1};
        if (i == 1) {
            EXPECT_TRUE(manager.checkMove(verticalMove));
            EXPECT_EQ(verticalMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
        } else {
            EXPECT_FALSE(manager.checkMove(verticalMove));
            EXPECT_EQ(verticalMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
        }
    }

    // can move horizontally one square only
    for (int i = 1; i < 8; i++) {
        Move horizMove{.piece = Piece::BK, .rankFrom = 1, .fileFrom = 1, .rankTo = 1, .fileTo = i + 1};
        if (i == 1) {
            EXPECT_TRUE(manager.checkMove(horizMove));
            EXPECT_EQ(horizMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
        } else {
            EXPECT_FALSE(manager.checkMove(horizMove));
            EXPECT_EQ(horizMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
        }
    }
}

TEST(BoardManagerLegality, BishopLegalityCorrect){
    auto manager = BoardManager();
    // load a board with black bishop in a1
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/b7");

    // can move diagonally
    for (int i = 1; i < 8; i++) {
        Move northEastMove{.piece = Piece::BB, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = i + 1};
        EXPECT_TRUE(manager.checkMove(northEastMove));
        EXPECT_EQ(northEastMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
    }

    // can't move vertically
    for (int i = 1; i < 8; i++) {
        Move verticalMove{.piece = Piece::BB, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = 1};

        EXPECT_FALSE(manager.checkMove(verticalMove));
        EXPECT_EQ(verticalMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    }

    // can't move horizontally
    for (int i = 1; i < 8; i++) {
        Move horizMove{.piece = Piece::BB, .rankFrom = 1, .fileFrom = 1, .rankTo = 1, .fileTo = i + 1};
        EXPECT_FALSE(manager.checkMove(horizMove));
        EXPECT_EQ(horizMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    }
}

TEST(BoardManagerLegality, KnightLegalityCorrect){
    auto manager = BoardManager();
    // load a board with black knight in a1
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/n7");

    // can't move diagonally
    for (int i = 1; i < 8; i++) {
        Move northEastMove{.piece = Piece::BN, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = i + 1};
        EXPECT_FALSE(manager.checkMove(northEastMove));
        EXPECT_EQ(northEastMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    }

    // can't move vertically
    for (int i = 1; i < 8; i++) {
        Move verticalMove{.piece = Piece::BN, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = 1};

        EXPECT_FALSE(manager.checkMove(verticalMove));
        EXPECT_EQ(verticalMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    }

    // can't move horizontally
    for (int i = 1; i < 8; i++) {
        Move horizMove{.piece = Piece::BN, .rankFrom = 1, .fileFrom = 1, .rankTo = 1, .fileTo = i + 1};
        EXPECT_FALSE(manager.checkMove(horizMove));
        EXPECT_EQ(horizMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    }

    // can do it's weird hop and jump
    Move hopMove{.piece = Piece::BN, .rankFrom = 1, .fileFrom = 1, .rankTo = 3, .fileTo = 2};
    EXPECT_TRUE(manager.checkMove(hopMove));
    EXPECT_EQ(hopMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
}

TEST(BoardManagerLegality, CantMoveToOccupiedSquareWithSameColour){
    auto manager = BoardManager();
    // load a default board
    manager.getBitboards()->loadFEN(Fen::STARTING_FEN);

    // the white queen can't move to where a white pawn is
    Move whiteQueenMove{.piece = Piece::WQ, .rankFrom = 1, .fileFrom = 1, .rankTo = 2, .fileTo = 1};
    EXPECT_FALSE(manager.checkMove(whiteQueenMove));
    EXPECT_EQ(whiteQueenMove.result, MoveResult::SQUARE_OCCUPIED);
}

TEST(BoardManagerLegality, CanMoveToOccupiedSquareWithOtherColour){
    auto manager = BoardManager();
    // load rooks in the top left-hand and the bottom left-hand of board
    manager.getBitboards()->loadFEN("r7/8/8/8/8/8/8/R7");

    // the white rook can capture the black rook
    Move whiteRookMove{.piece = Piece::WR, .rankFrom = 1, .fileFrom = 1, .rankTo = 8, .fileTo = 1};
    EXPECT_TRUE(manager.checkMove(whiteRookMove));
    EXPECT_EQ(whiteRookMove.result, MoveResult::PIECE_CAPTURE);

    // so can the black
    Move blackRookMove{.piece = Piece::WR, .rankFrom = 1, .fileFrom = 1, .rankTo = 8, .fileTo = 1};
    EXPECT_TRUE(manager.checkMove(blackRookMove));
    EXPECT_EQ(blackRookMove.result, MoveResult::PIECE_CAPTURE);
}

TEST(BoardManagerLegality,MostPiecesCantJumpOthers){
    auto manager = BoardManager();
    // blank space in a3, a1,2,4 occupied.
    // we'll move the white rook to each of 3,5. All should fail
    manager.getBitboards()->loadFEN("8/8/8/8/p7/8/P7/R7");

    Move moveA3{.piece = Piece::WR, .rankFrom = 1, .fileFrom = 1, .rankTo = 3, .fileTo = 1};
    EXPECT_FALSE(manager.checkMove(moveA3));
    EXPECT_EQ(moveA3.result, MoveResult::BLOCKING_PIECE);

    Move moveA5{.piece = Piece::WR, .rankFrom = 1, .fileFrom = 1, .rankTo = 5, .fileTo = 1};
    EXPECT_FALSE(manager.checkMove(moveA5));
    EXPECT_EQ(moveA5.result, MoveResult::BLOCKING_PIECE);
}
TEST(BoardManagerLegality,KnightsCanJumpOthers){
    auto manager = BoardManager();
    // knight in a1 with a variety of blockers
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/rb6/nR6");

    Move moveA3{.piece = Piece::BN, .rankFrom = 1, .fileFrom = 1, .rankTo = 3, .fileTo = 2};
    EXPECT_TRUE(manager.checkMove(moveA3));
    EXPECT_EQ(moveA3.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    Move moveA5{.piece = Piece::BN, .rankFrom = 1, .fileFrom = 1, .rankTo = 2, .fileTo = 3};
    EXPECT_TRUE(manager.checkMove(moveA5));
    EXPECT_EQ(moveA5.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
}

