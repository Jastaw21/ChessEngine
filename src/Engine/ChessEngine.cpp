//
// Created by jacks on 16/06/2025.
//

#include "Engine/ChessEngine.h"

#include <cmath>
#include <future>

#include "BoardManager/Referee.h"

#include "Engine/Evaluation.h"
#include "Engine/MoveGenerator.h"

int getPieceValue(Piece piece){
    switch (piece) {
        case WP:
        case BP: return 1;
        case WN:
        case BN: return 3;
        case WB:
        case BB: return 3;
        case WR:
        case BR: return 5;
        case WQ:
        case BQ: return 9;
        case WK:
        case BK: return 0;
    }
    return 0;
}

void ChessEngine::SortMoves(std::vector<Move>& moves, const Move& ttMove){
    std::ranges::stable_sort(moves,
                             [&](const Move& a, const Move& b) {
                                 int scoreA = 0;
                                 int scoreB = 0;

                                 if (a == ttMove) { return true; }
                                 if (b == ttMove) { return false; }

                                 if (a.resultBits & MoveResult::CAPTURE) {
                                     Piece victim = a.capturedPiece;
                                     Piece attacker = a.piece;

                                     scoreA = 10 * (getPieceValue(victim) - getPieceValue(attacker));
                                 }
                                 if (b.resultBits & MoveResult::CAPTURE) {
                                     Piece victim = b.capturedPiece;
                                     Piece attacker = b.piece;

                                     scoreB = 10 * (getPieceValue(victim) - getPieceValue(attacker));
                                 }

                                 return scoreA > scoreB;
                             });
}

ChessEngine::ChessEngine() : ChessPlayer(ENGINE),
                             rng(std::chrono::system_clock::now().time_since_epoch().count()){
    auto path = std::filesystem::current_path();
    std::string suffix = "";
    suffix += rng() % 26 + 65;
    path += "/searchLog" + suffix + ".txt";
    searchLogStream = std::ofstream(path);

    evaluator_.setBoardManager(&internalBoardManager_);
}

void ChessEngine::loadFEN(const std::string& fen){ boardManager()->setFullFen(fen); }

void ChessEngine::go(const int depth){
    const auto bestMove = Search(depth).bestMove;
    std::cout << "bestmove " << bestMove.toUCI() << std::endl;
}

void ChessEngine::go(const int depth, const int wtime, const int btime, const int winc, const int binc){
    const auto relevantTimeRemaining = internalBoardManager_.getCurrentTurn() == WHITE ? wtime : btime;
    const auto relevantTimeIncrement = internalBoardManager_.getCurrentTurn() == WHITE ? winc : binc;

    const auto thisTimeBudget = relevantTimeRemaining / 20 + relevantTimeIncrement / 2;

    const auto bestMove = Search(depth, thisTimeBudget).bestMove;
    std::cout << "bestmove " << bestMove.toUCI() << std::endl;
}

void ChessEngine::parseUCI(const std::string& uci){
    auto command = parser.parse(uci);
    // Create a visitor lambda that captures 'this' and forwards to the command handler
    auto visitor = [this](const auto& cmd) { this->commandHandler(cmd, this); };

    if (!command.has_value()) { return; } // no valid command was parsed
    std::visit(visitor, *command);
}


std::vector<Move> ChessEngine::generateMoveList(){ return MoveGenerator::getMoves(internalBoardManager_); }

bool ChessEngine::sendCommand(const std::string& command){
    parseUCI(command);
    return true;
}

std::string ChessEngine::readResponse(){ return ""; }

SearchResults ChessEngine::executeSearch(const int depth, const bool timed){
    SearchResults bestResult;
    auto moves = MoveGenerator::getMoves(internalBoardManager_);
    if (moves.empty()) { return bestResult; }

    bestResult.bestMove = moves[0];
    bestResult.score = -MATE_SCORE - 1;

    float alpha = -INFINITY;
    float beta = INFINITY;

    for (auto& move: moves) {
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

    currentSearchStats.depth = depth;
    bestResult.stats = currentSearchStats;
    return bestResult;
}

SearchResults ChessEngine::Search(const int depth){
    lastSearchEvaluations.reset();
    currentSearchStats.searchID++;;
    return executeSearch(depth);
}

SearchResults ChessEngine::Search(int MaxDepth, int SearchMs){
    currentSearchStats.searchID++;
    const int marginSearch = std::max(50, SearchMs - 50);
    deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(marginSearch);

    SearchResults bestResult;
    int maxDepthReached = 0;

    for (int depth = 1; depth <= MaxDepth; depth++) {
        if (std::chrono::steady_clock::now() >= deadline) { break; }
        bestResult = executeSearch(depth, true);
        maxDepthReached = depth;
    }

    currentSearchStats.depth = maxDepthReached;
    bestResult.stats = currentSearchStats;

    return bestResult;
}

std::optional<float> ChessEngine::evaluateGameState(const int depth, const int ply, const int boardStatus){
    if (boardStatus & BoardStatus::BLACK_CHECKMATE || boardStatus & WHITE_CHECKMATE) {
        currentSearchStats.endGameExits++;
        return -MATE_SCORE + ply;
    }

    if (internalBoardManager_.getGameResult() & GameResult::DRAW) { return 0.0f; }
    if (depth == 0) { return evaluator_.evaluate(); }
    return std::nullopt;
}


bool ChessEngine::performNullMoveReduction(const int depth, const float beta, const int ply, const bool timed,
                                           float& evaluatedValue){
    const int reduction = 3;
    internalBoardManager_.makeNullMove();

    std::vector<Move> nullPV;
    float nullScore = -alphaBeta(depth - reduction, -beta, -beta + 1,
                                 ply + 1, nullPV, timed, false); // Don't allow nested nulls
    internalBoardManager_.makeNullMove();

    if (nullScore >= beta) {
        currentSearchStats.nullMoveCutoffs++;
        evaluatedValue = beta;
        return true; // Fail-high cutoff
    }

    return false;
}

bool ChessEngine::getTranspositionTableValue(const int depth, Move& ttMove, float& evalResult){
    auto hash = boardManager()->getZobristHash()->getHash();
    auto ttEntry = transpositionTable_.retrieveVector(hash);
    currentSearchStats.ttProbes++;

    if (ttEntry.has_value() && ttEntry->depth >= depth) {
        currentSearchStats.ttCutoffs++;

        TTEntry refreshedEntry = ttEntry.value();
        refreshedEntry.age = currentSearchStats.searchID;
        transpositionTable_.storeVector(refreshedEntry);

        evalResult = ttEntry->eval;
        return true;
    }

    if (ttEntry.has_value()) { ttMove = ttEntry->bestMove; }
    return false;
}

void ChessEngine::storeTranspositionTableEntry(const int depth, float bestScore, Move bestMove){
    TTEntry newEntry{
                .key = boardManager()->getZobristHash()->getHash(),
                .eval = bestScore,
                .bestMove = bestMove,
                .depth = depth,
                .age = currentSearchStats.searchID // Track which search this is from
            };
    transpositionTable_.storeVector(newEntry);
    currentSearchStats.ttStores++;
}

float ChessEngine::performSearchLoop(std::vector<Move>& moves, const int depth, float alpha, const float beta,
                                     const int ply, const bool timed, std::vector<Move>& pv){
    float bestScore = -MATE_SCORE - 1;
    Move bestMove;
    std::vector<Move> bestPV;

    bool isFirstMove = true;
    for (auto& move: moves) {
        if (timed && std::chrono::steady_clock::now() >= deadline) {
            return 0.0f; // bail inside loop
        }

        // push the move onto the board
        internalBoardManager_.forceMove(move);
        std::vector<Move> thisPV;
        float eval = -alphaBeta(depth - 1, -beta, -alpha, ply + 1, thisPV, timed, true);
        internalBoardManager_.undoMove();

        if (eval > bestScore) {
            bestScore = eval;
            bestMove = move;
            bestPV = thisPV;
        }

        if (eval > alpha) { alpha = eval; }

        if (alpha >= beta) {
            currentSearchStats.betaCutoffs++;
            if (isFirstMove) { currentSearchStats.firstMoveCutoffs++; }
            break;
        }
        isFirstMove = false;
    }
    storeTranspositionTableEntry(depth, bestScore, bestMove);

    if (bestScore > -MATE_SCORE - 1) {
        pv.push_back(bestMove);
        pv.insert(pv.end(), bestPV.begin(), bestPV.end());
    }

    return bestScore;
}

float ChessEngine::alphaBeta(const int depth, float alpha, const float beta, const int ply, std::vector<Move>& pv,
                             const bool timed, const bool nullMoveAllowed){
    pv.clear();

    // if the game is over, or bottom depth - exit now
    const auto status = Referee::checkBoardStatus(
        *internalBoardManager_.getBitboards(),
        *internalBoardManager_.getMagicBitBoards(),
        internalBoardManager_.getCurrentTurn()
    );

    if (auto endGameEvaluation = evaluateGameState(depth, ply, status);
        endGameEvaluation.has_value()) { return endGameEvaluation.value(); }

    // query the transposition table
    Move ttMove;
    if (float evalResult; getTranspositionTableValue(depth, ttMove, evalResult)) { return evalResult; }

    // null move reductions
    if (nullMoveAllowed && depth >= 3 && !(status & (BoardStatus::BLACK_CHECK | BoardStatus::WHITE_CHECK))) {
        float evaluatedValue;
        if (performNullMoveReduction(depth, beta, ply, timed, evaluatedValue)) { return evaluatedValue; }
    }

    auto moves = MoveGenerator::getMoves(internalBoardManager_);
    SortMoves(moves, ttMove);

    if (moves.empty()) {
        currentSearchStats.endGameExits++;
        return evaluator_.evaluate();
    }

    currentSearchStats.nodesSearched++;
    return performSearchLoop(moves, depth, alpha, beta, ply, timed, pv);
}


PerftResults ChessEngine::perft(const int depth){
    if (depth == 0) return PerftResults{1, 0, 0, 0, 0, 0};

    PerftResults result{0, 0, 0, 0, 0, 0};
    auto moves = MoveGenerator::getMoves(internalBoardManager_);

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

std::vector<PerftResults> ChessEngine::perftDivide(const int depth){
    auto moves = MoveGenerator::getMoves(internalBoardManager_);
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

int ChessEngine::simplePerft(const int depth){
    if (depth == 0)
        return 1;

    int nodes = 0;
    auto moves = MoveGenerator::getMoves(internalBoardManager_);
    for (Move& move: moves) {
        internalBoardManager_.forceMove(move);
        nodes += simplePerft(depth - 1);
        internalBoardManager_.undoMove();
    }

    return nodes;
}

PerftResults ChessEngine::runPerftTest(const std::string& Fen, const int depth){
    internalBoardManager_.getBitboards()->setFenPositionOnly(Fen);
    return perft(depth);
}

std::vector<PerftResults> ChessEngine::runDivideTest(const std::string& Fen, const int depth){
    internalBoardManager_.getBitboards()->setFenPositionOnly(Fen);
    return perftDivide(depth);
}

std::vector<PerftResults> ChessEngine::runDivideTest(const int depth){ return perftDivide(depth); }