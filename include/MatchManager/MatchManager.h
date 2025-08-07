//
// Created by jacks on 19/07/2025.
//

#ifndef MATCHMANAGER_H
#define MATCHMANAGER_H

#include <queue>

#include "ChessPlayer.h"
#include "ManagerCommandHandler.h"
#include "BoardManager/BoardManager.h"
#include "EngineShared/UCIParsing/UciParser.h"
#include "Utility/Fen.h"

class CommunicatorBase;
using MessageQueue = std::queue<std::string>;


class MatchManager {
public:

    MatchManager() = default;

    MatchManager(ChessPlayer* startingPlayer, ChessPlayer* otherPlayer,
                 const FenString& startingFen) : currentPlayer_(startingPlayer),
                                                 otherPlayer_(otherPlayer),
                                                 startingFen_(startingFen){}

    MatchManager(ChessPlayer* startingPlayer, ChessPlayer* otherPlayer);

    void startGame();
    void tick();

    std::stack<Move> &getMoveHistory();
    void swapPlayers(){ std::swap(currentPlayer_, otherPlayer_); };
    void addMove(const std::string& moveUCI);
    void receiveCommand(const std::string& command){ messageQueueInbound_.push(command); }
    MessageQueue *getMessageQueueOutbound(){ return &messageQueueOutbound_; }
    MessageQueue *getMessageQueueInbound(){ return &messageQueueInbound_; }

    void setStartingFen(const FenString& fen){
        boardManager.setFullFen(fen);
        startingFen_ = fen;
    }

private:

    void parseUCI(const std::string& uci);

    ChessPlayer* currentPlayer_ = nullptr;
    ChessPlayer* otherPlayer_ = nullptr;
    UCIParser parser;
    ManagerCommandHandler commandHandler;
    BoardManager boardManager;

    FenString startingFen_ = Fen::FULL_STARTING_FEN;

    MessageQueue messageQueueOutbound_;
    MessageQueue messageQueueInbound_;

    void restartGame();

    int gamesToPlay = 100;
    int gamesPlayed = 0;
    int whiteWins = 0;
    int blackWins = 0;
    int draws = 0;

public:

    [[nodiscard]] FenString startingFen() const{ return startingFen_; }
    ChessPlayer *currentPlayer() const{ return currentPlayer_; }
    ChessPlayer *otherPlayer() const{ return otherPlayer_; }
    void setCurrentPlayer(ChessPlayer* current_player){ currentPlayer_ = current_player; }
    void setOtherPlayer(ChessPlayer* other_player){ otherPlayer_ = other_player; }
    BoardManager &getBoardManager(){ return boardManager; }
    BitBoards *getBitboards(){ return boardManager.getBitboards(); }
};


#endif //MATCHMANAGER_H
