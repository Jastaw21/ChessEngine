//
// Created by jacks on 16/06/2025.
//

#include "Engine/EngineBase.h"

#include <cmath>

#include "Engine/Evaluation.h"

float MATE_SCORE = 10000;

EngineBase::EngineBase() : ChessPlayer(ENGINE),
                           rng(std::chrono::system_clock::now().time_since_epoch().count()){
    auto path = std::filesystem::current_path();
    std::string suffix = "";
    suffix += rng() % 26 + 65;
    path += "/searchLog" + suffix + ".txt";
    searchLogStream = std::ofstream(path);

    evaluator_.setBoardManager(&internalBoardManager_);
}

void EngineBase::go(const int depth){
    const auto bestMove = search(depth);
    std::cout << "bestmove " << bestMove.toUCI() << std::endl;
}


void EngineBase::parseUCI(const std::string& uci){
    auto command = parser.parse(uci);
    // Create a visitor lambda that captures 'this' and forwards to the command handler
    auto visitor = [this](const auto& cmd) { this->commandHandler(cmd, this); };

    if (!command.has_value()) { return; } // no valid command was parsed
    std::visit(visitor, *command);
}

Move EngineBase::search(const int depth){
    std::string lastMove = internalBoardManager_.getMoveHistory().size() > 0
                               ? internalBoardManager_.getMoveHistory().top().toUCI()
                               : "New";
    searchLogStream << "Fen:" << internalBoardManager_.getFullFen() << " " << lastMove << std::endl;

    auto moves = generateMoveList();
    if (moves.empty()) { return Move(); }

    int randomIndex = rng() % moves.size();
    Move bestMove = moves[randomIndex];

    float bestEval = -MATE_SCORE - 1;
    for (auto& move: moves) {
        internalBoardManager_.forceMove(move);
        float eval = -negamax(depth - 1, 1);

        internalBoardManager_.undoMove();

        if (eval > bestEval) {
            bestEval = eval;
            bestMove = move;
        }

        searchLogStream << move.toUCI() << " " << eval << std::endl;
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


SearchResults EngineBase::searchWithResult(int depth){
    SearchResults bestResult;
    auto moves = generateMoveList();
    if (moves.empty()) { return bestResult; }

    bestResult.bestMove = moves[0];
    bestResult.score = -MATE_SCORE - 1;

    for (auto& move: moves) {
        internalBoardManager_.forceMove(move);

        std::vector<Move> thisPV;
        float eval = -negamaxWithPV(depth - 1, 1, thisPV);

        internalBoardManager_.undoMove();

        if (eval > bestResult.score) {
            bestResult.score = eval;
            bestResult.bestMove = move;

            bestResult.variation.clear();
            bestResult.variation.push_back(move);
            bestResult.variation.insert(bestResult.variation.end(),
                                        thisPV.begin(), thisPV.end());
        }
    }

    bestResult.depth = depth;
    return bestResult;
}

std::vector<Move> EngineBase::generateMoveList(){ return std::vector<Move>(); }

void EngineBase::loadFEN(const std::string& fen){ boardManager()->setFullFen(fen); }

PerftResults EngineBase::perft(const int depth){
    if (depth == 0) return PerftResults{1, 0, 0, 0, 0, 0};

    PerftResults result{0, 0, 0, 0, 0, 0};
    auto moves = generateMoveList();

    for (auto& move: moves) {
        // moves should be checked for legality already at this point so don't even worry
        internalBoardManager_.forceMove(move);
        const PerftResults child = perft(depth - 1);

        if (depth == 1) {
            result.nodes++;
            // Only increment move result types at leaf depth
            if (move.resultBits & CHECK) { result.checks++; }
            if (move.resultBits & EN_PASSANT) { result.enPassant++; }
            if (move.resultBits & CAPTURE) { result.captures++; }
            if (move.resultBits & CASTLING) { result.castling++; }
            if (move.resultBits & CHECK_MATE) { result.checkMate++; }
            if (move.resultBits & PROMOTION) { result.promotions++; }
        } else { result += child; }

        internalBoardManager_.undoMove();
    }
    return result;
}

std::vector<PerftResults> EngineBase::perftDivide(const int depth){
    auto moves = generateMoveList();
    std::vector<PerftResults> results;

    for (auto& move: moves) {
        auto result = PerftResults();
        result.fen = move.toUCI();
        internalBoardManager_.forceMove(move);

        if (depth == 1) {
            result.nodes = 1;
            if (move.resultBits & CHECK) { result.checks++; }
            if (move.resultBits & EN_PASSANT) { result.enPassant++; }
            if (move.resultBits & CAPTURE) { result.captures++; }
            if (move.resultBits & CASTLING) { result.castling++; }
            if (move.resultBits & CHECK_MATE) { result.checkMate++; }
            if (move.resultBits & PROMOTION) { result.promotions++; }
        } else { result += perft(depth - 1); }

        internalBoardManager_.undoMove();
        results.push_back(result);
    }

    return results;
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


float EngineBase::negamax(const int depth, const int ply){
    if (internalBoardManager_.getGameResult() & GameResult::CHECKMATE) { return -MATE_SCORE + ply; }
    if (internalBoardManager_.getGameResult() & GameResult::DRAW) { return 0.0f; }
    if (depth == 0) { return evaluator_.evaluate(); }

    auto moves = generateMoveList();
    if (moves.empty()) { return evaluator_.evaluate(); }

    float bestScore = -MATE_SCORE - 1;
    for (auto& move: moves) {
        internalBoardManager_.forceMove(move);
        float score = -negamax(depth - 1, ply + 1);
        internalBoardManager_.undoMove();

        bestScore = std::max(bestScore, score);

        if (score >= MATE_SCORE - ply) { break; }
    }
    return bestScore;
}

float EngineBase::negamaxWithPV(int depth, int ply, std::vector<Move>& pv){
    pv.clear();

    // Base case: leaf node or game over
    if (internalBoardManager_.getGameResult() & GameResult::CHECKMATE) { return -MATE_SCORE + ply; }
    if (internalBoardManager_.getGameResult() & GameResult::DRAW) { return 0.0f; }
    if (depth == 0) { return evaluator_.evaluate(); }

    auto moves = generateMoveList();
    if (moves.empty()) {
        return evaluator_.evaluate(); // includes stalemate or mate
    }

    float bestScore = -MATE_SCORE - 1;
    Move bestMove;
    std::vector<Move> bestPV;

    for (auto& move: moves) {
        internalBoardManager_.forceMove(move);
        std::vector<Move> thisPV;
        float score = -negamaxWithPV(depth - 1, ply + 1, thisPV);

        internalBoardManager_.undoMove();

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
            bestPV = thisPV;
        }
    }

    if (boardManager()->checkMove(bestMove)) {
        pv.push_back(bestMove);
        pv.insert(pv.end(), bestPV.begin(), bestPV.end());
    }

    return bestScore;
}