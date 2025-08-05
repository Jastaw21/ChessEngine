//
// Created by jacks on 04/08/2025.
//

#include "../../include/EngineShared/ZobristHash.h"
#include <chrono>

ZobristHash::ZobristHash() : rng(std::chrono::system_clock::now().time_since_epoch().count()){}
uint64_t ZobristHash::hash(const std::string& fen){ return 0; }