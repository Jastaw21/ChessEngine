//
// Created by jacks on 16/06/2025.
//

#include "Engine/TestEngine.h"

#include <bitset>
#include <iostream>
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

uint64_t TestEngine::getPrelimValidMoves(const Piece& piece, const int square){
    uint64_t result = 0ULL;
    switch (piece) {
        case WP:
        case BP:
            result |= 1ULL << (square + pieceColours[piece] == WHITE ? 8 : -8);
            if (square <= 15 && square >= 8)
                result |= 1ULL << (square + 16);
            return result;

        default:
            break;
    }
    return 0ULL;
}

float TestEngine::evaluate(){
    float materialScore = 0;

    for (int piece = 0; piece < PIECE_N; piece++) {
        auto pieceName = static_cast<Piece>(piece);
        const int pieceValue = pieceValues[pieceName];
        const int count = boardManager_->getBitboards()->countPiece(pieceName);
        const int valueCount = pieceValue * count;

        if (pieceColours[pieceName] == colour) { materialScore += valueCount; } else { materialScore -= valueCount; }
    }

    return materialScore;
}

Move TestEngine::search(){
    Position pos = Position(*boardManager_->getBitboards(), colour);

    auto moves = generateMoveList(pos);
    if (moves.empty()) {
        std::cout << "No Moves" << std::endl;
        return Move();
    }

    float bestMove = evaluate();

    int bestMoveIndex = 0;
    for (int move = 0; move < moves.size(); move++) {
        boardManager_->tryMove(moves[move]);
        std::cout << bestMove << std::endl;
        if (evaluate() > bestMove) {
            bestMove = evaluate();
            bestMoveIndex = move;
        }
        boardManager_->undoMove(moves[move]);
    }
    return moves[bestMoveIndex];
}

Move TestEngine::makeMove(){
    auto move = search();
    boardManager_->tryMove(move);
    return move;
}

std::vector<Move> TestEngine::generateMoveList(Position& position) const{
    std::vector<Move> moves;
    // check each piece we have
    for (int piece = 0; piece < PIECE_N; piece++) {
        auto pieceName = static_cast<Piece>(piece);
        // if it's not our piece, skip it
        if (pieceColours[pieceName] != colour)
            continue;

        // get all the starting squares for this piece
        auto bits = std::bitset<64>(position.getBitboards().getBitboard(pieceName));
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
                    if (boardManager_->checkMove(move))
                        moves.push_back(move);
                }
            }
        }
    }
    return moves;
}