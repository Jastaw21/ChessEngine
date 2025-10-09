//
// Created by jacks on 16/06/2025.
//

#include "Engine/EngineBase.h"

#include <cmath>
#include <future>

#include "BoardManager/Referee.h"

#include "Engine/Evaluation.h"

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

void EngineBase::SortMoves(std::vector<Move>& moves, const Move& ttMove){
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

SearchResults EngineBase::Search(const int depth){
    lastSearchEvaluations.reset();
    currentSearchStats.searchID++;;
    return executeSearch(depth);
}

SearchResults EngineBase::Search(int MaxDepth, int SearchMs){
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

bool EngineBase::evaluateGameState(const int depth, const int ply, float& evalValue){
    auto status = Referee::checkBoardStatus(
        *internalBoardManager_.getBitboards(),
        *internalBoardManager_.getMagicBitBoards(),
        internalBoardManager_.getCurrentTurn()
    );

    if (status & BoardStatus::BLACK_CHECKMATE || status & WHITE_CHECKMATE) {
        currentSearchStats.endGameExits++;
        evalValue = -MATE_SCORE + ply;
        return true;
    }
    if (internalBoardManager_.getGameResult() & GameResult::DRAW) {
        currentSearchStats.endGameExits++;
         evalValue = 0.0f;
        return true;
    }
    if (depth == 0) {
        currentSearchStats.endGameExits++;
        evalValue = evaluator_.evaluate();
         return true;
    }

    return false;
}

bool EngineBase::performNullMoveReduction(const int depth, const float beta, const int ply, const bool timed,
                                          float& evaluatedValue){
    const int reduction = 3;
    internalBoardManager_.makeNullMove();

    std::vector<Move> nullPV;
    float nullScore = -alphaBeta(depth - reduction, -beta, -beta + 1,
                                 ply + 1, nullPV, timed, false); // Don't allow nested nulls
    internalBoardManager_.makeNullMove();

    // we can skip a move, and still be better than the beta
    if (nullScore >= beta) {
        currentSearchStats.nullMoveCutoffs++;
        evaluatedValue = beta;
        return true;
    }

    return false;
}

float EngineBase::alphaBeta(const int depth, float alpha, const float beta, const int ply, std::vector<Move>& pv,
                            const bool timed, const bool nullMoveAllowed){

    pv.clear();

    // is there an end game state?
    float earlyExitEval = 0.f;
    if (evaluateGameState(depth, ply, earlyExitEval)){
        return earlyExitEval;
    }

    // null move reduction
    bool inCheck = status & (BoardStatus::BLACK_CHECK | BoardStatus::WHITE_CHECK);
     if (nullMoveAllowed && depth >= 3 && !inCheck) {
        float evaluatedValue;
        if (performNullMoveReduction(depth, beta, ply, timed, evaluatedValue)) { return evaluatedValue; }
    }    

    // retrieve (if we can) this postion
    auto hash = boardManager()->getZobristHash()->getHash();
    auto ttEntry = transpositionTable_.retrieveVector(hash);
    currentSearchStats.ttProbes++;

    Move ttMove;

    // found an entry, and it's searched deeper than this, use it blindly
    if (ttEntry.has_value() && ttEntry->depth >= depth) {
        currentSearchStats.ttCutoffs++;
        TTEntry refreshedEntry = ttEntry.value();
        refreshedEntry.age = currentSearchStats.searchID; // keep this node alive
        transpositionTable_.storeVector(refreshedEntry);

        return ttEntry->eval;
    }

    // use the work done before for move ordering
    if (ttEntry.has_value()) { ttMove = ttEntry->bestMove; }   
    
    currentSearchStats.nodesSearched++;
    auto moves = generateMoveList();
    SortMoves(moves, ttMove);

    // no possible moves - guess this is rare? TODO: Add emptyMove debugging
    if (moves.empty()) {
        currentSearchStats.noValidMovesFound++;
        return evaluator_.evaluate();
    }

    float bestScore = -MATE_SCORE - 1;
    Move bestMove;
    std::vector<Move> bestPV;

    bool isFirstMove = true; // so we can track first move cutoffs
    for (auto& move: moves) {
        
        // timed out.
        if (timed && std::chrono::steady_clock::now() >= deadline) {
            return 0.0f; // bail inside loop
        }

        // push the move onto the board
        internalBoardManager_.forceMove(move);

        // recursively search it
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

    // store this search
    TTEntry newEntry{
                .key = hash,
                .eval = bestScore,
                .bestMove = bestMove,
                .depth = depth,
                .age = currentSearchStats.searchID 
            };
    transpositionTable_.storeVector(newEntry);
    currentSearchStats.ttStores++;

    // build up the PV
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
