//
// Created by jacks on 21/06/2025.
//

#include <gtest/gtest.h>
#include "BoardManager/BoardManager.h"
#include "Utility/Fen.h"
#include "BoardManager/Rules.h"

TEST(MoveStruct, ToUCICorrect){
    constexpr Move move{.piece = WP, .rankFrom = 1, .fileFrom = 1, .rankTo = 2, .fileTo = 1};
    EXPECT_EQ(move.toUCI(), "a1a2");

    constexpr Move move2{.piece = WP, .rankFrom = 1, .fileFrom = 1, .rankTo = 8, .fileTo = 8};
    EXPECT_EQ(move2.toUCI(), "a1h8");
}

TEST(MoveStruct, MakeFromUCICorrect){
    const auto move = createMove(WN, "a1a2");
    EXPECT_EQ(move.piece, Piece::WN);
    EXPECT_EQ(move.rankFrom, 1);
    EXPECT_EQ(move.fileFrom, 1);
    EXPECT_EQ(move.rankTo, 2);
    EXPECT_EQ(move.fileTo, 1);
}

TEST(BoardManagerHistory, AllMovesResultInHistory){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("p7/8/8/8/8/8/8/8");

    EXPECT_EQ(manager.getMoveHistory().size(), 0);

    // make a move
    Move move{.piece = BP, .rankFrom = 8, .fileFrom = 1, .rankTo = 7, .fileTo = 1};

    // succesful moves should increase the size
    EXPECT_TRUE(manager.tryMove(move));
    EXPECT_EQ(manager.getMoveHistory().size(), 1);

    // unsuccsesful moves shouldn't increase the size
    Move failedMove = createMove(WP, "a7f2");
    EXPECT_FALSE(manager.tryMove(failedMove));
    EXPECT_EQ(manager.getMoveHistory().size(), 1);
}

TEST(BoardManagerHistory, UndoMovesRestoreHistory){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("p7/8/8/8/8/8/8/8");

    EXPECT_EQ(manager.getMoveHistory().size(), 0);

    // make a move
    Move move{.piece = BP, .rankFrom = 8, .fileFrom = 1, .rankTo = 7, .fileTo = 1};
    manager.tryMove(move);
    EXPECT_EQ(manager.getMoveHistory().size(), 1);

    manager.undoMove(move);
    EXPECT_EQ(manager.getMoveHistory().size(), 0);
}

TEST(BoardManagerLegality, WhitePawnLegalityCorrect){
    auto manager = BoardManager();
    // load a board with pawn in d4
    manager.getBitboards()->loadFEN("8/8/8/8/3P4/8/8/8");

    // white pawn can only move north, move should fail in all previous cases
    Move southMove{.piece = WP, .rankFrom = 4, .fileFrom = 4, .rankTo = 3, .fileTo = 4};
    Move eastMove{.piece = WP, .rankFrom = 4, .fileFrom = 4, .rankTo = 4, .fileTo = 5};
    Move westMove{.piece = WP, .rankFrom = 4, .fileFrom = 4, .rankTo = 4, .fileTo = 3};
    Move northEastMove{.piece = WP, .rankFrom = 4, .fileFrom = 4, .rankTo = 5, .fileTo = 5};

    Move northMove{.piece = WP, .rankFrom = 4, .fileFrom = 4, .rankTo = 5, .fileTo = 4};

    EXPECT_FALSE(manager.checkMove(southMove));
    EXPECT_FALSE(manager.checkMove(eastMove));
    EXPECT_FALSE(manager.checkMove(westMove));
    EXPECT_FALSE(manager.checkMove(northEastMove));

    EXPECT_TRUE(manager.checkMove(northMove));

    EXPECT_EQ(southMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    EXPECT_EQ(eastMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    EXPECT_EQ(westMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    EXPECT_EQ(northEastMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);

    EXPECT_EQ(northMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    // if not on the first rank, move only 1 rank.
    Move northMoveTwoSquares{.piece = WP, .rankFrom = 4, .fileFrom = 4, .rankTo = 6, .fileTo = 4};
    EXPECT_FALSE(manager.checkMove(northMoveTwoSquares));

    manager.getBitboards()->loadFEN("8/8/8/8/8/8/3P4/8");

    // if on the starting pawn rank, move 1 or 2 ranks.
    Move northMoveFromSecondRankOneSquare{.piece = WP, .rankFrom = 2, .fileFrom = 4, .rankTo = 3, .fileTo = 4};
    Move northMoveFromSecondRankTwoSquares{.piece = WP, .rankFrom = 2, .fileFrom = 4, .rankTo = 4, .fileTo = 4};
    Move northMoveFromSecondRankThreeSquares{
                .piece = WP, .rankFrom = 2, .fileFrom = 4, .rankTo = 5, .fileTo = 4
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
    Move southMove{.piece = BP, .rankFrom = 4, .fileFrom = 4, .rankTo = 3, .fileTo = 4};
    Move eastMove{.piece = BP, .rankFrom = 4, .fileFrom = 4, .rankTo = 4, .fileTo = 5};
    Move westMove{.piece = BP, .rankFrom = 4, .fileFrom = 4, .rankTo = 4, .fileTo = 3};
    Move southEastMove{.piece = BP, .rankFrom = 4, .fileFrom = 4, .rankTo = 3, .fileTo = 5};
    Move northMove{.piece = BP, .rankFrom = 4, .fileFrom = 4, .rankTo = 5, .fileTo = 4};

    EXPECT_FALSE(manager.checkMove(northMove));
    EXPECT_FALSE(manager.checkMove(eastMove));
    EXPECT_FALSE(manager.checkMove(westMove));
    EXPECT_FALSE(manager.checkMove(southEastMove));

    EXPECT_TRUE(manager.checkMove(southMove));

    // if not on the first rank, move only 1 rank.
    Move southTwoSquares{.piece = BP, .rankFrom = 4, .fileFrom = 4, .rankTo = 2, .fileTo = 4};
    EXPECT_FALSE(manager.checkMove(southTwoSquares));

    manager.getBitboards()->loadFEN("8/3p4/8/8/8/8/8/8");

    // if on the starting pawn rank, move 1 or 2 ranks.
    Move southMoveFromSecondRankOneSquare{.piece = BP, .rankFrom = 7, .fileFrom = 4, .rankTo = 6, .fileTo = 4};
    Move northMoveFromSecondRankTwoSquares{.piece = BP, .rankFrom = 7, .fileFrom = 4, .rankTo = 5, .fileTo = 4};
    Move southMoveFromSecondRankThreeSquares{
                .piece = BP, .rankFrom = 7, .fileFrom = 4, .rankTo = 4, .fileTo = 4
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
        Move northEastMove{.piece = WR, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = i + 1};
        EXPECT_FALSE(manager.checkMove(northEastMove));
        EXPECT_EQ(northEastMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    }

    // can move vertically
    for (int i = 1; i < 8; i++) {
        Move verticalMove{.piece = WR, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = 1};
        EXPECT_TRUE(manager.checkMove(verticalMove));
        EXPECT_EQ(verticalMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
    }

    // can move horizontally
    for (int i = 1; i < 8; i++) {
        Move horizMove{.piece = WR, .rankFrom = 1, .fileFrom = 1, .rankTo = 1, .fileTo = i + 1};
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
        Move northEastMove{.piece = WQ, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = i + 1};
        EXPECT_TRUE(manager.checkMove(northEastMove));
        EXPECT_EQ(northEastMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    }

    // can move vertically
    for (int i = 1; i < 8; i++) {
        Move verticalMove{.piece = WQ, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = 1};
        EXPECT_TRUE(manager.checkMove(verticalMove));
        EXPECT_EQ(verticalMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
    }

    // can move horizontally
    for (int i = 1; i < 8; i++) {
        Move horizMove{.piece = WQ, .rankFrom = 1, .fileFrom = 1, .rankTo = 1, .fileTo = i + 1};
        EXPECT_TRUE(manager.checkMove(horizMove));
        EXPECT_EQ(horizMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
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
        auto move = Move{.piece = WQ, .rankFrom = 1, .fileFrom = 1, .rankTo = rank, .fileTo = file};
        EXPECT_FALSE(manager.checkMove(move));
        EXPECT_EQ(move.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    }
}

TEST(BoardManagerLegality, KingLegalityCorrect){
    auto manager = BoardManager();
    // load a board with black king in a1
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/k7");

    // can diagonally move one square only
    for (int i = 1; i < 8; i++) {
        Move northEastMove{.piece = BK, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = i + 1};
        if (i == 1) {
            EXPECT_TRUE(manager.checkMove(northEastMove));
            EXPECT_EQ(northEastMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
        } else {
            EXPECT_FALSE(manager.checkMove(northEastMove));
            EXPECT_EQ(northEastMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
        }
    }

    // can vertically move one square only
    for (int i = 1; i < 8; i++) {
        Move verticalMove{.piece = BK, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = 1};
        if (i == 1) {
            EXPECT_TRUE(manager.checkMove(verticalMove));
            EXPECT_EQ(verticalMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
        } else {
            EXPECT_FALSE(manager.checkMove(verticalMove));
            EXPECT_EQ(verticalMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
        }
    }

    // can horizontally move one square only
    for (int i = 1; i < 8; i++) {
        Move horizMove{.piece = BK, .rankFrom = 1, .fileFrom = 1, .rankTo = 1, .fileTo = i + 1};
        if (i == 1) {
            EXPECT_TRUE(manager.checkMove(horizMove));
            EXPECT_EQ(horizMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
        } else {
            EXPECT_FALSE(manager.checkMove(horizMove));
            EXPECT_EQ(horizMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
        }
    }

    // can't do random moves
    for (int square = 0; square < 64; square++) {
        int rank, file;
        squareToRankAndFile(square, rank, file);
        auto move = Move{.piece = BK, .rankFrom = 1, .fileFrom = 1, .rankTo = rank, .fileTo = file};

        if (rank > 2 || file > 2) {
            EXPECT_FALSE(manager.checkMove(move));
            EXPECT_EQ(move.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
        }
    }
}

TEST(BoardManagerLegality, KingCanTakeItselfOutOfCheck){
    auto manager = BoardManager();
    // load a board with black king in a1
    manager.getBitboards()->loadFEN("3K4/3r4/8/8/8/8/8/8");

    auto move = createMove(WK, "d8d7");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::PIECE_CAPTURE);
}

TEST(BoardManagerLegality, BishopLegalityCorrect){
    auto manager = BoardManager();
    // load a board with a black bishop in a1
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/b7");

    // can move diagonally
    for (int i = 1; i < 8; i++) {
        Move northEastMove{.piece = BB, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = i + 1};
        EXPECT_TRUE(manager.checkMove(northEastMove));
        EXPECT_EQ(northEastMove.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
    }

    // can't move vertically
    for (int i = 1; i < 8; i++) {
        Move verticalMove{.piece = BB, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = 1};

        EXPECT_FALSE(manager.checkMove(verticalMove));
        EXPECT_EQ(verticalMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    }

    // can't move horizontally
    for (int i = 1; i < 8; i++) {
        Move horizMove{.piece = BB, .rankFrom = 1, .fileFrom = 1, .rankTo = 1, .fileTo = i + 1};
        EXPECT_FALSE(manager.checkMove(horizMove));
        EXPECT_EQ(horizMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    }

    // can't jump over others
    manager.getBitboards()->loadFEN(Fen::STARTING_FEN);
    Move move = createMove(WB, "c1e3");
    EXPECT_FALSE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::BLOCKING_PIECE);

    // can't jump over others
    manager.getBitboards()->loadFEN(Fen::STARTING_FEN);
    Move blackMove = createMove(BB, "c8h3");
    EXPECT_FALSE(manager.checkMove(blackMove));
    EXPECT_EQ(blackMove.result, MoveResult::BLOCKING_PIECE);

    // can't capture over others
    manager.getBitboards()->loadFEN("rnbqkbnr/ppppppp1/7p/8/8/8/PPPPPPPP/RNBQKBNR");
    Move nextMove = createMove(WB, "c1h6");
    EXPECT_FALSE(manager.checkMove(nextMove));
    EXPECT_EQ(nextMove.result, MoveResult::BLOCKING_PIECE);
}

TEST(BoardManagerLegality, KnightLegalityCorrect){
    auto manager = BoardManager();
    // load a board with black knight in a1
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/n7");

    // can't move diagonally
    for (int i = 1; i < 8; i++) {
        Move northEastMove{.piece = BN, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = i + 1};
        EXPECT_FALSE(manager.checkMove(northEastMove));
        EXPECT_EQ(northEastMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    }

    // can't move vertically
    for (int i = 1; i < 8; i++) {
        Move verticalMove{.piece = BN, .rankFrom = 1, .fileFrom = 1, .rankTo = i + 1, .fileTo = 1};

        EXPECT_FALSE(manager.checkMove(verticalMove));
        EXPECT_EQ(verticalMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    }

    // can't move horizontally
    for (int i = 1; i < 8; i++) {
        Move horizMove{.piece = BN, .rankFrom = 1, .fileFrom = 1, .rankTo = 1, .fileTo = i + 1};
        EXPECT_FALSE(manager.checkMove(horizMove));
        EXPECT_EQ(horizMove.result, MoveResult::MOVE_NOT_LEGAL_FOR_PIECE);
    }

    manager.getBitboards()->loadFEN("8/8/8/8/3N4/8/8/8");

    // can do its weird hop and jumps
    Move move = createMove(BN, "d4b3");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    move = createMove(BN, "d4c2");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    move = createMove(BN, "d4e2");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    move = createMove(BN, "d4f3");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    move = createMove(BN, "d4f5");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    move = createMove(BN, "d4e6");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    move = createMove(BN, "d4c6");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    move = createMove(BN, "d4b5");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
}

TEST(BoardManagerLegality, CantMoveToOccupiedSquareWithSameColour){
    auto manager = BoardManager();
    // load a default board
    manager.getBitboards()->loadFEN(Fen::STARTING_FEN);

    // the white queen can't move to where a white pawn is
    Move whiteQueenMove{.piece = WQ, .rankFrom = 1, .fileFrom = 4, .rankTo = 2, .fileTo = 4};
    EXPECT_FALSE(manager.checkMove(whiteQueenMove));
    EXPECT_EQ(whiteQueenMove.result, MoveResult::SQUARE_OCCUPIED);
}

TEST(BoardManagerLegality, CanMoveToOccupiedSquareWithOtherColour){
    auto manager = BoardManager();
    // load rooks in the top left-hand and the bottom left-hand of board
    manager.getBitboards()->loadFEN("r7/8/8/8/8/8/8/R7");

    // the white rook can capture the black rook
    Move whiteRookMove{.piece = WR, .rankFrom = 1, .fileFrom = 1, .rankTo = 8, .fileTo = 1};
    EXPECT_TRUE(manager.checkMove(whiteRookMove));
    EXPECT_EQ(whiteRookMove.result, MoveResult::PIECE_CAPTURE);

    // so can the black
    Move blackRookMove{.piece = WR, .rankFrom = 1, .fileFrom = 1, .rankTo = 8, .fileTo = 1};
    EXPECT_TRUE(manager.checkMove(blackRookMove));
    EXPECT_EQ(blackRookMove.result, MoveResult::PIECE_CAPTURE);
}

TEST(BoardManagerLegality, MostPiecesCantJumpOthers){
    auto manager = BoardManager();
    // Blank space in a3, a1,2,4 occupied.
    // We'll move the white rook to each of 3,5. All should fail
    manager.getBitboards()->loadFEN("8/8/8/8/p7/8/P7/R7");

    Move moveA3{.piece = WR, .rankFrom = 1, .fileFrom = 1, .rankTo = 3, .fileTo = 1};
    EXPECT_FALSE(manager.checkMove(moveA3));
    EXPECT_EQ(moveA3.result, MoveResult::BLOCKING_PIECE);

    Move moveA5{.piece = WR, .rankFrom = 1, .fileFrom = 1, .rankTo = 5, .fileTo = 1};
    EXPECT_FALSE(manager.checkMove(moveA5));
    EXPECT_EQ(moveA5.result, MoveResult::BLOCKING_PIECE);
}

TEST(BoardManagerLegality, KnightsCanJumpOthers){
    auto manager = BoardManager();
    // knight in a1 with a variety of blockers
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/rb6/nR6");

    Move moveA3{.piece = BN, .rankFrom = 1, .fileFrom = 1, .rankTo = 3, .fileTo = 2};
    EXPECT_TRUE(manager.checkMove(moveA3));
    EXPECT_EQ(moveA3.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    Move moveA5{.piece = BN, .rankFrom = 1, .fileFrom = 1, .rankTo = 2, .fileTo = 3};
    EXPECT_TRUE(manager.checkMove(moveA5));
    EXPECT_EQ(moveA5.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
}

TEST(BoardManagerMoveExecution, EmptySquareMoveUpdates){
    auto manager = BoardManager();
    // load a board with white pawn in d4
    manager.getBitboards()->loadFEN(Fen::STARTING_FEN);

    // move white pawn to e5
    Move move{.piece = WP, .rankFrom = 2, .fileFrom = 1, .rankTo = 3, .fileTo = 1};
    manager.tryMove(move);

    // move to square is correct
    EXPECT_EQ(manager.getBitboards()->getPiece(3, 1).value(), Piece::WP);
    // move from the square is empty
    EXPECT_FALSE(manager.getBitboards()->getPiece(2, 1).has_value());
}

TEST(BoardManagerMoveExecution, CaptureUpdatesState){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/nR6");

    // move rook at A1 to capture
    Move move{.piece = WR, .rankFrom = 1, .fileFrom = 2, .rankTo = 1, .fileTo = 1};
    manager.tryMove(move);

    // a1 is now the white rook
    EXPECT_EQ(manager.getBitboards()->getPiece(1, 1).value(), Piece::WR);
    // the black Knight is gone
    EXPECT_EQ(manager.getBitboards()->getBitboard(Piece::BN), 0ULL);
}

TEST(BoardManagerMoveExecution, MoveUpdatesFen){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::STARTING_FEN);
    auto move = createMove(WP, "e2e4");

    manager.tryMove(move);

    EXPECT_EQ(manager.getBitboards()->toFEN(), "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");
}


TEST(BoardManagerMoveExecution, SimpleUndoRestoresState){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/1R6");

    const uint64_t initialBitboard = manager.getBitboards()->getBitboard(WR);

    // move rook at A1 to capture
    Move move{.piece = WR, .rankFrom = 1, .fileFrom = 2, .rankTo = 1, .fileTo = 1};
    manager.tryMove(move);

    manager.undoMove(move);

    const uint64_t finalBitboard = manager.getBitboards()->getBitboard(WR);

    EXPECT_EQ(initialBitboard, finalBitboard);
}

TEST(BoardManagerMoveExecution, UndoCaptureRestoresFullState){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/nR6");

    const auto whiteRookBoardInitial = manager.getBitboards()->getBitboard(WR);
    const auto blackKnightBoardInitial = manager.getBitboards()->getBitboard(BN);

    // move rook at A1 to capture
    Move move{.piece = WR, .rankFrom = 1, .fileFrom = 2, .rankTo = 1, .fileTo = 1};
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

    const auto whiteRookBoardFinal = manager.getBitboards()->getBitboard(WR);
    const auto blackKnightBoardFinal = manager.getBitboards()->getBitboard(BN);

    EXPECT_EQ(whiteRookBoardInitial, whiteRookBoardFinal);
    EXPECT_EQ(blackKnightBoardInitial, blackKnightBoardFinal);
}

TEST(BoardManagerMoveExecution, KingCantBeCaptured){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/kP6");

    auto tryCapture = Move{.piece = BP, .rankFrom = 1, .fileFrom = 2, .rankTo = 1, .fileTo = 1};
    EXPECT_FALSE(manager.tryMove(tryCapture));

    manager.getBitboards()->loadFEN("3K4/8/2n5/8/8/8/8/8");
    auto tryCapture2 = Move{.piece = BN, .rankFrom = 6, .fileFrom = 3, .rankTo = 8, .fileTo = 4};
    EXPECT_FALSE(manager.tryMove(tryCapture2));
}

TEST(BoardManagerMoveExecution, PawnsOnlyCaptureDiagonally){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/p7/kP6");

    auto tryCaptureWest = Move{.piece = WP, .rankFrom = 1, .fileFrom = 2, .rankTo = 1, .fileTo = 1};
    EXPECT_FALSE(manager.tryMove(tryCaptureWest));

    auto tryCaptureNorthWest = Move{.piece = WP, .rankFrom = 1, .fileFrom = 2, .rankTo = 2, .fileTo = 1};
    EXPECT_TRUE(manager.tryMove(tryCaptureNorthWest));
    EXPECT_EQ(tryCaptureNorthWest.result, MoveResult::PIECE_CAPTURE);
}

TEST(BoardManagerAdvancedRules, KingCantMoveInCheck){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/kQ6");

    auto move = createMove(BK, "a1a2");
    EXPECT_FALSE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::KING_IN_CHECK);
}

TEST(BoardManagerAdvancedRules, KingCantBeInCheckThroughOtherPieces){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("7k/6r1/8/8/8/8/8/B7");

    auto move = createMove(BK, "h8g8");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
}

TEST(BoardManagerAdvancedRules, PiecesCanSaveKingFromCheck){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/8/qR6/8/8/K7");

    auto move = createMove(WR, "b4a4");

    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::PIECE_CAPTURE);
}

TEST(BoardManagerAdvancedRules, CheckCantBeExposed){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/8/8/q7/R7/K7");

    auto rookMoveOutOfWay = createMove(WR, "a2b2");
    EXPECT_FALSE(manager.checkMove(rookMoveOutOfWay));
    EXPECT_EQ(rookMoveOutOfWay.result, MoveResult::DISCOVERED_CHECK);
}

TEST(BoardManagerAdvancedRules, SimpleEnPassant){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::STARTING_FEN);

    // e5e6 e5d5 then into en passant
    auto whiteMove1 = createMove(WP, "e2e4");
    manager.tryMove(whiteMove1);
    auto whiteMove2 = createMove(WP, "e4e5");
    manager.tryMove(whiteMove2);
    auto blackMove1 = createMove(BP, "f7f5");
    manager.tryMove(blackMove1);

    auto testEnPassant = createMove(WP, "e5f6");
    EXPECT_TRUE(manager.tryMove(testEnPassant));
    EXPECT_EQ(testEnPassant.result, MoveResult::EN_PASSANT);
}

TEST(BoardManagerAdvancedRules, EnPassantCanBeUndone){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::STARTING_FEN);

    // e5e6 e5d5 then into en passant
    auto whiteMove1 = createMove(WP, "e2e4");
    manager.tryMove(whiteMove1);
    auto whiteMove2 = createMove(WP, "e4e5");
    manager.tryMove(whiteMove2);
    auto blackMove1 = createMove(BP, "f7f5");
    manager.tryMove(blackMove1);

    const auto initialWhiteBitboard = manager.getBitboards()->getBitboard(BP);
    const auto initialBlackBitboard = manager.getBitboards()->getBitboard(WP);

    const auto testEnPassant = createMove(WP, "e5f6");

    manager.undoMove(testEnPassant);

    const auto finalWhiteBitboard = manager.getBitboards()->getBitboard(BP);
    const auto finalBlackBitboard = manager.getBitboards()->getBitboard(WP);

    EXPECT_EQ(initialWhiteBitboard, finalWhiteBitboard);
    EXPECT_EQ(initialBlackBitboard, finalBlackBitboard);
}

TEST(BoardManagerAdvancedRules, CastlingKingSideWorks){
    auto manager = BoardManager();
    // this is a white castling position
    manager.getBitboards()->loadFEN("rnbqkbnr/p2ppppp/1pp5/8/4P3/3B1N2/PPPP1PPP/RNBQK2R");

    // this would be the castling move
    auto whiteMove = createMove(WK, "e1g1");
    EXPECT_TRUE(manager.checkMove(whiteMove));
    EXPECT_EQ(whiteMove.result, MoveResult::CASTLING);

    manager.getBitboards()->loadFEN("rnbqk2r/p2p1ppp/1ppbpn2/8/4P3/3B1N2/PPPP1PPP/RNBQK2R");
    auto blackMove = createMove(BK, "e8g8");
    EXPECT_TRUE(manager.checkMove(blackMove));
    EXPECT_EQ(blackMove.result, MoveResult::CASTLING);
}

TEST(BoardManagerAdvancedRules, KingSideCastlingUpdatesBoardState){
    auto manager = BoardManager();

    // this is a white castling position
    manager.getBitboards()->loadFEN("rnbqkbnr/p2ppppp/1pp5/8/4P3/3B1N2/PPPP1PPP/RNBQK2R");
    // this would be the castling move
    auto whiteMove = createMove(WK, "e1g1");
    ASSERT_TRUE(manager.tryMove(whiteMove));

    EXPECT_EQ(manager.getBitboards()->toFEN(), "rnbqkbnr/p2ppppp/1pp5/8/4P3/3B1N2/PPPP1PPP/RNBQ1RK1");


    manager.getBitboards()->loadFEN("rnbqk2r/p2p1ppp/1ppbpn2/8/4P3/3B1N2/PPPP1PPP/RNBQK2R");
    auto blackMove = createMove(BK, "e8g8");
    ASSERT_TRUE(manager.tryMove(blackMove));
    EXPECT_EQ(manager.getBitboards()->toFEN(),  "rnbq1rk1/p2p1ppp/1ppbpn2/8/4P3/3B1N2/PPPP1PPP/RNBQK2R");


}

TEST(BoardManagerAdvancedRules, QueenSideCastlingUpdatesBoardState){
    auto manager = BoardManager();

    // black first
    manager.getBitboards()->loadFEN("r3kbnr/p1pqpppp/bp1p4/2n5/8/1PPP4/P3PPPP/RNBQKBNR");
    auto blackMove = createMove(BK, "e8c8");
    ASSERT_TRUE(manager.tryMove(blackMove));
    ASSERT_TRUE(blackMove.result == MoveResult::CASTLING);
    EXPECT_EQ(manager.getBitboards()->toFEN(), "2kr1bnr/p1pqpppp/bp1p4/2n5/8/1PPP4/P3PPPP/RNBQKBNR");
}


TEST(BoardManagerMoveExecution, TurnChanges){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::STARTING_FEN);

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
            case(0):
                EXPECT_EQ(CrossBoardMoves::wholeFile(i), 0x101010101010100);
                break;

            case(1):
                EXPECT_EQ(CrossBoardMoves::wholeFile(i), 0x202020202020200);
                break;
            case(7):
                EXPECT_EQ(CrossBoardMoves::wholeFile(i), 0x8080808080808000);
            default: ;
        }
    }
}

TEST(RulesHeaderTests, WholeRank){
    for (int i = 0; i < 8; i++) {
        switch (i) {
            case(0):
                EXPECT_EQ(CrossBoardMoves::wholeRank(i *8), 0xfe);
                if (CrossBoardMoves::wholeRank(i *8) != 0xfe)
                    std::cout << i << " "<< (CrossBoardMoves::wholeRank(i *8) == 0xfe )<< std::endl;
                break;

            case(1):
                EXPECT_EQ(CrossBoardMoves::wholeRank(i*8), 0xfe00);
                if (CrossBoardMoves::wholeRank(i *8) != 0xfe00)
                    std::cout << i << " "<< (CrossBoardMoves::wholeRank(i *8) == 0xfe00 )<< std::endl;
                break;
            case(7):
                if (CrossBoardMoves::wholeRank(i *8) != 0xfe00000000000000)
                    std::cout << i << " "<< (CrossBoardMoves::wholeRank(i *8) == 0xfe00000000000000) << std::endl;
                EXPECT_EQ(CrossBoardMoves::wholeRank(i*8), 0xfe00000000000000);
            default: ;
        }
    }
}

TEST(RulesHeaderTests, Diags){
    EXPECT_EQ(CrossBoardMoves::diags(18), 0x804020110a000a11);

    EXPECT_EQ(CrossBoardMoves::diags(rankAndFileToSquare(6,6)), 0x8850005088040201);

    EXPECT_EQ(CrossBoardMoves::diags(rankAndFileToSquare(8,8)), 0x40201008040201);

    EXPECT_EQ(CrossBoardMoves::diags(rankAndFileToSquare(1,1)), 0x8040201008040200);

}

TEST(RulesHeaderTests, OnePieceMoves){
    constexpr uint64_t H1 = 7;
    EXPECT_EQ( SingleMoves::north(H1),32768);
    EXPECT_EQ( SingleMoves::south(H1),0);
    EXPECT_EQ( SingleMoves::east(H1),0);
    EXPECT_EQ( SingleMoves::west(H1),64);

    constexpr uint64_t H8 = 63;
    EXPECT_EQ( SingleMoves::north(H8),0);
    EXPECT_EQ( SingleMoves::south(H8),0x80000000000000);
    EXPECT_EQ( SingleMoves::east(H8),0);
    EXPECT_EQ( SingleMoves::west(H8),0x4000000000000000);

    constexpr uint64_t A8 = 56;
    EXPECT_EQ( SingleMoves::north(A8),0);
    EXPECT_EQ( SingleMoves::south(A8),0x1000000000000);
    EXPECT_EQ( SingleMoves::east(A8),0x200000000000000);
    EXPECT_EQ( SingleMoves::west(A8),0);

    constexpr uint64_t A1 = 0;
    EXPECT_EQ( SingleMoves::north(A1),0x100);
    EXPECT_EQ( SingleMoves::south(A1),0);
    EXPECT_EQ( SingleMoves::east(A1),0x2);
    EXPECT_EQ( SingleMoves::west(A1),0);
}

TEST(RulesHeaderTests, KnightMoves){

    EXPECT_EQ(SingleMoves::getKnightMoves(0), 0x20400);
    EXPECT_EQ(SingleMoves::getKnightMoves(rankAndFileToSquare(4,4)), 0x142200221400);

    EXPECT_EQ(SingleMoves::getKnightMoves(rankAndFileToSquare(1,2)), 0x50800);
}

TEST(RulesHeaderTests, PawnMoves){
    // all allow diags for simplicity

    // from starting rank - n, ne, 2xn
    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(8,WP), 0x1030000);

    // from next rank - n, ne
    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(16,WP), 0x3000000);


    // from centre - nw, n ,ne
    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(rankAndFileToSquare(4,4),WP), 0x1c00000000);
    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(rankAndFileToSquare(5,8),WP), 0xc00000000000);

    // can't wrap n/s
    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(rankAndFileToSquare(8,8),WP), 0);
}

TEST(RulesHeaderTests, BishopMoves){
    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(rankAndFileToSquare(1,1),WB), 0x8040201008040200);
    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(rankAndFileToSquare(5,6),BB), 0x488500050880402);
}

TEST(RulesHeaderTests, RookMoves){

    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(rankAndFileToSquare(1,1),WR), 0x1010101010101fe);
    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(rankAndFileToSquare(6,6),WR), 0x2020df2020202020);

    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(rankAndFileToSquare(2,1),WR), 0x10101010101fe01);

}

TEST(RulesHeaderTests, QueenMoves){
    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(rankAndFileToSquare(4,5),WQ), 0x11925438ef385492);
    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(rankAndFileToSquare(4,5),BQ), 0x11925438ef385492);
    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(rankAndFileToSquare(1,8),WQ), 0x8182848890a0c07f);
    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(rankAndFileToSquare(1,8),BQ), 0x8182848890a0c07f);
}

TEST(RulesHeaderTests, KingMoves){
    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(rankAndFileToSquare(4,5),WK), 0x3828380000);
    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(rankAndFileToSquare(1,8),BK), 0xc040);
    EXPECT_EQ(RulesCheck::getPsuedoLegalMoves(rankAndFileToSquare(2,2),BK), 0x70507);

}