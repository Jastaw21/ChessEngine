//
// Created by jacks on 19/07/2025.
//

#include "../../include/MatchManager/MatchManager.h"

#include <filesystem>
#include <fstream>
#include "EngineShared/CommunicatorBase.h"
#include "GUI/gui.h"

MatchManager::MatchManager(ChessPlayer* startingPlayer, ChessPlayer* otherPlayer){
    currentPlayer_ = startingPlayer;
    otherPlayer_ = otherPlayer;

    whitePlayer = startingPlayer;
    blackPlayer = otherPlayer;

    const auto currentCommunicator = std::make_shared<CommunicatorBase>(this, currentPlayer_);
    currentPlayer_->setCommunicator(currentCommunicator);
    const auto otherCommunicator = std::make_shared<CommunicatorBase>(this, otherPlayer_);
    otherPlayer_->setCommunicator(otherCommunicator);
}

void MatchManager::startGame(){
    if (currentPlayer_ == nullptr || otherPlayer_ == nullptr)
        return;

    if (currentPlayer_->playerType == HUMAN) { return; }

    currentPlayer()->parseUCI("position " + boardManager.getFullFen());
    currentPlayer()->parseUCI("go");
}

void MatchManager::processGameResult(){
    auto result = boardManager.getGameResult();
    std::string gameResult = "";
    if (result & DRAW) { draws++; }
    if (result & WHITE_WINS) { whiteWins++; }
    if (result & BLACK_WINS) { blackWins++; }

    std::cout << "Game Over " << "W: " << whiteWins << " B: " << blackWins << " D: " << draws << std::endl;

    gamesPlayed++;
    restartGame();
}

void MatchManager::tick(){
    if (gamesPlayed == gamesToPlay) { std::cout << "Game count: " << gamesPlayed << std::endl; }
    if (boardManager.isGameOver()) {
        processGameResult();
        return;
    }
    if (const auto message = currentPlayer_->consumeMessage(); message.has_value()) {
        std::cout << message.value() << std::endl;
        // handle the inbound command
        parseUCI(message.value());
    }
}

void MatchManager::parseUCI(const std::string& uci){
    auto command = parser.parse(uci);
    // Create a visitor lambda that captures 'this' and forwards to command handler
    auto visitor = [this](const auto& cmd) { this->commandHandler(cmd, this); };
    std::visit(visitor, *command);
}

void MatchManager::notifyGUIofMove(const Move& move){}

void MatchManager::restartGame(){
    dumpGameLog();
    boardManager.resetGame(startingFen()); // reset our internal state

    currentPlayer_ = whitePlayer;
    otherPlayer_ = blackPlayer;
    // tell both players it's a new game
    currentPlayer()->parseUCI("ucinewgame");
    currentPlayer()->getCommunicator()->resetQueues();
    otherPlayer()->parseUCI("ucinewgame");
    otherPlayer()->getCommunicator()->resetQueues();
    startGame();
}

void MatchManager::dumpGameLog(){
    auto path = std::filesystem::current_path();
    path += "/gameLog.txt";
    std::ofstream file(path, gamesPlayed == 1 ? std::ios::trunc : std::ios::app);

    BoardManager localBoardManager = boardManager;
    localBoardManager.setFullFen(startingFen());

    file << "New Game:" << gamesPlayed << " startpos " << startingFen() << std::endl;
    auto staticMoveHistory = boardManager.getMoveHistory();
    std::vector<Move> moves;
    while (!staticMoveHistory.empty()) {
        moves.push_back(staticMoveHistory.top());
        staticMoveHistory.pop();
    }
    int i = moves.size() - 1;
    while (i >= 0) {
        std::string moveOutput;
        moveOutput += "G:" + std::to_string(gamesPlayed);
        moveOutput += " " + localBoardManager.getFullFen() + " " + moves[i].toUCI();
        localBoardManager.forceMove(moves[i]);
        file << moveOutput << std::endl;
        i--;
    }
}


std::stack<Move> &MatchManager::getMoveHistory(){ return boardManager.getMoveHistory(); }
void MatchManager::addMove(const std::string& moveUCI){ boardManager.tryMove(moveUCI); }