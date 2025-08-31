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

class ChessGui;
class CommunicatorBase;
using MessageQueue = std::queue<std::string>;


class MatchManager {
public:

    MatchManager() = default;

    MatchManager(ChessPlayer* startingPlayer, ChessPlayer* otherPlayer,
                 const FenString& startingFen) : currentPlayer_(startingPlayer),
                                                 otherPlayer_(otherPlayer),
                                                 whitePlayer(startingPlayer),
                                                 blackPlayer(otherPlayer),
                                                 startingFen_(startingFen){}

    MatchManager(ChessPlayer* startingPlayer, ChessPlayer* otherPlayer);
    ~MatchManager();

    void startGame();
    void processGameResult();
    void tick();

    std::stack<Move> &getMoveHistory();
    void swapPlayers(){ std::swap(currentPlayer_, otherPlayer_); }
    void setCommunicator(CommunicatorBase* communicator_base){ communicator_ = communicator_base; }
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
    void notifyGUIofMove(const Move& move);

    ChessPlayer* currentPlayer_ = nullptr;
    ChessPlayer* otherPlayer_ = nullptr;
    ChessPlayer* whitePlayer = nullptr;
    ChessPlayer* blackPlayer = nullptr;
    UCIParser parser;
    ManagerCommandHandler commandHandler;
    BoardManager boardManager;
    CommunicatorBase* communicator_ = nullptr;
    ChessGui* gui_ = nullptr;

    FenString startingFen_ = Fen::FULL_STARTING_FEN;

    MessageQueue messageQueueOutbound_;
    MessageQueue messageQueueInbound_;

    void restartGame();
    void dumpGameLog();

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
    void setGUI(ChessGui* gui){ gui_ = gui; }
    ChessGui *getGui(){ return gui_; }
};


#endif //MATCHMANAGER_H