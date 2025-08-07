//
// Created by jacks on 16/06/2025.
//

#include "Engine/EngineBase.h"

#include <cmath>

#include "EngineShared/CommunicatorBase.h"


float EvaluatorBase::evaluateMove(Move& move){
    const float scoreBefore = evaluate();
    boardManager_->tryMove(move);
    const float scoreAfter = evaluate();
    boardManager_->undoMove();

    const auto result = scoreAfter - scoreBefore;
    if (boardManager_->getCurrentTurn() == WHITE) { return result; }

    return -result;
}

EngineBase::EngineBase(): ChessPlayer(ENGINE),
                          rng(std::chrono::system_clock::now().time_since_epoch().count()){
    evaluator_ = new EvaluatorBase(&internalBoardManager_);
}

void EngineBase::go(const int depth){
    const auto rmove = search(depth);
    communicator_->send("bestmove " + rmove.toUCI());
}

void EngineBase::makeReady(){}

void EngineBase::parseUCI(const std::string& uci){
    auto command = parser.parse(uci);
    // Create a visitor lambda that captures 'this' and forwards to the command handler
    auto visitor = [this](const auto& cmd) { this->commandHandler(cmd, this); };

    if (!command.has_value()) { return; } // no command was parsed
    std::visit(visitor, *command);
}


Move EngineBase::search(const int depth){
    auto moves = generateMoveList();
    if (moves.empty()) { return Move(); }

    int randomIndex = rng() % moves.size();

    Move bestMove = moves[randomIndex]; // shuffle the preselected move, so if all are equal it'll pick a random one
    float bestEval = -INFINITY;

    const Colours thisTurn = internalBoardManager_.getCurrentTurn();
    const bool isWhite = thisTurn == WHITE;

    for (auto& move: moves) {
        internalBoardManager_.forceMove(move);
        float eval = alphaBeta(depth - 1, isWhite, -INFINITY, INFINITY);
        internalBoardManager_.undoMove();

        if (!isWhite) { eval = -eval; }

        if (eval > bestEval) {
            bestEval = eval;
            bestMove = move;
        }
    }
    return bestMove;
}

PerftResults EngineBase::runPerftTest(const std::string& Fen, const int depth){
    internalBoardManager_.getBitboards()->setFenPositionOnly(Fen);
    return perft(depth);
}

std::vector<PerftResults> EngineBase::runDivideTest(const std::string& Fen, const int depth){
    internalBoardManager_.getBitboards()->setFenPositionOnly(Fen);
    return perftDivide(depth);
}

std::vector<PerftResults> EngineBase::runDivideTest(const int depth){ return perftDivide(depth); }

std::vector<Move> EngineBase::generateMoveList(){ return std::vector<Move>(); }

void EngineBase::loadFEN(const std::string& fen){
    // std::string firstPartOfFen;
    // for (int i = 0; i < fen.size(); i++) {
    //     if (fen[i] == ' ') { break; }
    //     firstPartOfFen += fen[i];
    // }
    // internalBoardManager_.getBitboards()->loadFEN(firstPartOfFen);
    boardManager()->setFullFen(fen);
}

float EngineBase::alphaBeta(const int depth, const bool isMaximising, float alpha, float beta){
    if (depth == 0 || internalBoardManager_.isGameOver()) { return evaluator_->evaluate(); }

    auto moves = generateMoveList();
    std::ranges::sort(moves.begin(), moves.end(),
                      [&](const Move& a, const Move& b) { return a.resultBits & CAPTURE > b.resultBits & CAPTURE; });
    if (isMaximising) {
        float maxEval = -INFINITY;
        for (auto& move: moves) {
            internalBoardManager_.forceMove(move);
            float eval = alphaBeta(depth - 1, !isMaximising, alpha, beta);
            internalBoardManager_.undoMove();
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) { break; }
        }
        return maxEval;
    }
    float minEval = INFINITY;
    for (auto& move: moves) {
        internalBoardManager_.forceMove(move);
        float eval = alphaBeta(depth - 1, !isMaximising, alpha, beta);
        internalBoardManager_.undoMove();
        minEval = std::min(minEval, eval);
        beta = std::min(beta, eval);
        if (beta <= alpha) { break; }
    }
    return minEval;
}

PerftResults EngineBase::perft(const int depth){
    if (depth == 0) return PerftResults{1, 0, 0, 0, 0, 0};

    PerftResults result{0, 0, 0, 0, 0, 0};
    auto moves = generateMoveList();

    for (auto& move: moves) {
        internalBoardManager_.forceMove(move);
        // moves should be checked for legality already at this point so don't even worry
        const PerftResults child = perft(depth - 1);

        if (depth == 1) {
            result.nodes++;
            // Only increment move result types at leaf depth
            if (move.resultBits & CHECK) { result.checks++; }
            if (move.resultBits & EN_PASSANT) { result.enPassant++; }
            if (move.resultBits & CAPTURE) { result.captures++; }
            if (move.resultBits & CASTLING) { result.castling++; }
            if (move.resultBits & CHECK_MATE) { result.checkMate++; }
        } else { result += child; }

        internalBoardManager_.undoMove();
    }
    return result;
}

int EngineBase::simplePerft(const int depth){
    if (depth == 0)
        return 1;

    int nodes = 0;
    auto moves = generateMoveList();
    for (Move& move: moves) {
        internalBoardManager_.forceMove(move);
        nodes += simplePerft(depth - 1);
        internalBoardManager_.undoMove();
    }

    return nodes;
}

std::vector<PerftResults> EngineBase::perftDivide(const int depth){
    auto moves = generateMoveList();
    std::vector<PerftResults> results;

    for (auto& move: moves) {
        auto result = PerftResults();
        result.nodes = 0;
        result.fen = move.toUCI();
        internalBoardManager_.forceMove(move);
        result += perft(depth - 1);
        internalBoardManager_.undoMove();

        results.push_back(result);
    }

    return results;
}

