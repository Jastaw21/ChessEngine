#include "HumanPlayer.h"
#include "BoardManager/BitBoards.h"
#include "CLIEngine/StandaloneUciPlayer.h"
#include "Engine/MainEngine.h"
#include "EngineShared/CommunicatorBase.h"
#include "GUI/gui.h"


void runGame(){
    auto whitePlayer = HumanPlayer(WHITE);
    auto blackPlayer = MainEngine();
    blackPlayer.setEvaluator(std::make_shared<GoodEvaluator>(blackPlayer.boardManager()));

    auto gui = ChessGui(&whitePlayer, &blackPlayer);
    gui.getMatchManager()->getBitboards()->setFenPositionOnly(Fen::STARTING_FEN);
    gui.loop();
}

void runEngineStandaloneGame(){
    auto whiteEngine =
            StandaloneUCIPlayer(R"(C:\Users\jacks\source\repos\Chess\cmake-build-release\StandaloneEngine.exe)");

    auto blackEngine =
            StandaloneUCIPlayer(R"(C:\Users\jacks\source\repos\Chess\cmake-build-release\StandaloneEngine.exe)");
    auto gui = ChessGui(&whiteEngine, &blackEngine);
    gui.getMatchManager()->setStartingFen(Fen::FULL_STARTING_FEN);

    whiteEngine.setCommunicator(std::make_shared<TerminalCommunicator>(gui.getMatchManager().get(), &whiteEngine));
    blackEngine.setCommunicator(std::make_shared<TerminalCommunicator>(gui.getMatchManager().get(), &blackEngine));
    gui.loop();
}

void runEngineGame(){
    auto whitePlayer = MainEngine();
    whitePlayer.setEngineID("Good Eval");
    whitePlayer.setEvaluator(std::make_shared<GoodEvaluator>(whitePlayer.boardManager()));

    auto blackPlayer = MainEngine();
    blackPlayer.setEngineID("Bad Eval");
    blackPlayer.setEvaluator(std::make_shared<GoodEvaluator>(blackPlayer.boardManager()));

    auto gui = ChessGui(&whitePlayer, &blackPlayer);
    gui.getMatchManager()->setStartingFen(Fen::FULL_STARTING_FEN);
    gui.loop();
}

int main(int argc, char** argv){
    runGame();
    return 0;
}