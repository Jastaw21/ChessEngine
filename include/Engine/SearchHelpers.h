//
// Created by jacks on 15/10/2025.
//

#include <cmath>
#include <iostream>
#include <vector>

#include "BoardManager/Move.h"
inline float percentOf(int numerator, int denominator){ return denominator > 0 ? 100.f * numerator / denominator : 0; }

struct SearchStatistics {
    int searchID = 0;
    int nodesSearched = 0;
    int depth = 0;

    int endGameExits = 0;
    int nullMoveCutoffs = 0;
    int betaCutoffs = 0;

    int ttProbes = 0;
    int ttHits = 0;
    int ttCutoffs = 0;
    int ttStores = 0;


    int firstMoveCutoffs = 0;

    int pvHashHits = 0;


    void print() const{
        double ebf = std::pow(nodesSearched, 1.0 / depth);
        std::cout << "Search ID: " << searchID << std::endl
                << "Depth: " << depth << std::endl
                << "Nodes:" << nodesSearched << std::endl
                << "ebf: " << ebf << std::endl

                << "tt eval cutoffs: " << ttCutoffs << std::endl
                << "Beta cutoffs: " << betaCutoffs << " " << percentOf(betaCutoffs, nodesSearched) << std::endl
                << "Endgame exits: " << endGameExits << std::endl
                << "Null move cutoffs: " << nullMoveCutoffs << std::endl
                << "Remaining Nodes: " << nodesSearched - ttCutoffs - betaCutoffs - endGameExits - nullMoveCutoffs <<
                std::endl

                << "First move cutoffs: " << firstMoveCutoffs << " " << percentOf(firstMoveCutoffs, betaCutoffs) <<
                std::endl

                << "tt hits: " << ttHits << " " << percentOf(ttHits, ttProbes) << std::endl
                << "tt stores: " << ttStores
                << std::endl;
    }

    float firstMovePercent() const{ return betaCutoffs > 0 ? 100.f * firstMoveCutoffs / betaCutoffs : 0; }
    float cutoffPercent() const{ return betaCutoffs > 0 ? 100.f * betaCutoffs / nodesSearched : 0; }
};

struct SearchResults {
    float score;
    Move bestMove;
    std::vector<Move> variation;

    SearchStatistics stats;
};

struct MoveEvaluations {
    std::vector<Move> moves;
    std::vector<float> scores;

    void reset(){
        moves.clear();
        scores.clear();
    }

    float getScore(const Move& move){
        for (size_t i = 0; i < moves.size(); i++) { if (moves[i].toUCI() == move.toUCI()) { return scores[i]; } }
        return 0;
    }

    void print() const{
        for (size_t i = 0; i < moves.size(); i++) { std::cout << moves[i].toUCI() << " " << scores[i] << std::endl; }
    }
};