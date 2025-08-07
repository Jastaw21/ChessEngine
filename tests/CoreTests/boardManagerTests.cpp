//
// Created by jacks on 21/06/2025.
//

#include <gtest/gtest.h>
#include "BoardManager/BoardManager.h"
#include "BoardManager/Rules.h"
#include "Utility/Fen.h"

TEST(MoveStruct, ToUCICorrect){
    auto move = Move(WP, 1, 1, 2, 1);
    EXPECT_EQ(move.toUCI(), "a1a2");

    auto move2 = Move(WP, 1, 1, 8, 8);
    EXPECT_EQ(move2.toUCI(), "a1h8");
}

TEST(MoveStruct, MakeFromUCICorrect){
    auto move = createMove(WN, "a1a2");
    EXPECT_EQ(move.piece, Piece::WN);
    EXPECT_EQ(move.rankFrom, 1);
    EXPECT_EQ(move.fileFrom, 1);
    EXPECT_EQ(move.rankTo, 2);
    EXPECT_EQ(move.fileTo, 1);
}

TEST(BoardManagerHistory, AllMovesResultInHistory){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("p7/8/8/8/8/8/8/8");

    EXPECT_EQ(manager.getMoveHistory().size(), 0);

    // make a move
    Move move = createMove(BP, "a8a7");

    // successful moves should increase the size
    EXPECT_TRUE(manager.tryMove(move));
    EXPECT_EQ(manager.getMoveHistory().size(), 1);

    // unsuccessful moves shouldn't increase the size
    Move failedMove = createMove(WP, "a7f2");
    EXPECT_FALSE(manager.tryMove(failedMove));
    EXPECT_EQ(manager.getMoveHistory().size(), 1);
}

TEST(BoardManagerHistory, UndoMovesRestoreHistory){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("p7/8/8/8/8/8/8/8");

    EXPECT_EQ(manager.getMoveHistory().size(), 0);

    // make a move
    Move move = createMove(BP, "a8a7");
    manager.tryMove(move);
    EXPECT_EQ(manager.getMoveHistory().size(), 1);

    manager.undoMove(move);
    EXPECT_EQ(manager.getMoveHistory().size(), 0);
}

TEST(BoardManagerLegality, WhitePawnLegalityCorrect){
    auto manager = BoardManager();
    // load a board with pawn in d4
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/3P4/8/8/8");

    // white pawn can only move north, move should fail in all previous cases
    Move southMove = createMove(WP, "d4d3");
    Move eastMove = createMove(WP, "d4e4");
    Move westMove = createMove(WP, "d4c4");
    Move northEastMove = createMove(WP, "d4e5");

    Move northMove = createMove(WP, "d4d5");

    EXPECT_FALSE(manager.checkMove(southMove));
    EXPECT_FALSE(manager.checkMove(eastMove));
    EXPECT_FALSE(manager.checkMove(westMove));
    EXPECT_FALSE(manager.checkMove(northEastMove));

    EXPECT_TRUE(manager.checkMove(northMove));

    EXPECT_TRUE(southMove.resultBits & MoveResult::ILLEGAL_MOVE);
    EXPECT_TRUE(eastMove.resultBits & MoveResult::ILLEGAL_MOVE);
    EXPECT_TRUE(westMove.resultBits & MoveResult::ILLEGAL_MOVE);
    EXPECT_TRUE(northEastMove.resultBits & MoveResult::ILLEGAL_MOVE);
    EXPECT_TRUE(northMove.resultBits & MoveResult::PUSH);

    // if not on the first rank, move only 1 rank.
    Move northMoveTwoSquares = createMove(WP, "d4d6");
    EXPECT_FALSE(manager.checkMove(northMoveTwoSquares));

    manager.getBitboards()->setFenPositionOnly("8/8/8/8/8/8/3P4/8");

    // if on the starting pawn rank, move 1 or 2 ranks.
    auto northMoveFromSecondRankOneSquare = Move(WP, 2, 4, 3, 4);
    auto northMoveFromSecondRankTwoSquares = Move(WP, 2, 4, 4, 4);
    auto northMoveFromSecondRankThreeSquares = Move(WP, 2, 4, 5, 4);
    EXPECT_TRUE(manager.checkMove(northMoveFromSecondRankOneSquare));
    EXPECT_TRUE(manager.checkMove(northMoveFromSecondRankTwoSquares));
    EXPECT_FALSE(manager.checkMove(northMoveFromSecondRankThreeSquares));
}

TEST(BoardManagerLegality, BlackPawnLegalityCorrect){
    auto manager = BoardManager();
    // load a board with pawn in d4
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/3p4/8/8/8");

    // black pawn can only move south, move should fail in all other cases
    auto southMove = Move(BP, 4, 4, 3, 4);
    auto eastMove = Move(BP, 4, 4, 4, 5);
    auto westMove = Move(BP, 4, 4, 4, 3);
    auto southEastMove = Move(BP, 4, 4, 3, 5);
    auto northMove = Move(BP, 4, 4, 5, 4);

    EXPECT_FALSE(manager.checkMove(northMove));
    EXPECT_FALSE(manager.checkMove(eastMove));
    EXPECT_FALSE(manager.checkMove(westMove));
    EXPECT_FALSE(manager.checkMove(southEastMove));

    EXPECT_TRUE(manager.checkMove(southMove));

    // if not on the first rank, move only 1 rank.
    auto southTwoSquares = Move(BP, 4, 4, 2, 4);
    EXPECT_FALSE(manager.checkMove(southTwoSquares));

    manager.getBitboards()->setFenPositionOnly("8/3p4/8/8/8/8/8/8");

    // if on the starting pawn rank, move 1 or 2 ranks.
    auto moveOneSouth = Move(BP, 7, 4, 6, 4);
    auto moveTwoSouth = Move(BP, 7, 4, 5, 4);
    auto moveThreeSouth = Move(BP, 7, 4, 4, 4);
    EXPECT_TRUE(manager.checkMove(moveOneSouth));
    EXPECT_TRUE(manager.checkMove(moveTwoSouth));
    EXPECT_FALSE(manager.checkMove(moveThreeSouth));
}

TEST(BoardManagerLegality, RookLegalityCorrect){
    auto manager = BoardManager();
    // load a board with white rook in a1
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/8/8/8/R7");

    // cant move diagonally
    for (int i = 1; i < 8; i++) {
        auto northEastMove = Move(WR, 1, 1, i + 1, i + 1);
        EXPECT_FALSE(manager.checkMove(northEastMove));
        EXPECT_TRUE(northEastMove.resultBits & MoveResult::ILLEGAL_MOVE);
    }

    // can move vertically
    for (int i = 1; i < 8; i++) {
        auto verticalMove = Move(WR, 1, 1, i + 1, 1);
        EXPECT_TRUE(manager.checkMove(verticalMove));
        EXPECT_TRUE(verticalMove.resultBits & MoveResult::PUSH);
    }

    // can move horizontally
    for (int i = 1; i < 8; i++) {
        auto horizMove = Move(WR, 1, 1, 1, i + 1);
        EXPECT_TRUE(manager.checkMove(horizMove));
        EXPECT_TRUE(horizMove.resultBits & MoveResult::PUSH);
    }
}

TEST(BoardManagerLegality, QueenLegalityCorrect){
    auto manager = BoardManager();
    // load a board with black queen in a1
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/8/8/8/q7");

    // can move diagonally
    for (int i = 1; i < 8; i++) {
        auto northEastMove = Move(WQ, 1, 1, i + 1, i + 1);
        EXPECT_TRUE(manager.checkMove(northEastMove));
    }

    // can move vertically
    for (int i = 1; i < 8; i++) {
        auto verticalMove = Move(WQ, 1, 1, i + 1, 1);
        EXPECT_TRUE(manager.checkMove(verticalMove));
    }

    // can move horizontally
    for (int i = 1; i < 8; i++) {
        auto horizMove = Move(WQ, 1, 1, 1, i + 1);
        EXPECT_TRUE(manager.checkMove(horizMove));
    }

    // can't do random moves
    for (int square = 0; square < 64; square++) {
        int rank, file;
        squareToRankAndFile(square, rank, file);

        if (file == 1)
            continue;
        if (rank == 1)
            continue;
        if (rank - 1 == file - 1)
            continue;
        auto move = Move(WQ, 1, 1, rank, file);
        EXPECT_FALSE(manager.checkMove(move));
    }

    // can't do some random passes
    manager.getBitboards()->setFenPositionOnly(Fen::STARTING_FEN);
    Move move = createMove(WQ, "d8d1");
    EXPECT_FALSE(manager.checkMove(move));
}

TEST(BoardManagerLegality, KingLegalityCorrect){
    auto manager = BoardManager();
    // load a board with black king in a1
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/8/8/8/k7");

    // can diagonally move one square only
    for (int i = 1; i < 8; i++) {
        auto northEastMove = Move(BK, 1, 1, i + 1, i + 1);
        if (i == 1) { EXPECT_TRUE(manager.checkMove(northEastMove)); } else {
            EXPECT_FALSE(manager.checkMove(northEastMove));
        }
    }

    // can vertically move one square only
    for (int i = 1; i < 8; i++) {
        auto verticalMove = Move(BK, 1, 1, i + 1, 1);
        if (i == 1) { EXPECT_TRUE(manager.checkMove(verticalMove)); } else {
            EXPECT_FALSE(manager.checkMove(verticalMove));
        }
    }

    // can horizontally move one square only
    for (int i = 1; i < 8; i++) {
        auto horizMove = Move(BK, 1, 1, 1, i + 1);
        if (i == 1) { EXPECT_TRUE(manager.checkMove(horizMove)); } else { EXPECT_FALSE(manager.checkMove(horizMove)); }
    }

    // can't do random moves
    for (int square = 0; square < 64; square++) {
        int rank, file;
        squareToRankAndFile(square, rank, file);
        auto move = Move(BK, 1, 1, rank, file);

        if (rank > 2 || file > 2) { EXPECT_FALSE(manager.checkMove(move));; }
    }
}

TEST(BoardManagerLegality, KingCanTakeItselfOutOfCheck){
    auto manager = BoardManager();
    // load a board with black king in a1
    manager.getBitboards()->setFenPositionOnly("3K4/3r4/8/8/8/8/8/8");

    auto move = createMove(WK, "d8d7");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_TRUE(move.resultBits & MoveResult::CAPTURE);
}

TEST(BoardManagerLegality, BishopLegalityCorrect){
    auto manager = BoardManager();
    // load a board with a black bishop in a1
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/8/8/8/b7");

    // can move diagonally
    for (int i = 1; i < 8; i++) {
        auto northEastMove = Move(BB, 1, 1, i + 1, i + 1);
        EXPECT_TRUE(manager.checkMove(northEastMove));

        EXPECT_TRUE(northEastMove.resultBits & MoveResult::PUSH);
    }

    // can't move vertically
    for (int i = 1; i < 8; i++) {
        auto verticalMove = Move(BB, 1, 1, i + 1, 1);

        EXPECT_FALSE(manager.checkMove(verticalMove));

        EXPECT_TRUE(verticalMove.resultBits & MoveResult::ILLEGAL_MOVE);
    }

    // can't move horizontally
    for (int i = 1; i < 8; i++) {
        auto horizMove = Move(BB, 1, 1, 1, i + 1);
        EXPECT_FALSE(manager.checkMove(horizMove));

        EXPECT_TRUE(horizMove.resultBits & MoveResult::ILLEGAL_MOVE);
    }

    // can't jump over others
    manager.getBitboards()->setFenPositionOnly(Fen::STARTING_FEN);
    Move move = createMove(WB, "c1e3");
    EXPECT_FALSE(manager.checkMove(move));

    EXPECT_TRUE(move.resultBits & MoveResult::ILLEGAL_MOVE);

    // can't jump over others
    manager.getBitboards()->setFenPositionOnly(Fen::STARTING_FEN);
    Move blackMove = createMove(BB, "c8h3");
    EXPECT_FALSE(manager.checkMove(blackMove));

    EXPECT_TRUE(blackMove.resultBits & MoveResult::ILLEGAL_MOVE);

    // can't capture over others
    manager.getBitboards()->setFenPositionOnly("rnbqkbnr/ppppppp1/7p/8/8/8/PPPPPPPP/RNBQKBNR");
    Move nextMove = createMove(WB, "c1h6");
    EXPECT_FALSE(manager.checkMove(nextMove));

    EXPECT_TRUE(nextMove.resultBits & MoveResult::ILLEGAL_MOVE);
}

TEST(BoardManagerLegality, KnightLegalityCorrect){
    auto manager = BoardManager();
    // load a board with black knight in a1
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/8/8/8/n7");

    // can't move diagonally
    for (int i = 1; i < 8; i++) {
        auto northEastMove = Move(BN, 1, 1, i + 1, i + 1);
        EXPECT_FALSE(manager.checkMove(northEastMove));

        EXPECT_TRUE(northEastMove.resultBits & MoveResult::ILLEGAL_MOVE);
    }

    // can't move vertically
    for (int i = 1; i < 8; i++) {
        auto verticalMove = Move(BN, 1, 1, i + 1, 1);

        EXPECT_FALSE(manager.checkMove(verticalMove));

        EXPECT_TRUE(verticalMove.resultBits & MoveResult::ILLEGAL_MOVE);
    }

    // can't move horizontally
    for (int i = 1; i < 8; i++) {
        auto horizMove = Move(BN, 1, 1, 1, i + 1);
        EXPECT_FALSE(manager.checkMove(horizMove));

        EXPECT_TRUE(horizMove.resultBits & MoveResult::ILLEGAL_MOVE);
    }

    manager.getBitboards()->setFenPositionOnly("8/8/8/8/3N4/8/8/8");

    // can do its weird hop and jumps
    Move move = createMove(BN, "d4b3");
    EXPECT_TRUE(manager.checkMove(move));

    EXPECT_TRUE(move.resultBits & MoveResult::PUSH);
    move = createMove(BN, "d4c2");
    EXPECT_TRUE(manager.checkMove(move));

    EXPECT_TRUE(move.resultBits & MoveResult::PUSH);
    move = createMove(BN, "d4e2");
    EXPECT_TRUE(manager.checkMove(move));

    EXPECT_TRUE(move.resultBits & MoveResult::PUSH);
    move = createMove(BN, "d4f3");
    EXPECT_TRUE(manager.checkMove(move));

    EXPECT_TRUE(move.resultBits & MoveResult::PUSH);
    move = createMove(BN, "d4f5");
    EXPECT_TRUE(manager.checkMove(move));

    EXPECT_TRUE(move.resultBits & MoveResult::PUSH);
    move = createMove(BN, "d4e6");
    EXPECT_TRUE(manager.checkMove(move));

    EXPECT_TRUE(move.resultBits & MoveResult::PUSH);
    move = createMove(BN, "d4c6");
    EXPECT_TRUE(manager.checkMove(move));

    EXPECT_TRUE(move.resultBits & MoveResult::PUSH);
    move = createMove(BN, "d4b5");
    EXPECT_TRUE(manager.checkMove(move));

    EXPECT_TRUE(move.resultBits & MoveResult::PUSH);
}

TEST(BoardManagerLegality, CantMoveToOccupiedSquareWithSameColour){
    auto manager = BoardManager();
    // load a default board
    manager.getBitboards()->setFenPositionOnly(Fen::STARTING_FEN);

    // the white queen can't move to where a white pawn is
    auto whiteQueenMove = Move(WQ, 1, 4, 2, 4);
    EXPECT_FALSE(manager.checkMove(whiteQueenMove));

    EXPECT_TRUE(whiteQueenMove.resultBits & MoveResult::ILLEGAL_MOVE);
}

TEST(BoardManagerLegality, CanMoveToOccupiedSquareWithOtherColour){
    auto manager = BoardManager();
    // load rooks in the top left-hand and the bottom left-hand of board
    manager.getBitboards()->setFenPositionOnly("r7/8/8/8/8/8/8/R7");

    // the white rook can capture the black rook
    auto whiteRookMove = Move(WR, 1, 1, 8, 1);
    EXPECT_TRUE(manager.checkMove(whiteRookMove));

    EXPECT_TRUE(whiteRookMove.resultBits & MoveResult::CAPTURE);

    // so can the black
    auto blackRookMove = Move(BR, 8, 1, 1, 1);
    EXPECT_TRUE(manager.checkMove(blackRookMove));

    EXPECT_TRUE(blackRookMove.resultBits & MoveResult::CAPTURE);
}

TEST(BoardManagerLegality, MostPiecesCantJumpOthers){
    auto manager = BoardManager();
    // Blank space in a3, a1,2,4 occupied.
    // We'll move the white rook to each of 3,5. All should fail
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/p7/8/P7/R7");

    auto moveA3 = Move(WR, 1, 1, 3, 1);
    EXPECT_FALSE(manager.checkMove(moveA3));
    EXPECT_TRUE(moveA3.resultBits & MoveResult::ILLEGAL_MOVE);

    auto moveA5 = Move(WR, 1, 1, 5, 1);
    EXPECT_FALSE(manager.checkMove(moveA5));
    EXPECT_TRUE(moveA5.resultBits & MoveResult::ILLEGAL_MOVE);
}

TEST(BoardManagerLegality, KnightsCanJumpOthers){
    auto manager = BoardManager();
    // knight in a1 with a variety of blockers
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/8/8/rb6/nR6");

    auto moveA3 = Move(BN, 1, 1, 3, 2);
    EXPECT_TRUE(manager.checkMove(moveA3));

    EXPECT_TRUE(moveA3.resultBits & MoveResult::PUSH);

    auto moveA5 = Move(BN, 1, 1, 2, 3);
    EXPECT_TRUE(manager.checkMove(moveA5));

    EXPECT_TRUE(moveA5.resultBits & MoveResult::PUSH);
}

TEST(BoardManagerLegality, CheckWeirdQueenBehaviour){
    auto manager = BoardManager();
    // load a board with white queen in a1
    manager.getBitboards()->setFenPositionOnly(Fen::STARTING_FEN);

    auto move = createMove(WQ, "d8d1");
    EXPECT_FALSE(manager.checkMove(move));
}

TEST(BoardManagerMoveExecution, EmptySquareMoveUpdates){
    auto manager = BoardManager();
    // load a board with white pawn in d4
    manager.getBitboards()->setFenPositionOnly(Fen::STARTING_FEN);

    // move white pawn to e5
    auto move = Move(WP, 2, 1, 3, 1);
    manager.tryMove(move);

    // move to square is correct
    EXPECT_EQ(manager.getBitboards()->getPiece(3, 1).value(), Piece::WP);
    // move from the square is empty
    EXPECT_FALSE(manager.getBitboards()->getPiece(2, 1).has_value());
}

TEST(BoardManagerMoveExecution, CaptureUpdatesState){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/8/8/8/nR6");

    // move rook at A1 to capture
    auto move = Move(WR, 1, 2, 1, 1);
    manager.tryMove(move);

    // a1 is now the white rook
    EXPECT_EQ(manager.getBitboards()->getPiece(1, 1).value(), Piece::WR);
    // the black Knight is gone
    EXPECT_EQ(manager.getBitboards()->getBitboard(Piece::BN), 0ULL);
}

TEST(BoardManagerMoveExecution, MoveUpdatesFen){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly(Fen::STARTING_FEN);
    auto move = createMove(WP, "e2e4");

    manager.tryMove(move);

    EXPECT_EQ(manager.getBitboards()->toFEN(), "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");
}


TEST(BoardManagerMoveExecution, SimpleUndoRestoresState){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/8/8/8/1R6");

    const Bitboard initialBitboard = manager.getBitboards()->getBitboard(WR);

    // move rook at A1 to capture
    auto move = Move(WR, 1, 2, 1, 1);
    manager.tryMove(move);

    manager.undoMove(move);

    const Bitboard finalBitboard = manager.getBitboards()->getBitboard(WR);

    EXPECT_EQ(initialBitboard, finalBitboard);
}

TEST(BoardManagerMoveExecution, UndoCaptureRestoresFullState){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/8/8/8/nR6");

    auto whiteRookBoardInitial = manager.getBitboards()->getBitboard(WR);
    auto blackKnightBoardInitial = manager.getBitboards()->getBitboard(BN);

    // move rook at A1 to capture
    auto move = Move(WR, 1, 2, 1, 1);
    manager.tryMove(move);

    // a1 is now the white rook
    EXPECT_EQ(manager.getBitboards()->getPiece(1, 1).value(), Piece::WR);
    // the black Knight is gone
    EXPECT_EQ(manager.getBitboards()->getBitboard(Piece::BN), 0ULL);

    manager.undoMove(move);
    // a1 is now the black night again
    EXPECT_EQ(manager.getBitboards()->getPiece(1, 1).value(), Piece::BN);
    // the white rook is back where it was
    EXPECT_EQ(manager.getBitboards()->getPiece(1,2), Piece::WR);

    auto whiteRookBoardFinal = manager.getBitboards()->getBitboard(WR);
    auto blackKnightBoardFinal = manager.getBitboards()->getBitboard(BN);

    EXPECT_EQ(whiteRookBoardInitial, whiteRookBoardFinal);
    EXPECT_EQ(blackKnightBoardInitial, blackKnightBoardFinal);
}

TEST(BoardManagerMoveExecution, CastlingCanBeUndone){
    auto manager = BoardManager();
    // this is a white castling position
    manager.getBitboards()->setFenPositionOnly("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");

    // this would be the castling move
    auto whiteMove = createMove(WK, "e1g1");
    auto initialWhiteKingBoard = manager.getBitboards()->getBitboard(WK);
    auto initialWhiteRookBoard = manager.getBitboards()->getBitboard(WR);
    ASSERT_TRUE(manager.checkMove(whiteMove));

    ASSERT_TRUE(whiteMove.resultBits & MoveResult::CASTLING);
    EXPECT_EQ(manager.getBitboards()->getBitboard(WK), initialWhiteKingBoard);
    EXPECT_EQ(manager.getBitboards()->getBitboard(WR), initialWhiteRookBoard);

    manager.getBitboards()->setFenPositionOnly("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");
    auto qsInitialWhiteRook = manager.getBitboards()->getBitboard(WR);
    auto qsInitialWhiteKing = manager.getBitboards()->getBitboard(WK);
    ASSERT_TRUE(manager.checkMove(whiteMove));

    ASSERT_TRUE(whiteMove.resultBits & MoveResult::CASTLING);
    EXPECT_EQ(manager.getBitboards()->getBitboard(WR), qsInitialWhiteRook);
    EXPECT_EQ(manager.getBitboards()->getBitboard(WK), qsInitialWhiteKing);

    manager.getBitboards()->setFenPositionOnly("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");
    auto blackMove = createMove(BK, "e8g8");
    auto initialBlackKingBoard = manager.getBitboards()->getBitboard(BK);
    auto initialBlackRookBoard = manager.getBitboards()->getBitboard(BR);
    ASSERT_TRUE(manager.checkMove(blackMove));

    ASSERT_TRUE(blackMove.resultBits & MoveResult::CASTLING);
    EXPECT_EQ(manager.getBitboards()->getBitboard(BK), initialBlackKingBoard);
    EXPECT_EQ(manager.getBitboards()->getBitboard(BR), initialBlackRookBoard);

    manager.getBitboards()->setFenPositionOnly("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");
    auto ksblackMove = createMove(BK, "e8c8");
    auto ksinitialBlackKingBoard = manager.getBitboards()->getBitboard(BK);
    auto ksinitialBlackRookBoard = manager.getBitboards()->getBitboard(BR);
    ASSERT_TRUE(manager.checkMove(ksblackMove));

    ASSERT_TRUE(ksblackMove.resultBits & MoveResult::CASTLING);
    EXPECT_EQ(manager.getBitboards()->getBitboard(BK), ksinitialBlackKingBoard);
    EXPECT_EQ(manager.getBitboards()->getBitboard(BR), ksinitialBlackRookBoard);
}

TEST(BoardManagerMoveExecution, KingCantBeCaptured){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/8/8/8/kP6");

    auto tryCapture = Move(BP, 1, 2, 1, 1);
    EXPECT_FALSE(manager.tryMove(tryCapture));

    manager.getBitboards()->setFenPositionOnly("3K4/8/2n5/8/8/8/8/8");
    auto tryCapture2 = Move(BN, 6, 3, 8, 4);
    EXPECT_FALSE(manager.tryMove(tryCapture2));
}

TEST(BoardManagerLegality, PawnsOnlyCaptureDiagonally){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/8/8/p7/kP6");

    auto tryCaptureWest = Move(WP, 1, 2, 1, 1);
    EXPECT_FALSE(manager.tryMove(tryCaptureWest));

    auto tryCaptureNorthWest = Move(WP, 1, 2, 2, 1);
    EXPECT_TRUE(manager.tryMove(tryCaptureNorthWest));

    EXPECT_TRUE(tryCaptureNorthWest.resultBits & MoveResult::CAPTURE);

    manager.setCurrentTurn(WHITE);
    manager.getBitboards()->setFenPositionOnly("rnb1kbnr/pppp1ppp/4p3/8/6Pq/5P2/PPPPP2P/RNBQKBNR");
    auto attemptedStraightCapture = Move(WP, 2, 8, 4, 8);

    EXPECT_FALSE(manager.tryMove(attemptedStraightCapture));
}

TEST(BoardManagerAdvancedRules, KingCantMoveInCheck){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/8/8/8/kQ6");

    auto move = createMove(BK, "a1a2");
    EXPECT_FALSE(manager.checkMove(move));
    EXPECT_TRUE(move.resultBits & MoveResult::KING_IN_CHECK);
}

TEST(BoardManagerAdvancedRules, KingCantBeInCheckThroughOtherPieces){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("7k/6r1/8/8/8/8/8/B7");

    auto move = createMove(BK, "h8g8");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_TRUE(move.resultBits & MoveResult::PUSH);
}

TEST(BoardManagerAdvancedRules, PiecesCanSaveKingFromCheck){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/qR6/8/8/K7");

    auto move = createMove(WR, "b4a4");

    EXPECT_TRUE(manager.checkMove(move));

    EXPECT_TRUE(move.resultBits & MoveResult::CAPTURE);
}

TEST(BoardManagerAdvancedRules, CheckCantBeExposed){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("8/8/8/8/8/q7/R7/K7");

    auto rookMoveOutOfWay = createMove(WR, "a2b2");
    EXPECT_FALSE(manager.checkMove(rookMoveOutOfWay));
    EXPECT_TRUE(rookMoveOutOfWay.resultBits & MoveResult::KING_IN_CHECK);

    manager.getBitboards()->setFenPositionOnly("8/2p5/K2p4/1P5r/1R3p1k/8/4P1P1/8");
    auto BPMovesOutOfWay = createMove(BP, "f4f3");
    EXPECT_FALSE(manager.checkMove(BPMovesOutOfWay));

    EXPECT_TRUE(BPMovesOutOfWay.resultBits & MoveResult::KING_IN_CHECK);
}

TEST(BoardManagerAdvancedRules, SimpleEnPassant){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly(Fen::STARTING_FEN);

    // e5e6 e5d5 then into en passant
    auto whiteMove1 = createMove(WP, "e2e4");
    ASSERT_TRUE(manager.tryMove(whiteMove1));
    auto whiteMove2 = createMove(WP, "e4e5");
    ASSERT_TRUE(manager.tryMove(whiteMove2));
    auto blackMove1 = createMove(BP, "f7f5");
    ASSERT_TRUE(manager.tryMove(blackMove1));

    auto testEnPassant = createMove(WP, "e5f6");
    EXPECT_TRUE(manager.tryMove(testEnPassant));

    EXPECT_TRUE(testEnPassant.resultBits & MoveResult::EN_PASSANT);
}

TEST(BoardManagerAdvancedRules, EnPassantCanBeUndone){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly(Fen::STARTING_FEN);

    // e5e6 e5d5 then into en passant
    auto whiteMove1 = createMove(WP, "e2e4");
    manager.tryMove(whiteMove1);
    auto whiteMove2 = createMove(WP, "e4e5");
    manager.tryMove(whiteMove2);
    auto blackMove1 = createMove(BP, "f7f5");
    manager.tryMove(blackMove1);

    auto initialWhiteBitboard = manager.getBitboards()->getBitboard(BP);
    auto initialBlackBitboard = manager.getBitboards()->getBitboard(WP);

    auto testEnPassant = createMove(WP, "e5f6");

    manager.undoMove(testEnPassant);

    auto finalWhiteBitboard = manager.getBitboards()->getBitboard(BP);
    auto finalBlackBitboard = manager.getBitboards()->getBitboard(WP);

    EXPECT_EQ(initialWhiteBitboard, finalWhiteBitboard);
    EXPECT_EQ(initialBlackBitboard, finalBlackBitboard);

    manager.getBitboards()->setFenPositionOnly("r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R");
    auto move = createMove(BP, "b4a3");

    auto blackEnPassantStartingState = manager.getBitboards()->toFEN();
    manager.tryMove(move);
    manager.undoMove(move);
    auto blackEnPassantFinalState = manager.getBitboards()->toFEN();
    EXPECT_EQ(blackEnPassantStartingState, blackEnPassantFinalState);
}

TEST(BoardManagerAdvancedRules, CastlingKingSideWorks){
    auto manager = BoardManager();
    // this is a white castling position
    manager.getBitboards()->setFenPositionOnly("rnbqkbnr/p2ppppp/1pp5/8/4P3/3B1N2/PPPP1PPP/RNBQK2R");

    // this would be the castling move
    auto whiteMove = createMove(WK, "e1g1");
    EXPECT_TRUE(manager.checkMove(whiteMove));

    EXPECT_TRUE(whiteMove.resultBits & MoveResult::CASTLING);

    manager.getBitboards()->setFenPositionOnly("rnbqk2r/p2p1ppp/1ppbpn2/8/4P3/3B1N2/PPPP1PPP/RNBQK2R");
    auto blackMove = createMove(BK, "e8g8");
    EXPECT_TRUE(manager.checkMove(blackMove));

    EXPECT_TRUE(blackMove.resultBits & MoveResult::CASTLING);
}

TEST(BoardManagerAdvancedRules, KingSideCastlingUpdatesBoardState){
    auto manager = BoardManager();

    // this is a white castling position
    manager.getBitboards()->setFenPositionOnly("rnbqkbnr/p2ppppp/1pp5/8/4P3/3B1N2/PPPP1PPP/RNBQK2R");
    // this would be the castling move
    auto whiteMove = createMove(WK, "e1g1");
    ASSERT_TRUE(manager.tryMove(whiteMove));

    EXPECT_EQ(manager.getBitboards()->toFEN(), "rnbqkbnr/p2ppppp/1pp5/8/4P3/3B1N2/PPPP1PPP/RNBQ1RK1");

    manager.getBitboards()->setFenPositionOnly("rnbqk2r/p2p1ppp/1ppbpn2/8/4P3/3B1N2/PPPP1PPP/RNBQK2R");
    auto blackMove = createMove(BK, "e8g8");
    ASSERT_TRUE(manager.tryMove(blackMove));
    EXPECT_EQ(manager.getBitboards()->toFEN(), "rnbq1rk1/p2p1ppp/1ppbpn2/8/4P3/3B1N2/PPPP1PPP/RNBQK2R");
}

TEST(BoardManagerAdvancedRules, QueenSideCastlingUpdatesBoardState){
    auto manager = BoardManager();

    // black first
    manager.getBitboards()->setFenPositionOnly("r3kbnr/p1pqpppp/bp1p4/2n5/8/1PPP4/P3PPPP/RNBQKBNR");
    auto blackMove = createMove(BK, "e8c8");
    ASSERT_TRUE(manager.tryMove(blackMove));

    ASSERT_TRUE(blackMove.resultBits & MoveResult::CASTLING);
    EXPECT_EQ(manager.getBitboards()->toFEN(), "2kr1bnr/p1pqpppp/bp1p4/2n5/8/1PPP4/P3PPPP/RNBQKBNR");
}


TEST(BoardManagerMoveExecution, TurnChanges){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly(Fen::STARTING_FEN);

    auto whiteMove = createMove(WP, "e2e4");
    manager.tryMove(whiteMove);

    EXPECT_EQ(manager.getCurrentTurn(), Colours::BLACK);

    auto blackMove = createMove(BP, "e7e5");
    manager.tryMove(blackMove);

    EXPECT_EQ(manager.getCurrentTurn(), Colours::WHITE);
}

TEST(RulesHeaderTests, WholeFile){
    for (int i = 0; i < 8; i++) {
        switch (i) {
            case 0:
                EXPECT_EQ(Sliders::wholeFile(i), 0x101010101010100);
                break;

            case 1:
                EXPECT_EQ(Sliders::wholeFile(i), 0x202020202020200);
                break;
            case 7:
                EXPECT_EQ(Sliders::wholeFile(i), 0x8080808080808000);
            default: ;
        }
    }
}

TEST(RulesHeaderTests, WholeRank){
    for (int i = 0; i < 8; i++) {
        switch (i) {
            case 0:
                EXPECT_EQ(Sliders::wholeRank(i *8), 0xfe);
                if (Sliders::wholeRank(i * 8) != 0xfe)
                    std::cout << i << " " << (Sliders::wholeRank(i * 8) == 0xfe) << std::endl;
                break;

            case 1:
                EXPECT_EQ(Sliders::wholeRank(i*8), 0xfe00);
                if (Sliders::wholeRank(i * 8) != 0xfe00)
                    std::cout << i << " " << (Sliders::wholeRank(i * 8) == 0xfe00) << std::endl;
                break;
            case 7:
                if (Sliders::wholeRank(i * 8) != 0xfe00000000000000)
                    std::cout << i << " " << (Sliders::wholeRank(i * 8) == 0xfe00000000000000) << std::endl;
                EXPECT_EQ(Sliders::wholeRank(i*8), 0xfe00000000000000);
            default: ;
        }
    }
}

TEST(RulesHeaderTests, Diags){
    EXPECT_EQ(Sliders::diags(18), 0x804020110a000a11);

    EXPECT_EQ(Sliders::diags(rankAndFileToSquare(6,6)), 0x8850005088040201);

    EXPECT_EQ(Sliders::diags(rankAndFileToSquare(8,8)), 0x40201008040201);

    EXPECT_EQ(Sliders::diags(rankAndFileToSquare(1,1)), 0x8040201008040200);
}

TEST(RulesHeaderTests, OnePieceMoves){
    constexpr Bitboard H1 = 7;
    EXPECT_EQ(SingleMoves::north(H1), 32768);
    EXPECT_EQ(SingleMoves::south(H1), 0);
    EXPECT_EQ(SingleMoves::east(H1), 0);
    EXPECT_EQ(SingleMoves::west(H1), 64);

    constexpr Bitboard H8 = 63;
    EXPECT_EQ(SingleMoves::north(H8), 0);
    EXPECT_EQ(SingleMoves::south(H8), 0x80000000000000);
    EXPECT_EQ(SingleMoves::east(H8), 0);
    EXPECT_EQ(SingleMoves::west(H8), 0x4000000000000000);

    constexpr Bitboard A8 = 56;
    EXPECT_EQ(SingleMoves::north(A8), 0);
    EXPECT_EQ(SingleMoves::south(A8), 0x1000000000000);
    EXPECT_EQ(SingleMoves::east(A8), 0x200000000000000);
    EXPECT_EQ(SingleMoves::west(A8), 0);

    constexpr Bitboard A1 = 0;
    EXPECT_EQ(SingleMoves::north(A1), 0x100);
    EXPECT_EQ(SingleMoves::south(A1), 0);
    EXPECT_EQ(SingleMoves::east(A1), 0x2);
    EXPECT_EQ(SingleMoves::west(A1), 0);
}

TEST(RulesHeaderTests, KnightMoves){
    auto boards = BitBoards();
    boards.setFenPositionOnly("8/8/8/8/8/8/8/k7");
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(0,WN, &boards), 0x20400);

    boards.setFenPositionOnly("8/8/8/8/3n4/8/8/8");
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(rankAndFileToSquare(4,4), BN, &boards), 0x142200221400);

    boards.setFenPositionOnly("8/8/8/8/8/8/8/1n6");
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(rankAndFileToSquare(1,2), WN, &boards), 0x50800);
}

TEST(RulesHeaderTests, BishopMoves){
    auto boards = BitBoards();
    boards.setFenPositionOnly("8/8/8/8/8/8/8/B7");
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(rankAndFileToSquare(1,1),WB, &boards), 0x8040201008040200);
    boards.setFenPositionOnly("8/8/8/5b2/8/8/8/8");
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(rankAndFileToSquare(5,6),BB, &boards), 0x488500050880402);
}

TEST(RulesHeaderTests, AttackDoesntJump){
    auto boards = BitBoards();
    boards.setFenPositionOnly(Fen::STARTING_FEN);
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(rankAndFileToSquare(8,3),BB, &boards), 0x0);
    boards.setFenPositionOnly("8/8/8/5b2/8/8/8/8");

    boards.setFenPositionOnly(Fen::KIWI_PETE_FEN);
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(rankAndFileToSquare(7,7),BB, &boards), 0x2000800000000000);
}

TEST(RulesHeaderTests, RookMoves){
    auto boards = BitBoards();
    boards.setFenPositionOnly("8/8/8/8/8/8/8/R7");
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(rankAndFileToSquare(1,1),WR, &boards), 0x1010101010101fe);
    boards.setFenPositionOnly("8/8/5R2/8/8/8/8/8");
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(rankAndFileToSquare(6,6),WR, &boards), 0x2020df2020202020);
    boards.setFenPositionOnly("8/8/8/8/8/8/R7/8");
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(rankAndFileToSquare(2,1),WR, &boards), 0x10101010101fe01);
}

TEST(RulesHeaderTests, QueenMoves){
    auto boards = BitBoards();
    boards.setFenPositionOnly("8/8/8/8/4Q3/8/8/8");
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(rankAndFileToSquare(4,5),WQ, &boards), 0x11925438ef385492);
    boards.setFenPositionOnly("8/8/8/8/4q3/8/8/8");
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(rankAndFileToSquare(4,5),BQ, &boards), 0x11925438ef385492);
    boards.setFenPositionOnly("8/8/8/8/8/8/8/7Q");
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(rankAndFileToSquare(1,8),WQ, &boards), 0x8182848890a0c07f);
    boards.setFenPositionOnly("8/8/8/8/8/8/8/7q");
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(rankAndFileToSquare(1,8),BQ, &boards), 0x8182848890a0c07f);
}

TEST(RulesHeaderTests, KingMoves){
    auto boards = BitBoards();

    boards.setFenPositionOnly("8/8/8/8/4K3/8/8/8");
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(rankAndFileToSquare(4,5),WK,&boards), 0x3828380000);
    boards.setFenPositionOnly("8/8/8/8/8/8/8/7k");
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(rankAndFileToSquare(1,8),BK,&boards), 0xc040);
    boards.setFenPositionOnly("8/8/8/8/8/8/1k6/8");
    EXPECT_EQ(RulesCheck::getPseudoLegalMoves(rankAndFileToSquare(2,2),BK,&boards), 0x70507);
}

TEST(RulesHeaderTests, CastlingMoves){
    auto boards = BitBoards();
    // this is a white castling position
    boards.setFenPositionOnly("rnbqkbnr/p2ppppp/1pp5/8/4P3/3B1N2/PPPP1PPP/RNBQK2R");
    EXPECT_EQ(RulesCheck::getCastlingMoves(4,WK, &boards), 0x40);
}

TEST(RulesHeaderTests, EnPassantVulnerable){
    auto boards = BitBoards();
    boards.setFenPositionOnly("rnbqkbnr/ppppp1pp/8/4Pp2/8/8/PPPP1PPP/RNBQKBNR");

    EXPECT_EQ(RulesCheck::getEnPassantVulnerableSquares(&boards, WHITE), 0x200000000000);

    boards.setFenPositionOnly("rnbqkbnr/ppppp1pp/5p2/4P3/8/8/PPPP1PPP/RNBQKBNR");
    EXPECT_EQ(RulesCheck::getEnPassantVulnerableSquares(&boards, WHITE), 0);
}


TEST(BoardManagerLegality, CantCaptureOwnPieceKiwiPeteScenario){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly(Fen::KIWI_PETE_FEN);

    auto whiteMove = createMove(WP, "b2c3");
    EXPECT_FALSE(manager.checkMove(whiteMove));
}

TEST(BoardManagerLegality, PawnCantJumpOthers){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly(Fen::KIWI_PETE_FEN);

    auto whiteMove = createMove(WP, "c2c4");
    auto whiteMove2 = createMove(WP, "f2f4");
    auto whiteMove3 = createMove(WP, "h2h4");

    EXPECT_FALSE(manager.checkMove(whiteMove));
    EXPECT_FALSE(manager.checkMove(whiteMove2));
    EXPECT_FALSE(manager.checkMove(whiteMove3));
}

TEST(BoardManagerLegality, MissingKiwiPeteMoves){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly(Fen::KIWI_PETE_FEN);

    auto queenMove = createMove(WQ, "f3f6");
    auto bishopMove = createMove(WB, "d2h6");
    auto castlingMove = createMove(WK, "e1g1");
    auto otherCastlingMove = createMove(WK, "e1c1");
    auto missingKingMove = createMove(WK, "e1d1");

    EXPECT_TRUE(manager.checkMove(queenMove));
    EXPECT_TRUE(manager.checkMove(bishopMove));
    EXPECT_TRUE(manager.checkMove(castlingMove));
    EXPECT_TRUE(manager.checkMove(otherCastlingMove));
    EXPECT_TRUE(manager.checkMove(missingKingMove));
}

TEST(BoardManagerMoveExecution, check_e8c8_KP_doesntChangeBoardState){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly(Fen::KIWI_PETE_FEN);

    auto move = createMove(WB, "e2a6");
    ASSERT_TRUE(manager.tryMove(move));

    auto failingMove = createMove(WK, "e8c8");

    auto initialState = manager.getBitboards()->toFEN();
    manager.checkMove(failingMove);
    auto finalState = manager.getBitboards()->toFEN();

    EXPECT_EQ(initialState, finalState);
}

TEST(BoardManagerAdvancedRules, CantCastleThroughAttackedSquares){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("r3k2r/p1ppqNb1/bn2pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");

    auto attemptedCastling = createMove(BK, "e8c8");
    EXPECT_FALSE(manager.checkMove(attemptedCastling));

    manager.getBitboards()->setFenPositionOnly("r3k2r/p1ppqNb1/bn2pnp1/3P4/1p2P3/2N2Q2/PPPBBPpP/R3K2R");
    auto attemptedCastling2 = createMove(WK, "e1g1");
    EXPECT_FALSE(manager.checkMove(attemptedCastling2));
}

TEST(BoardManagerAdvancedRules, CantCastleThroughOccupiedSquares){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("r2qk2r/p1pp1Qb1/bn2p1p1/3PN3/1p2P3/2N4p/PPPBBPPP/R3K2R");

    auto attemptedCastling = createMove(BK, "e8c8");
    EXPECT_FALSE(manager.checkMove(attemptedCastling));
}

TEST(BoardManagerAdvancedRules, CantCastleToC_If_B_Occupied){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("r3k1r1/p1ppqpb1/bn2pnp1/3PN3/1p2P3/5Q1p/PPPBBPPP/RN2K2R");

    auto attemptedCastling = createMove(WK, "e1c1");
    EXPECT_FALSE(manager.checkMove(attemptedCastling));
}

TEST(BoardManagerAdvancedRules, CanCastleToC_if_B_Attacked){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("r3k2r/p1pNqpb1/bn2pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");

    auto attemptedCastling = createMove(BK, "e8c8");
    EXPECT_TRUE(manager.checkMove(attemptedCastling));
}


TEST(BoardManagerMoveExecution, FixPosition3CapturesNotFound){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly(Fen::POSITION_3_FEN);

    auto firstMove = createMove(WP, "e2e3");
    ASSERT_TRUE(manager.tryMove(firstMove));

    auto secondMove = createMove(BR, "h5b5");
    ASSERT_TRUE(manager.tryMove(secondMove));
    EXPECT_TRUE(secondMove.resultBits & CAPTURE);
}

TEST(BoardManagerMoveExecution, Position3TurnChangesIncorrectly){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly(Fen::POSITION_3_FEN);

    auto startingTurn = manager.getCurrentTurn();
    auto firstMove = createMove(WP, "g2g3");
    ASSERT_TRUE(manager.tryMove(firstMove));

    auto endingTurn = manager.getCurrentTurn();

    EXPECT_EQ(startingTurn, WHITE);
    EXPECT_EQ(endingTurn, BLACK);

    // auto secondMove = createMove(WP)
}

TEST(BoardManagerLegality, kiwpeteA2A4EnPassant){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly(Fen::KIWI_PETE_FEN);

    auto whitea2a4 = createMove(WP, "a2a4");
    ASSERT_TRUE(manager.tryMove(whitea2a4));

    // this should work as EP
    auto blackb4b3 = createMove(BP, "b4a3");
    EXPECT_TRUE(manager.checkMove(blackb4b3));
    EXPECT_TRUE(blackb4b3.resultBits & EN_PASSANT);
}

TEST(BoardManagerLegality, KingCanEscapeCheck){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("1nb1r2r/7p/8/1P4p1/8/2b4P/4PP1P/2B1KBNR");

    auto escapeMove = createMove(WK, "e1d1");
    EXPECT_TRUE(manager.checkMove(escapeMove));
}

TEST(BoardManagerLegality, ChecksAreRegistered){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("7k/4R3/8/8/8/8/8/K5R1");
    auto whiteMove = createMove(WR, "e7e8");
    ASSERT_TRUE(manager.tryMove(whiteMove));

    EXPECT_TRUE(whiteMove.resultBits & CHECK);
}

TEST(BoardManagerLegality, KiwiPeteCanMoveBlackPawns){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly(Fen::KIWI_PETE_FEN);
    auto whiteMove = createMove(WN, "e5f7");
    ASSERT_TRUE(manager.tryMove(whiteMove));

    auto blackMove = createMove(BP, "g6g5");
    ASSERT_TRUE(manager.tryMove(blackMove));
}

TEST(BoardManagerLegality, EnPassantRequiresLastMoveToBePawnMovingTwoSquares){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("r3k2r/p1ppqNb1/bn2pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");
    auto blackPawnMoveOneSouth = createMove(BP, "e6e5");
    ASSERT_TRUE(manager.tryMove(blackPawnMoveOneSouth));

    auto attemptedEnPassant = createMove(WP, "d5e6");
    ASSERT_FALSE(manager.checkMove(attemptedEnPassant));
}

TEST(BoardManagerLegality, OpponentKingInCheck){
    auto manager = BoardManager();
    manager.setFullFen("k7/3RR3/8/8/8/8/8/8 w - - 0 1");
    auto move = createMove(WR, "d7d8");
    ASSERT_TRUE(manager.tryMove(move));
    EXPECT_TRUE(move.resultBits & CHECK);
    EXPECT_TRUE(manager.opponentKingInCheck());
}

TEST(BoardManagerAdvancedRules, CheckMateWorks){
    auto manager = BoardManager(WHITE);
    manager.getBitboards()->setFenPositionOnly("8/4N1pk/8/8/8/4R3/8/6K1"); // pre-check state

    auto checkMateMove = createMove(WR, "e3h3");
    ASSERT_TRUE(manager.tryMove(checkMateMove));

    EXPECT_TRUE(manager.isNowCheckMate());

    manager.setCurrentTurn(BLACK);
    manager.getBitboards()->setFenPositionOnly("rnbqkbnr/pppp1ppp/4p3/8/6P1/5P2/PPPPP2P/RNBQKBNR");
    auto checkMateMove2 = createMove(BQ, "d8h4");

    ASSERT_TRUE(manager.tryMove(checkMateMove2));
    EXPECT_TRUE(manager.isNowCheckMate());
    EXPECT_TRUE(checkMateMove2.resultBits & CHECK_MATE);
    EXPECT_TRUE(checkMateMove2.resultBits & CHECK);

    manager.setCurrentTurn(WHITE);
    manager.getBitboards()->setFenPositionOnly("r2qk2r/p1pp1pb1/bn2pQp1/3PN3/1p2P3/2N4p/PPPBBPPP/R3K2R");
    auto checkMateMove3 = createMove(WQ, "f6f7");
    ASSERT_TRUE(manager.tryMove(checkMateMove3));
    EXPECT_TRUE(manager.isNowCheckMate());
    EXPECT_TRUE(checkMateMove3.resultBits & CHECK_MATE);
    EXPECT_TRUE(checkMateMove3.resultBits & CHECK);
}

TEST(BoardManagerAdvancedRules, EnPassantMustBeAttackingLastMovedPiece){
    auto manager = BoardManager(WHITE);
    manager.getBitboards()->setFenPositionOnly(Fen::POSITION_3_FEN);

    auto move1 = createMove(WP, "a2a4");
    ASSERT_TRUE(manager.tryMove(move1));
    auto move2 = createMove(BP, "c7c5");
    ASSERT_TRUE(manager.tryMove(move2));
    auto attemptedEnPassant = createMove(WP, "a4b5");
    EXPECT_FALSE(manager.checkMove(attemptedEnPassant));
}


TEST(BoardManager, StartingSquares){
    auto manager = BoardManager();

    manager.getBitboards()->setFenPositionOnly(Fen::STARTING_FEN);

    const auto startingWhitePawns = manager.getStartingSquaresOfPiece(WP);
    EXPECT_EQ(startingWhitePawns.size(), 8);

    std::vector<int> expectedStarts;
    for (int i = 8; i <= 15; i++) { expectedStarts.push_back(i); }

    for (const auto& start: startingWhitePawns) {
        EXPECT_TRUE(std::ranges::any_of(expectedStarts, [&](const auto actualStart) { return actualStart == start; }));
    }
}

TEST(BoardManager, LoadingFen){
    auto manager = BoardManager();
    manager.setFullFen(Fen::STARTING_FEN + " w KQkq - 0 1");
    EXPECT_EQ(manager.getBitboards()->toFEN(), Fen::STARTING_FEN);
    EXPECT_EQ(manager.getCurrentTurn(), WHITE);

    manager.setFullFen(Fen::STARTING_FEN + " b KQkq - 0 1");
    EXPECT_EQ(manager.getBitboards()->toFEN(), Fen::STARTING_FEN);
    EXPECT_EQ(manager.getCurrentTurn(), BLACK);
}

TEST(BoardManager, ThreeFoldRepetition){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly(Fen::STARTING_FEN);

    auto whiteMove = createMove(WN, "b1c3");
    auto blackMove = createMove(BN, "b8c6");

    auto reverseWhiteMove = createMove(WN, "c3b1");
    auto reverseBlackMove = createMove(BN, "c6b8");

    for (int attempt = 0; attempt < 2; attempt++) {
        EXPECT_NE(manager.getGameResult(), GameResult::REPETITION | GameResult::DRAW);
        ASSERT_TRUE(manager.tryMove(whiteMove));
        ASSERT_TRUE(manager.tryMove(blackMove));
        ASSERT_TRUE(manager.tryMove(reverseWhiteMove));
        ASSERT_TRUE(manager.tryMove(reverseBlackMove));
    }

    EXPECT_EQ(manager.getGameResult(), GameResult::REPETITION | GameResult::DRAW);
}

TEST(BoardManager, CheckMatesAreFoundGameState){
    auto manager = BoardManager();
    manager.setFullFen("rnbqkbnr/pppp1ppp/8/4p3/6P1/5P2/PPPPP2P/RNBQKBNR b KQkq - 0 1");

    auto checkMove = createMove(BQ, "d8h4");
    ASSERT_TRUE(manager.tryMove(checkMove));
    auto result = manager.getGameResult();
    EXPECT_TRUE(checkMove.resultBits & CHECK_MATE);
    EXPECT_TRUE(result & GameResult::CHECKMATE);
}

TEST(BoardManager, CantCastleIfNoCastle){
    auto manager = BoardManager();
    manager.getBitboards()->setFenPositionOnly("rnbqkbnr/pppppppp/8/8/8/1NBQ4/PPPPPPPP/4KBNR");

    auto attemptedCastling = createMove(WK, "e1c1");

    EXPECT_FALSE(manager.checkMove(attemptedCastling));
}