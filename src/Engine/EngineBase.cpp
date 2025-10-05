//
// Created by jacks on 16/06/2025.
//

#include "Engine/EngineBase.h"

#include <cmath>
#include <future>

#include "Engine/Evaluation.h"


void EngineBase::SortMoves(std::vector<Move>& moves, const Move& ttMove){
    std::ranges::stable_sort(moves,
                             [&ttMove, this](const Move& a, const Move& b) {
                                 return ScoreMove(a, ttMove) > ScoreMove(b, ttMove);
                             });
}

int EngineBase::ScoreMove(const Move& move, const Move& ttMove){
    // is it the best pv
    if (move == ttMove) { return 1000000; }

    int score = 0;
    if (move.resultBits & MoveResult::CHECK)
        score += 5000;
    if (move.resultBits & MoveResult::PROMOTION)
        score += 1000;
    if (move.resultBits & MoveResult::CAPTURE)
        score += 500;

    return score;
}

EngineBase::EngineBase() : ChessPlayer(ENGINE),
                           rng(std::chrono::system_clock::now().time_since_epoch().count()){
    auto path = std::filesystem::current_path();
    std::string suffix = "";
    suffix += rng() % 26 + 65;
    path += "/searchLog" + suffix + ".txt";
    searchLogStream = std::ofstream(path);

    evaluator_.setBoardManager(&internalBoardManager_);
}

void EngineBase::loadFEN(const std::string& fen){ boardManager()->setFullFen(fen); }

void EngineBase::go(const int depth){
    const auto bestMove = Search(depth).bestMove;
    std::cout << "bestmove " << bestMove.toUCI() << std::endl;
}

void EngineBase::go(const int depth, const int wtime, const int btime, const int winc, const int binc){
    const auto relevantTimeRemaining = internalBoardManager_.getCurrentTurn() == WHITE ? wtime : btime;
    const auto relevantTimeIncrement = internalBoardManager_.getCurrentTurn() == WHITE ? winc : binc;

    const auto thisTimeBudget = relevantTimeRemaining / 20 + relevantTimeIncrement / 2;

    const auto bestMove = Search(depth, thisTimeBudget).bestMove;
    std::cout << "bestmove " << bestMove.toUCI() << std::endl;
}

void EngineBase::parseUCI(const std::string& uci){
    auto command = parser.parse(uci);
    // Create a visitor lambda that captures 'this' and forwards to the command handler
    auto visitor = [this](const auto& cmd) { this->commandHandler(cmd, this); };

    if (!command.has_value()) { return; } // no valid command was parsed
    std::visit(visitor, *command);
}

std::vector<Move> EngineBase::generateMoveList(){ return std::vector<Move>(); }

SearchResults EngineBase::executeSearch(const int depth, const bool timed){
    SearchResults bestResult;
    auto moves = generateMoveList();
    if (moves.empty()) { return bestResult; }

    bestResult.bestMove = moves[0];
    bestResult.score = -MATE_SCORE - 1;

    float alpha = -INFINITY;
    float beta = INFINITY;

    for (auto& move: moves) {
        NodesSearched++;
        internalBoardManager_.forceMove(move);

        std::vector<Move> thisPV;
        float eval = -alphaBeta(depth - 1, alpha, beta, 1, thisPV, timed);
        lastSearchEvaluations.moves.push_back(move);
        lastSearchEvaluations.scores.push_back(eval);

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
    bestResult.hashHits = HashHits;
    bestResult.nodesSearched = NodesSearched;
    return bestResult;
}

SearchResults EngineBase::Search(const int depth){
    lastSearchEvaluations.reset();
    currentSearchID++;
    return executeSearch(depth);
}

SearchResults EngineBase::Search(int MaxDepth, int SearchMs){
    currentSearchID++;
    const int marginSearch = std::max(50, SearchMs - 50);
    deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(marginSearch);

    SearchResults bestResult;
    int maxDepthReached = 0;

    for (int depth = 1; depth <= MaxDepth; depth++) {
        if (std::chrono::steady_clock::now() >= deadline) { break; }
        bestResult = executeSearch(depth, true);
        maxDepthReached = depth;
    }

    bestResult.hashHits = HashHits;
    bestResult.nodesSearched = NodesSearched;
    bestResult.depth = maxDepthReached;

    return bestResult;
}

bool EngineBase::evaluateGameState(const int depth, const int ply, float& value1){
    if (internalBoardManager_.getGameResult() & GameResult::CHECKMATE) {
        value1 = -MATE_SCORE + ply;
        return true;
    }
    if (!internalBoardManager_.getMoveHistory().empty() && internalBoardManager_.getMoveHistory().top().resultBits &
        MoveResult::CHECK_MATE) {
        value1 = -MATE_SCORE + ply;
        return true;
    }
    if (internalBoardManager_.getGameResult() & GameResult::DRAW) {
        value1 = 0.0f;
        return true;
    }
    if (depth == 0) {
        value1 = evaluator_.evaluate();
        return true;
    }
    return false;
}

float EngineBase::alphaBeta(const int depth, float alpha, const float beta, const int ply, std::vector<Move>& pv,
                            const bool timed){
    pv.clear();
    if (internalBoardManager_.getGameResult() & GameResult::CHECKMATE) { return -MATE_SCORE + ply; }
    if (internalBoardManager_.getMoveHistory().size() > 0 && internalBoardManager_.getMoveHistory().top().resultBits &
        MoveResult::CHECK_MATE) { return -MATE_SCORE + ply; }
    if (internalBoardManager_.getGameResult() & GameResult::DRAW) { return 0.0f; }
    if (depth == 0) { return evaluator_.evaluate(); }

    auto hash = boardManager()->getZobristHash()->getHash();
    auto ttEntry = transpositionTable_.retrieveVector(hash);

    Move ttMove;
    if (ttEntry.has_value()) {
        if (ttEntry->depth >= depth) {
            HashHits++;
            return ttEntry->eval;
        }
        ttMove = ttEntry->bestMove;
    }

    auto moves = generateMoveList();
    SortMoves(moves, ttMove);

    if (moves.empty())
        return evaluator_.evaluate();

    float bestScore = -MATE_SCORE - 1;
    Move bestMove;
    std::vector<Move> bestPV;

    for (auto& move: moves) {
        if (timed && std::chrono::steady_clock::now() >= deadline) {
            return 0.0f; // bail inside loop
        }
        NodesSearched++;
        // push the move onto the board
        internalBoardManager_.forceMove(move);
        std::vector<Move> thisPV;

        float eval = -alphaBeta(depth - 1, -beta, -alpha, ply + 1, thisPV, timed);

        internalBoardManager_.undoMove();

        if (eval > bestScore) {
            bestScore = eval;
            bestMove = move;
            bestPV = thisPV;
        }

        if (eval > alpha) { alpha = eval; }

        if (alpha >= beta) { break; }
    }
    TTEntry newEntry{
                .key = hash,
                .eval = bestScore,
                .bestMove = bestMove,
                .depth = depth,
                .age = currentSearchID // Track which search this is from
            };
    transpositionTable_.storeVector(newEntry);

    if (bestScore > -MATE_SCORE - 1) {
        pv.push_back(bestMove);
        pv.insert(pv.end(), bestPV.begin(), bestPV.end());
    }

    return bestScore;
}


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
            if (move.resultBits & PROMOTION) {
                // spacing comment
                result.promotions++;
            }
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

PerftResults EngineBase::runPerftTest(const std::string& Fen, const int depth){
    internalBoardManager_.getBitboards()->setFenPositionOnly(Fen);
    return perft(depth);
}

std::vector<PerftResults> EngineBase::runDivideTest(const std::string& Fen, const int depth){
    internalBoardManager_.getBitboards()->setFenPositionOnly(Fen);
    return perftDivide(depth);
}

std::vector<PerftResults> EngineBase::runDivideTest(const int depth){ return perftDivide(depth); }