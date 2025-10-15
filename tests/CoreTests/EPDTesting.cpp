//
// Created by jacks on 05/10/2025.
//

#include "EPDParser.h"
#include <gtest/gtest.h>

#include "Engine/ChessEngine.h"


TEST(ParsingEPD, BasicParsing){
    auto parsedResults = ParseEPDFile("C:/Users/jacks/source/repos/Chess/tests/CoreTests/TestPositions.txt");
    // correct number are parsed
    EXPECT_EQ(parsedResults.size(), 25);

    // first move is Nf6+, this fen 1rbq1rk1/p1b1nppp/1p2p3/8/1B1pN3/P2B4/1P3PPP/2RQ1R1K w - -, and a BM
    auto firstResult = parsedResults.front();
    EXPECT_EQ(firstResult.move.piece, WN);
    EXPECT_EQ(firstResult.fen, "1rbq1rk1/p1b1nppp/1p2p3/8/1B1pN3/P2B4/1P3PPP/2RQ1R1K w - - 1 0");
    EXPECT_TRUE(firstResult.BestMove);
    EXPECT_EQ(firstResult.movePGN, "Nf6+");

    auto secondresult = parsedResults.at(1);
    EXPECT_EQ(secondresult.move.piece, BN);
    EXPECT_EQ(secondresult.fen, "3r2k1/p2r1p1p/1p2p1p1/q4n2/3P4/PQ5P/1P1RNPP1/3R2K1 b - - 1 0");
    EXPECT_TRUE(secondresult.BestMove);
    EXPECT_EQ(secondresult.movePGN, "Nxd4");
}

TEST(ParsingPNG, PawnMovesWork){
    // double push
    auto pawnPush = "c4";
    auto pawnPushMove = createMove(WP, "c2c4");
    auto parsedResult = PGNToUCI(pawnPush, Fen::FULL_STARTING_FEN, WHITE);
    EXPECT_EQ(parsedResult.toUCI(), pawnPushMove.toUCI());

    // double push from black
    auto pawnPushBlack = "c5";
    auto pawnPushBlackMove = createMove(BP, "c7c5");
    auto parsedResultBlack = PGNToUCI(pawnPushBlack, Fen::FULL_STARTING_FEN_BLACK, BLACK);
    EXPECT_EQ(parsedResultBlack.toUCI(), pawnPushBlackMove.toUCI());

    // single push
    auto pawnPushSingle = "d3";
    auto pawnPushSingleMove = createMove(WP, "d2d3");
    auto parsedResultSingle = PGNToUCI(pawnPushSingle, Fen::FULL_STARTING_FEN, WHITE);
    EXPECT_EQ(parsedResultSingle.toUCI(), pawnPushSingleMove.toUCI());

    // single push black
    auto black_pawnPushSingle = "h6";
    auto black_pawnPushSingleMove = createMove(BP, "h7h6");
    auto black_parsedResultSingle = PGNToUCI(black_pawnPushSingle, Fen::FULL_STARTING_FEN_BLACK, BLACK);
    EXPECT_EQ(black_parsedResultSingle.toUCI(), black_pawnPushSingleMove.toUCI());
}

TEST(ParsingPNG, PawnCapturesWork){
    const auto startingFen = "rnbqkbnr/ppp3pp/4p3/3P1p2/3P4/8/PP2PPPP/RNBQKBNR w KQkq - 0 5";
    auto pawnCapture = "dxe6";
    auto pawnCaptureMove = createMove(WP, "d5e6");

    auto parsedPNG = PGNToUCI(pawnCapture, startingFen, WHITE);
    EXPECT_EQ(parsedPNG.toUCI(), pawnCaptureMove.toUCI());
}

TEST(Perft, FirstBatch){
    auto results = ParseEPDFile("C:/Users/jacks/source/repos/Chess/tests/CoreTests/TestPositions.txt");
    auto engine = ChessEngine();
    int totalCount = 0;
    int failureCount = 0;
    for (const auto& result: results) {
        totalCount++;
        engine.setFullFen(result.fen);
        auto move = engine.Search(12, 1000);

        const auto matching = move.bestMove.toUCI() == result.move.toUCI();
        const auto correct = result.BestMove ? matching : !matching;
        if (!correct) {
            failureCount++;

            auto moveEvals = engine.getLastSearchEvaluations().getScore(result.move);

            std::cout
                    << "Id: " << totalCount
                    << " failed, starting fen: " << result.fen
                    << " our move: " << move.bestMove.toUCI() << " score: " << move.score
                    << " at depth: " << move.stats.depth
                    << " engine wanted: " << result.move.toUCI() << " which we scored: " << moveEvals
                    << std::endl;
        } else { std::cout << "Passed: " << totalCount << std::endl; }
    }

    EXPECT_EQ(failureCount, 0);
    std::cout << "Failed: " << failureCount << " out of " << totalCount << std::endl;
}