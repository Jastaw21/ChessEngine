//
// Created by jacks on 16/06/2025.
//

#include "Engine/EngineBase.h"

void CommandHandlerBase::operator()(const UCICommand& cmd){}
EngineBase::EngineBase(const Colours colour): ChessPlayer(colour, ENGINE){}
Move EngineBase::makeMove(){ return createMove(PIECE_N, "a1a1"); }
void EngineBase::parseUCI(const std::string& uci){ auto result = parser.parse(uci); }

