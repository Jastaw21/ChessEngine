//
// Created by jacks on 21/06/2025.
//

#include <gtest/gtest.h>
#include "BoardManager/BoardManager.h"
#include "Utility/Fen.h"

TEST(MoveStruct, ToUCICorrect){
    constexpr Move move{.piece = Piece::WP, .rankFrom = 1, .fileFrom = 1, .rankTo = 2, .fileTo = 1};
    EXPECT_EQ(move.toUCI(), "a1a2");

    constexpr Move move2{.piece = Piece::WP, .rankFrom = 1, .fileFrom = 1, .rankTo = 8, .fileTo = 8};
    EXPECT_EQ(move2.toUCI(), "a1h8");
}

TEST(MoveStruct, MakeFromUCICorrect){
    const auto move = createMove(Piece::WN, "a1a2");
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
    Move move{.piece = Piece::BP, .rankFrom = 8, .fileFrom = 1, .rankTo = 7, .fileTo = 1};

    // succesful moves should increase the size
    EXPECT_TRUE(manager.tryMove(move));
    EXPECT_EQ(manager.getMoveHistory().size(), 1);

    // unsuccsesful moves shouldn't increase the size
    Move failedMove = createMove(Piece::WP, "a7f2");
    EXPECT_FALSE(manager.tryMove(move));
    EXPECT_EQ(manager.getMoveHistory().size(), 1);
}

TEST(BoardManagerHistory, UndoMovesRestoreHistory){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("p7/8/8/8/8/8/8/8");

    EXPECT_EQ(manager.getMoveHistory().size(), 0);

    // make a move
    Move move{.piece = Piece::BP, .rankFrom = 8, .fileFrom = 1, .rankTo = 7, .fileTo = 1};
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
    Move southMove{.piece = Piece::WP, .rankFrom = 4, .fileFrom = 4, .rankTo = 3, .fileTo = 4};
    Move eastMove{.piece = Piece::WP, .rankFrom = 4, .fileFrom = 4, .rankTo = 4, .fileTo = 5};
    Move westMove{.piece = Piece::WP, .rankFrom = 4, .fileFrom = 4, .rankTo = 4, .fileTo = 3};
    Move northEastMove{.piece = Piece::WP, .rankFrom = 4, .fileFrom = 4, .rankTo = 5, .fileTo = 5};

    Move northMove{.piece = Piece::WP, .rankFrom = 4, .fileFrom = 4, .rankTo = 5, .fileTo = 4};

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
    Move southEastMove{.piece = Piece::BP, .rankFrom = 4, .fileFrom = 4, .rankTo = 3, .fileTo = 5};
    Move northMove{.piece = Piece::BP, .rankFrom = 4, .fileFrom = 4, .rankTo = 5, .fileTo = 4};

    EXPECT_FALSE(manager.checkMove(northMove));
    EXPECT_FALSE(manager.checkMove(eastMove));
    EXPECT_FALSE(manager.checkMove(westMove));
    EXPECT_FALSE(manager.checkMove(southEastMove));

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

    // can vertically move one square only
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

    // can horizontally move one square only
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
    // load a board with a black bishop in a1
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

    manager.getBitboards()->loadFEN("8/8/8/8/3N4/8/8/8");

    // can do its weird hop and jumps
    Move move = createMove(Piece::BN, "d4b3");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    move = createMove(Piece::BN, "d4c2");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    move = createMove(Piece::BN, "d4e2");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    move = createMove(Piece::BN, "d4f3");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    move = createMove(Piece::BN, "d4f5");
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    move = createMove(Piece::BN, "d4e6");;
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    move = createMove(Piece::BN, "d4c6");;
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);

    move = createMove(Piece::BN, "d4b5");;
    EXPECT_TRUE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::MOVE_TO_EMPTY_SQUARE);
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

TEST(BoardManagerLegality, MostPiecesCantJumpOthers){
    auto manager = BoardManager();
    // Blank space in a3, a1,2,4 occupied.
    // We'll move the white rook to each of 3,5. All should fail
    manager.getBitboards()->loadFEN("8/8/8/8/p7/8/P7/R7");

    Move moveA3{.piece = Piece::WR, .rankFrom = 1, .fileFrom = 1, .rankTo = 3, .fileTo = 1};
    EXPECT_FALSE(manager.checkMove(moveA3));
    EXPECT_EQ(moveA3.result, MoveResult::BLOCKING_PIECE);

    Move moveA5{.piece = Piece::WR, .rankFrom = 1, .fileFrom = 1, .rankTo = 5, .fileTo = 1};
    EXPECT_FALSE(manager.checkMove(moveA5));
    EXPECT_EQ(moveA5.result, MoveResult::BLOCKING_PIECE);
}

TEST(BoardManagerLegality, KnightsCanJumpOthers){
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

TEST(BoardManagerMoveExecution, EmptySquareMoveUpdates){
    auto manager = BoardManager();
    // load a board with white pawn in d4
    manager.getBitboards()->loadFEN(Fen::STARTING_FEN);

    // move white pawn to e5
    Move move{.piece = Piece::WP, .rankFrom = 2, .fileFrom = 1, .rankTo = 3, .fileTo = 1};
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
    Move move{.piece = Piece::WR, .rankFrom = 1, .fileFrom = 2, .rankTo = 1, .fileTo = 1};
    manager.tryMove(move);

    // a1 is now the white rook
    EXPECT_EQ(manager.getBitboards()->getPiece(1, 1).value(), Piece::WR);
    // the black Knight is gone
    EXPECT_EQ(manager.getBitboards()->getBitboard(Piece::BN), 0ULL);
}

TEST(BoardManagerMoveExecution, SimpleUndoRestoresState){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/1R6");

    uint64_t initialBitboard = manager.getBitboards()->getBitboard(Piece::WR);

    // move rook at A1 to capture
    Move move{.piece = Piece::WR, .rankFrom = 1, .fileFrom = 2, .rankTo = 1, .fileTo = 1};
    manager.tryMove(move);

    manager.undoMove(move);

    uint64_t finalBitboard = manager.getBitboards()->getBitboard(Piece::WR);

    EXPECT_EQ(initialBitboard, finalBitboard);
}

TEST(BoardManagerMoveExecution, UndoCaptureRestoresFullState){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/nR6");

    auto whiteRookBoardInitial = manager.getBitboards()->getBitboard(Piece::WR);
    auto blackKnightBoardInitial = manager.getBitboards()->getBitboard(Piece::BN);

    // move rook at A1 to capture
    Move move{.piece = Piece::WR, .rankFrom = 1, .fileFrom = 2, .rankTo = 1, .fileTo = 1};
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

    const auto whiteRookBoardFinal = manager.getBitboards()->getBitboard(Piece::WR);
    const auto blackKnightBoardFinal = manager.getBitboards()->getBitboard(Piece::BN);

    EXPECT_EQ(whiteRookBoardInitial, whiteRookBoardFinal);
    EXPECT_EQ(blackKnightBoardInitial, blackKnightBoardFinal);
}

TEST(BoardManagerMoveExecution, KingCantBeCaptured){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/kP6");

    auto tryCapture = Move{.piece = Piece::BP, .rankFrom = 1, .fileFrom = 2, .rankTo = 1, .fileTo = 1};
    EXPECT_FALSE(manager.tryMove(tryCapture));

    manager.getBitboards()->loadFEN("3K4/8/2n5/8/8/8/8/8");
    auto tryCapture2 = Move{.piece = Piece::BN, .rankFrom = 6, .fileFrom = 3, .rankTo = 8, .fileTo = 4};
    EXPECT_FALSE(manager.tryMove(tryCapture2));
}

TEST(BoardManagerMoveExecution, PawnsOnlyCaptureDiagonally){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/p7/kP6");

    auto tryCaptureWest = Move{.piece = Piece::WP, .rankFrom = 1, .fileFrom = 2, .rankTo = 1, .fileTo = 1};
    EXPECT_FALSE(manager.tryMove(tryCaptureWest));

    auto tryCaptureNorthWest = Move{.piece = Piece::WP, .rankFrom = 1, .fileFrom = 2, .rankTo = 2, .fileTo = 1};
    EXPECT_TRUE(manager.tryMove(tryCaptureNorthWest));
    EXPECT_EQ(tryCaptureNorthWest.result, MoveResult::PIECE_CAPTURE);
}

TEST(BoardManagerAdvandedRules, KingCantMoveInCheck){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/8/8/8/8/kQ6");

    auto move = createMove(Piece::BK, "a1a2");
    EXPECT_FALSE(manager.checkMove(move));
    EXPECT_EQ(move.result, MoveResult::KING_IN_CHECK);
}

TEST(BoardManagerAdvancedRules, SimpleEnPassant){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::STARTING_FEN);

    // e5e6 e5d5 then into en passant
    auto whiteMove1 = createMove(Piece::WP, "e2e4");
    manager.tryMove(whiteMove1);
    auto whiteMove2 = createMove(Piece::WP, "e4e5");
    manager.tryMove(whiteMove2);
    auto blackMove1 = createMove(Piece::BP, "f7f5");
    manager.tryMove(blackMove1);

    auto testEnPassant = createMove(Piece::WP, "e5f6");
    EXPECT_TRUE(manager.tryMove(testEnPassant));
    EXPECT_EQ(testEnPassant.result, MoveResult::EN_PASSANT);
}

TEST(BoardManagerAdvancedRules, EnPassantCanBeUndone){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::STARTING_FEN);


    // e5e6 e5d5 then into en passant
    auto whiteMove1 = createMove(Piece::WP, "e2e4");
    manager.tryMove(whiteMove1);
    auto whiteMove2 = createMove(Piece::WP, "e4e5");
    manager.tryMove(whiteMove2);
    auto blackMove1 = createMove(Piece::BP, "f7f5");
    manager.tryMove(blackMove1);

    const auto initialWhiteBitboard = manager.getBitboards()->getBitboard(Piece::BP);
    const auto initialBlackBitboard = manager.getBitboards()->getBitboard(Piece::WP);

    auto testEnPassant = createMove(Piece::WP, "e5f6");

    manager.undoMove(testEnPassant);

    const auto finalWhiteBitboard = manager.getBitboards()->getBitboard(Piece::BP);
    const auto finalBlackBitboard = manager.getBitboards()->getBitboard(Piece::WP);

    EXPECT_EQ(initialWhiteBitboard, finalWhiteBitboard);
    EXPECT_EQ(initialBlackBitboard, finalBlackBitboard);
}

