//
// Created by jacks on 16/06/2025.
//

#include "Engine/TestEngine.h"

#include <bitset>
#include <iostream>
#include <bits/ostream.tcc>
#include "BoardManager/Rules.h"


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


float TestEngine::evaluate(BoardManager& mgr) const{
    float materialScore = 0.0f;

    for (int piece = 0; piece < PIECE_N; piece++) {
        auto pieceName = static_cast<Piece>(piece);

        int ourPieceValue = 0;
        int opponentPieceValue = 0;
        if (pieceColours[pieceName] == colour) {
            ourPieceValue += pieceValues[pieceName] * mgr.getBitboards()->countPiece(pieceName);
        }

        else {
            opponentPieceValue += pieceValues[pieceName] * mgr.getBitboards()->countPiece(pieceName);
        }

        materialScore += ourPieceValue - opponentPieceValue;
    }
    return materialScore;
}

float TestEngine::minMax(BoardManager& mgr, const int depth, const bool isMaximising) const{
    if (depth == 0) { return evaluate(mgr); }

    auto moves = generateMoveList(mgr);

    if (moves.empty()) { return evaluate(mgr); }

    int bestEval = isMaximising ? -100000 : 100000;

    for (Move& move: moves) {
        mgr.tryMove(move);
        int eval = minMax(mgr, depth - 1, !isMaximising);
        mgr.undoMove();

        if (isMaximising) { bestEval = std::max(bestEval, eval); }
        else { bestEval = std::min(bestEval, eval); }
    }
    return bestEval;
}


Move TestEngine::search(int depth) const{
    BoardManager startingManager = *boardManager_;

    auto moves = generateMoveList(startingManager);
    if (moves.empty()) {
        std::cout << "No Moves" << std::endl;
        return Move();
    }

    Move bestMove = moves[0];
    float bestEval = -100000;

    const Colours thisTurn = startingManager.getCurrentTurn();


    for (Move& move: moves) {
        startingManager.tryMove(move);
        const float eval = minMax(startingManager, depth, thisTurn == colour);
        startingManager.undoMove();

        if (thisTurn == colour) {
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

std::vector<Move> TestEngine::generateValidMovesFromPosition(const BoardManager& mgr, Piece piece,
                                                             int start_square) const{}


std::vector<Move> TestEngine::generateMovesForPiece(BoardManager& mgr, const Piece& piece) const{
    std::vector<Move> pieceMoves;

    const auto& piecePositions = std::bitset<64>(mgr.getBitboards()->getBitboard(piece));

    for (int startSquare = 0 ; startSquare < piecePositions.size(); startSquare++) {
        if (!piecePositions.test(startSquare))
            continue;

        int startRank, startFile;
        squareToRankAndFile(startSquare, startRank, startFile);

        auto validMoves = generateValidMovesFromPosition(mgr, piece, startSquare);
        pieceMoves.insert(pieceMoves.end(), validMoves.begin(), validMoves.end());
    }
}

std::vector<Move> TestEngine::generateMoveList(BoardManager& mgr){
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

                const auto destMoves = RulesCheck::getPsuedoLegalMoves(bit, pieceName);
                auto attackBits = std::bitset<64>(destMoves);
                for (int attackBit = 0; attackBit < attackBits.size(); attackBit++) {
                    if (attackBits.test(attackBit)) {
                        int destRank, destFile;
                        squareToRankAndFile(attackBit, destRank, destFile);
                        // ReSharper disable once CppTooWideScopeInitStatement
                        Move move = {pieceName, startRank, startFile, destRank, destFile};
                        if (mgr.checkMove(move))
                            moves.push_back(move);
                    }
                }
            }
        }
    }
    return moves;
}


PerftResults TestEngine::perft(const int depth, BoardManager& boardManager) const{
    if (depth == 0) return PerftResults(1, 0);

    PerftResults perft_results{0, 0};
    std::vector<Move> moves = generateMoveList(boardManager);

    for (Move& move : moves) {
        boardManager.tryMove(move);
        perft_results.captures += move.result == PIECE_CAPTURE ? 1 : 0;
        perft_results.enPassant += move.result == EN_PASSANT ? 1 : 0;

        const PerftResults child_perft_results = perft(depth - 1, boardManager);
        perft_results += child_perft_results;

        boardManager.undoMove();
    }

    return perft_results;
}

PerftResults TestEngine::runPerftTest(const std::string& Fen, const int depth) const{
    BoardManager mgr = BoardManager(colour);
    mgr.getBitboards()->loadFEN(Fen);
    return perft(depth, mgr);
}