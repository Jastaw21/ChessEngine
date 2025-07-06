//
// Created by jacks on 20/06/2025.
//

#include <gtest/gtest.h>

#include "BoardManager/BitBoards.h"
#include "Engine/Piece.h"
#include "Utility/Fen.h"

TEST(BitBoards, BitBoardsCanBeBuilt){
    auto board = BitBoards();
    const auto blackRookA8Fen = "r7/8/8/8/8/8/8/8";
    board.loadFEN(blackRookA8Fen);

    const auto blackRookBoard = board[BR];
    // piece added to blackRookBoard
    EXPECT_GT(blackRookBoard, 0);

    // all other pieces are zero
    for (int i = 0; i < PIECE_N; i++) {
        const auto piece = static_cast<Piece>(i);
        if (piece == BR)
            break;

        EXPECT_EQ(board[piece], 0);
    }

    // test board resets on loading a new fen
    const auto whiteRookH1Fen = "8/8/8/8/8/8/8/R7";
    board.loadFEN(whiteRookH1Fen);
    for (int i = 0; i < PIECE_N; i++) {
        const auto piece = static_cast<Piece>(i);
        if (piece == WR) {
            EXPECT_EQ(board[piece], 1);
            break;
        }

        EXPECT_EQ(board[piece], 0);
    }
}

TEST(BitBoards, BitboardsLocationsCorrect){
    auto board = BitBoards();
    const auto blackRookA8Fen = "r7/8/8/8/8/8/8/8";
    board.loadFEN(blackRookA8Fen);

    auto blackRookBoard = board[BR];
    EXPECT_EQ(blackRookBoard, 0x100000000000000);

    board.loadFEN(Fen::STARTING_FEN);
    blackRookBoard = board[BR];
    EXPECT_EQ(blackRookBoard, 0x8100000000000000);

    EXPECT_EQ(board[Piece::WP], 0xff00);
    EXPECT_EQ(board[Piece::WN], 0x42);
    EXPECT_EQ(board[Piece::WB], 0x24);
    EXPECT_EQ(board[Piece::WK], 0x10);
    EXPECT_EQ(board[Piece::WQ], 0x8);
}

TEST(BitBoards,ToFenWorksAfterInit){

    auto board = BitBoards();
    board.loadFEN(Fen::STARTING_FEN);
    EXPECT_EQ(board.toFEN(), Fen::STARTING_FEN);
}

TEST(BitBoards, ToFenWorksInWeirdPositions){
    auto board = BitBoards();
    board.loadFEN("rnbqkbnr/p2ppppp/1pp5/8/4P3/3B1N2/PPPP1PPP/RNBQK2R");
    EXPECT_EQ(board.toFEN(), "rnbqkbnr/p2ppppp/1pp5/8/4P3/3B1N2/PPPP1PPP/RNBQK2R");
}


TEST(BitBoards, SetZeroRemovesBit){
    auto board = BitBoards();
    const auto blackRookA8Fen = "r7/8/8/8/8/8/8/8";
    board.loadFEN(blackRookA8Fen);

    board.setZero(8, 1);

    for (int i = 0; i < PIECE_N; i++) {
        const auto piece = static_cast<Piece>(i);
        EXPECT_EQ(board[piece], 0);
    }

    const auto blackRookBoard = board[BR];
    EXPECT_EQ(blackRookBoard, 0);
}

TEST(BitBoards, SetOneSetsBit){
    auto board = BitBoards();
    const auto blackRookA8Fen = "8/8/8/8/8/8/8/8";
    board.loadFEN(blackRookA8Fen);

    board.setOne(BR, 8, 1);

    const auto blackRookBoard = board[BR];
    EXPECT_EQ(blackRookBoard, 0x100000000000000);
}

TEST(BitBoards, RankAndFileToSquare){
    int rank, file;

    // square 0 / "a1"
    Fen::FenToRankAndFile("a1", rank, file);
    EXPECT_EQ(rankAndFileToSquare(rank, file), 0);

    // last square, 64 / "h8"
    Fen::FenToRankAndFile("h8", rank, file);
    EXPECT_EQ(rankAndFileToSquare(rank, file), 63);

    // top left square, 56, "a8"
    Fen::FenToRankAndFile("a8", rank, file);
    EXPECT_EQ(rankAndFileToSquare(rank, file), 56);
}

TEST(BitBoards, SquareToRankAndFile){
    int rank, file;
    constexpr int a1 = 0;
    squareToRankAndFile(a1, rank, file);
    EXPECT_EQ(1, rank);
    EXPECT_EQ(1, file);
}

TEST(BitBoards, TestBoardFunction){
    auto board = BitBoards();
    board.loadFEN(Fen::STARTING_FEN);

    constexpr uint64_t a1 = 1;
    constexpr uint64_t a4 = 0x1000000;

    EXPECT_TRUE(board.testBoard(a1));
    EXPECT_FALSE(board.testBoard(a4));
}

TEST(BitBoards, TestSquareFunction){
    auto board = BitBoards();
    board.loadFEN(Fen::STARTING_FEN);

    for (int i = 0; i < 64; i++) {
        if (i <=15 || i >=48) {
            bool test = board.testSquare(i);
            EXPECT_TRUE(board.testSquare(i));
        }
        else
            EXPECT_FALSE(board.testSquare(i));
    }
}

TEST(BitBoards, GetPieceFindsPieces){
    auto board = BitBoards();
    board.loadFEN("pppppppp/8/8/8/8/8/8/8");

    for (int i = 56; i < 64; i++) { EXPECT_EQ(board.getPiece(i).value(), Piece::BP); }
}

TEST(BitBoards, GetPieceReturnsNullInEmptySquares){
    auto board = BitBoards();
    board.loadFEN("pppppppp/8/8/8/8/8/8/8");

    for (int i = 0; i < 56; i++) { EXPECT_FALSE(board.getPiece(i).has_value()); }
}

TEST(BitBoards, GetPieceOverloadsMatch){
    auto board = BitBoards();
    board.loadFEN("pppppppp/8/8/8/8/8/8/8");

    for (int i = 0; i < 56; i++) {
        int rank, file;
        squareToRankAndFile(i, rank, file);
        EXPECT_EQ(board.getPiece(rank,file), board.getPiece(i));
    }

    for (int i = 56; i < 64; i++) {
        int rank, file;
        squareToRankAndFile(i, rank, file);
        EXPECT_EQ(board.getPiece(rank,file).value(), board.getPiece(i).value());
    }
}

TEST(BitBoards, CountPieces){

    auto boards = BitBoards();
    boards.loadFEN(Fen::STARTING_FEN);

    EXPECT_EQ(boards.countPiece(WP), 8);
    EXPECT_EQ(boards.countPiece(WK), 1);

}

TEST(BitBoards, WeirdBlackRookInWrongPlace){

    auto boards = BitBoards();
    boards.loadFEN("3K4/3r4/8/8/8/8/8/8");

    const auto& blackRookBoard = boards[BR];
    EXPECT_EQ(blackRookBoard, 0x8000000000000);

    const auto& whiteRookBoard = boards[WR];
    EXPECT_EQ(whiteRookBoard, 0x0);

}