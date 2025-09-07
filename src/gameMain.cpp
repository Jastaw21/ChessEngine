#include "HumanPlayer.h"

#include "Engine/MainEngine.h"
#include "EngineShared/ProcessChessEngine.h"

#include "GUI/gui.h"


void runGame(){
    auto whitePlayer = HumanPlayer(WHITE);
    auto blackPlayer = MainEngine();

    auto gui = ChessGui(&whitePlayer, &blackPlayer);
    gui.getMatchManager()->setStartingFen(Fen::FULL_STARTING_FEN);
    gui.loop();
}

void runEngineStandaloneGame(){
    ProcessChessEngine whiteEngine{"StandaloneEngine.exe", "w"};
    whiteEngine.setEngineID("White");

    ProcessChessEngine blackEngine{"StandaloneEngine.exe", "b"};
    blackEngine.setEngineID("Black");

    auto gui = ChessGui(&whiteEngine, &blackEngine);
    gui.getMatchManager()->setStartingFen(Fen::FULL_STARTING_FEN);
    gui.loop();
}


void runEngineGame(){
    auto whitePlayer = MainEngine();
    whitePlayer.setEngineID("Good Eval");

    auto blackPlayer = MainEngine();
    blackPlayer.setEngineID("Bad Eval");

    auto gui = ChessGui(&whitePlayer, &blackPlayer);
    gui.getMatchManager()->setStartingFen(Fen::FULL_STARTING_FEN);
    gui.loop();
}

int main(int argc, char** argv){
    runEngineStandaloneGame();
    return 0;
}