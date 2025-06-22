//
// Created by jacks on 16/06/2025.
//

#include "Engine/EngineBase.h"

EngineBase::EngineBase(const Colours colour): ChessPlayer(colour, PlayerType::ENGINE){}
Move EngineBase::makeMove(){ return createMove(Piece::PIECE_N, "a1a1"); }