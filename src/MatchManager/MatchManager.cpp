//
// Created by jacks on 19/07/2025.
//

#include "../../include/MatchManager/MatchManager.h"

#include <filesystem>
#include <fstream>
#include "EngineShared/CommunicatorBase.h"

MatchManager::MatchManager(ChessPlayer* startingPlayer, ChessPlayer* otherPlayer){
    currentPlayer_ = startingPlayer;
    otherPlayer_ = otherPlayer;
    const auto currentCommunicator = new MatchManagerCommunicator(this);
    currentPlayer_->setCommunicator(currentCommunicator);
    const auto otherCommunicator = new MatchManagerCommunicator(this);
    otherPlayer_->setCommunicator(otherCommunicator);
}

void MatchManager::startGame(){
    if (currentPlayer_ == nullptr || otherPlayer_ == nullptr)
        return;

    if (currentPlayer_->playerType == HUMAN) { return; }

    currentPlayer()->parseUCI("position " + boardManager.getFullFen());
    currentPlayer()->parseUCI("go");
}

void MatchManager::tick(){
    if (gamesPlayed == gamesToPlay) { std::cout << "Game count: " << gamesPlayed << std::endl; }
    if (boardManager.isGameOver()) {
        auto result = boardManager.getGameResult();
        std::string gameResult = "";
        if (result & DRAW) { draws++; }
        if (result & WHITE_WINS) { whiteWins++; }
        if (result & BLACK_WINS) { blackWins++; }

        std::cout << "Game Over " << "W: " << whiteWins << " B: " << blackWins << " D: " << draws << std::endl;

        gamesPlayed++;
        restartGame();
        return;
    }
    while (!messageQueueOutbound_.empty()) {
        currentPlayer()->parseUCI(messageQueueOutbound_.front());
        messageQueueOutbound_.pop();
    }

    while (!messageQueueInbound_.empty()) {
        parseUCI(messageQueueInbound_.front());
        messageQueueInbound_.pop();
    }
}


void MatchManager::parseUCI(const std::string& uci){
    auto command = parser.parse(uci);
    // Create a visitor lambda that captures 'this' and forwards to command handler
    auto visitor = [this](const auto& cmd) { this->commandHandler(cmd, this); };
    std::visit(visitor, *command);
}

void MatchManager::restartGame(){
    dumpGameLog();
    boardManager.resetGame(startingFen()); // reset our internal state

    // tell both players it's a new game
    currentPlayer()->parseUCI("ucinewgame");
    otherPlayer()->parseUCI("ucinewgame");
    while (!messageQueueOutbound_.empty()) { messageQueueOutbound_.pop(); }
    while (!messageQueueInbound_.empty()) { messageQueueInbound_.pop(); }
    startGame();

    std::swap(currentPlayer_, otherPlayer_);
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