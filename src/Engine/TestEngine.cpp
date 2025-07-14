//
// Created by jacks on 16/06/2025.
//

#include "Engine/TestEngine.h"

#include <bitset>
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
        } else { opponentPieceValue += pieceValues[pieceName] * mgr.getBitboards()->countPiece(pieceName); }

        materialScore += ourPieceValue - opponentPieceValue;
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


Move TestEngine::search(const int depth){
    BoardManager startingManager = *boardManager_;

    auto moves = generateMoveList(startingManager);
    if (moves.empty()) { return Move(); }

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
    auto move = search(3);
    boardManager_->tryMove(move);
    return move;
}

std::vector<Move> TestEngine::generateValidMovesFromPosition(BoardManager& mgr, const Piece& piece,
                                                             const int startSquare){
    std::vector<Move> validMoves;
    const auto possibleMoves = std::bitset<64>(
        RulesCheck::getPseudoLegalMoves(startSquare, piece, mgr.getBitboards()));

    for (int endSquare = 0; endSquare < possibleMoves.size(); endSquare++) {
        if (!possibleMoves.test(endSquare)) { continue; }

        int startRank, startFile;
        int endRank, endFile;
        squareToRankAndFile(startSquare, startRank, startFile);
        squareToRankAndFile(endSquare, endRank, endFile);
        Move candidateMove(piece, startSquare, endSquare);

        if (mgr.checkMove(candidateMove))
            validMoves.push_back(candidateMove);
    }

    return validMoves;
}


std::vector<Move> TestEngine::generateMovesForPiece(BoardManager& mgr, const Piece& piece){
    std::vector<Move> pieceMoves;

    const auto& piecePositions = std::bitset<64>(mgr.getBitboards()->getBitboard(piece));

    for (int startSquare = 0; startSquare < piecePositions.size(); startSquare++) {
        if (!piecePositions.test(startSquare))
            continue;

        int startRank, startFile;
        squareToRankAndFile(startSquare, startRank, startFile);

        auto validMoves = generateValidMovesFromPosition(mgr, piece, startSquare);
        pieceMoves.insert(pieceMoves.end(), validMoves.begin(), validMoves.end());
    }

    return pieceMoves;
}

std::vector<Move> TestEngine::generateMoveList(BoardManager& mgr){
    std::vector<Move> moves;
    // check each piece we have
    for (int piece = 0; piece < PIECE_N; piece++) {
        auto pieceName = static_cast<Piece>(piece);
        // if it's not our piece, skip it
        if (pieceColours[pieceName] != mgr.getCurrentTurn())
            continue;

        auto pieceMoves = generateMovesForPiece(mgr, pieceName);
        moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
    }
    return moves;
}

auto TestEngine::simplePerft(const int depth, BoardManager& boardManager){
    if (depth == 0)
        return 1;

    int nodes = 0;
    auto moves = generateMoveList(boardManager);
    for (Move& move: moves) {
        boardManager.tryMove(move);
        nodes += simplePerft(depth - 1, boardManager);
        boardManager.undoMove();
    }

    return nodes;
}

PerftResults TestEngine::perft(const int depth, BoardManager& boardManager){
    if (depth == 0) return PerftResults{1, 0, 0, 0, 0, 0};

    PerftResults result{0, 0, 0, 0, 0, 0};
    auto moves = generateMoveList(boardManager);

    for (auto& move: moves) {
        boardManager.tryMove(move);
        PerftResults child = perft(depth - 1, boardManager);

        if (depth == 1) {
            result.nodes++;
            // Only increment move result types at leaf depth
            if (move.resultBits & CHECK) { result.checks++; }
            if (move.resultBits & EN_PASSANT) { result.enPassant++; }
            if (move.resultBits & CAPTURE) { result.captures++; }
            if (move.resultBits & CASTLING) { result.castling++; }
            if (move.resultBits & CHECK_MATE) { result.checkMate++; }
        } else { result += child; }

        boardManager.undoMove();
    }
    return result;
}


auto TestEngine::perftDivide(const int depth, BoardManager& boardManager){
    auto moves = generateMoveList(boardManager);
    std::vector<PerftResults> results;

    for (auto& move: moves) {
        auto result = PerftResults();
        result.nodes = 0;
        result.fen = move.toUCI();
        boardManager.tryMove(move);
        result += perft(depth - 1, boardManager);
        boardManager.undoMove();

        results.push_back(result);
    }

    return results;
}

std::vector<PerftResults> TestEngine::runDivideTest(const std::string& Fen, const int depth) const{
    auto mgr = BoardManager(colour);
    mgr.getBitboards()->loadFEN(Fen);
    return perftDivide(depth, mgr);
}

std::vector<PerftResults> TestEngine::runDivideTest(BoardManager& mgr, const int depth){
    return perftDivide(depth, mgr);
}

PerftResults TestEngine::runPerftTest(const std::string& Fen, const int depth) const{
    auto mgr = BoardManager(colour);
    mgr.getBitboards()->loadFEN(Fen);
    return perft(depth, mgr);
}