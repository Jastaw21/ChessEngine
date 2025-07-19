//
// Created by jacks on 19/07/2025.
//

#ifndef MATCHMANAGER_H
#define MATCHMANAGER_H

#include "ChessPlayer.h"
#include "BoardManager/BoardManager.h"
#include "BoardManager/ManagerCommandHandler.h"
#include "EngineShared/UCIParsing/UciParser.h"
#include "Utility/Fen.h"

class MatchManager {
public:

    MatchManager() = default;

    MatchManager(ChessPlayer* startingPlayer, ChessPlayer* otherPlayer,
                 const std::string& startingFen) : currentPlayer(startingPlayer),
                                                   otherPlayer(otherPlayer),
                                                   startingFen_(startingFen){}

    MatchManager(ChessPlayer* startingPlayer, ChessPlayer* otherPlayer) : currentPlayer(startingPlayer),
                                                                          otherPlayer(otherPlayer){}

    void tick();

    void parseUCI(const std::string& uci);
    void sendCommand(Command cmd);
    std::stack<Move> &getMoveHistory();
    void swapPlayers(){ std::swap(currentPlayer, otherPlayer); };
    ChessPlayer* currentPlayer = nullptr;
    ChessPlayer* otherPlayer = nullptr;

    void addMove(const std::string& moveUCI);

    BoardManager &getBoardManager(){ return boardManager; }
    BitBoards *getBitboards(){ return boardManager.getBitboards(); }

private:

    UCIParser parser;
    ManagerCommandHandler commandHandler;
    BoardManager boardManager;

    std::string startingFen_ = Fen::FULL_STARTING_FEN;

public:

    [[nodiscard]] std::string startingFen() const{ return startingFen_; }
};


#endif //MATCHMANAGER_H
