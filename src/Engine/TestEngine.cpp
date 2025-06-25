//
// Created by jacks on 16/06/2025.
//

#include "Engine/TestEngine.h"

#include <bitset>
#include <iostream>
#include <math.h>
#include <bits/ostream.tcc>


std::unordered_map<Piece, int> pieceValues = {
            {WP, 1},
            {BP, 1},

            {WR, 2},
            {BR, 2},

            {WN, 3},
            {BN, 3},

            {WB, 3},
            {BB, 3},

            {WK, 99},
            {BK, 99},

            {WQ, 20},
            {BQ, 20}
        };


TestEngine::TestEngine(const Colours colour) : EngineBase(colour){}

TestEngine::TestEngine(const Colours colour, BoardManager* boardManager): EngineBase(colour),
                                                                          boardManager_(boardManager){}


float TestEngine::evaluate(BoardManager& mgr){
    float materialScore = 0.0f;

    for (int piece = 0; piece < PIECE_N; piece++) {
        auto pieceName = static_cast<Piece>(piece);
        const int pieceValue = pieceValues[pieceName];

        const int whiteCount = mgr.getBitboards()->countPiece(pieceName);
        const int blackCount = mgr.getBitboards()->countPiece(pieceName);

        materialScore += pieceValue * (whiteCount - blackCount);
    }
    return materialScore;
}

float TestEngine::minMax(BoardManager& mgr, const int depth, const bool isMaximising){
    if (depth == 0) { return evaluate(mgr); }

    auto moves = generateMoveList(mgr);

    if (moves.empty()) { return evaluate(mgr); }

    int bestEval = isMaximising ? -100000 : 100000;

    for (Move& move: moves) {
        mgr.tryMove(move);
        int eval = minMax(mgr, depth - 1, !isMaximising);
        mgr.undoMove();

        if (isMaximising) { bestEval = std::max(bestEval, eval); } else { bestEval = std::min(bestEval, eval); }
    }
    return bestEval;
}


Move TestEngine::search(){
    BoardManager startingManager = *boardManager_;

    auto moves = generateMoveList(startingManager);
    if (moves.empty()) {
        std::cout << "No Moves" << std::endl;
        return Move();
    }

    Move bestMove = moves[0];
    float bestEval = -100000;

    const Colours us = startingManager.getCurrentTurn();

    for (Move& move: moves) {
        startingManager.tryMove(move);
        const float eval = minMax(startingManager, 2, us == WHITE);
        startingManager.undoMove();

        if (us == WHITE) {
            if (eval > bestEval) {
                bestEval = eval;
                bestMove = move;
            }
        } else {
            if (eval < bestEval) {
                bestEval = eval;
                bestMove = move;
            }
        }
    }
    return bestMove;
}

Move TestEngine::makeMove(){
    auto move = search();
    boardManager_->tryMove(move);
    return move;
}

std::vector<Move> TestEngine::generateMoveList(BoardManager& mgr) const{
    std::vector<Move> moves;
    // check each piece we have
    for (int piece = 0; piece < PIECE_N; piece++) {
        auto pieceName = static_cast<Piece>(piece);
        // if it's not our piece, skip it
        if (pieceColours[pieceName] != mgr.getCurrentTurn())
            continue;

        // get all the starting squares for this piece
        auto bits = std::bitset<64>(mgr.getBitboards()->getBitboard(pieceName));
        for (int bit = 0; bit < bits.size(); bit++) {
            if (bits.test(bit)) {
                int startRank, startFile;
                squareToRankAndFile(bit, startRank, startFile);
                // get all the squares we can get to
                for (int destSquare = 0; destSquare < 64; destSquare++) {
                    int destRank, destFile;
                    squareToRankAndFile(destSquare, destRank, destFile);
                    // ReSharper disable once CppTooWideScopeInitStatement
                    Move move = {pieceName, startRank, startFile, destRank, destFile};
                    if (mgr.checkMove(move))
                        moves.push_back(move);
                }
            }
        }
    }
    return moves;
}