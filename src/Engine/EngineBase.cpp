//
// Created by jacks on 16/06/2025.
//

#include "Engine/EngineBase.h"

#include <cmath>

#include "Engine/Evaluation.h"
#include "EngineShared/CommunicatorBase.h"


EngineBase::EngineBase() : ChessPlayer(ENGINE),
                           rng(std::chrono::system_clock::now().time_since_epoch().count()){
    evaluator_ = std::make_shared<EvaluatorBase>(&internalBoardManager_);
}

void EngineBase::go(const int depth){
    const auto rmove = search(depth);
    // communicator_->messageQueueFromEngine.push("bestmove " + rmove.toUCI());
    communicator_->sendToEngine("bestmove " + rmove.toUCI());
}

Move EngineBase::getBestMove(const int depth){
    const auto rmove = search(depth);
    return rmove;
}

void EngineBase::makeReady(){ return; }

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
    Move bestMove = moves[randomIndex];

    float bestEval = -INFINITY;
    for (auto& move: moves) {
        internalBoardManager_.forceMove(move);
        float eval = -negamax(depth - 1, 1);
        internalBoardManager_.undoMove();

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

void EngineBase::loadFEN(const std::string& fen){ boardManager()->setFullFen(fen); }


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

// Negamax implementation replaces Minimax logic
float EngineBase::negamax(const int depth, const int ply){
    // Base case: leaf node or game over
    if (depth == 0 || internalBoardManager_.isGameOver()) { return evaluator_->evaluate(); }

    auto moves = generateMoveList();
    if (moves.empty()) {
        return evaluator_->evaluate(); // includes stalemate or mate
    }

    float bestScore = -INFINITY;
    for (auto& move: moves) {
        internalBoardManager_.forceMove(move);
        float score = -negamax(depth - 1, ply + 1);
        internalBoardManager_.undoMove();

        bestScore = std::max(bestScore, score);
    }
    return bestScore;
}