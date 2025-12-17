#include "../include/Engine/HumanPlayer.h"

#include "../include/Engine/ProcessChessEngine.h"
#include "Engine/ChessEngine.h"

#include "GUI/gui.h"

void runGame(){
    auto whitePlayer = HumanPlayer(WHITE);
    auto blackPlayer = ChessEngine();

    auto gui = ChessGui(&whitePlayer, &blackPlayer);
    gui.getMatchManager()->setStartingFen(Fen::FULL_STARTING_FEN);
    gui.loop();
}

void runEngineStandaloneGame(){
    ProcessChessEngine whiteEngine{"C:/Users/jacks/source/repos/Chess/Builds/V1.exe", "w"};
    whiteEngine.setEngineID("White");

    ProcessChessEngine blackEngine{"C:/Users/jacks/source/repos/Chess/Builds/V2 Regressed.exe", "b"};
    blackEngine.setEngineID("Black");

    auto gui = ChessGui(&whiteEngine, &blackEngine);
    gui.getMatchManager()->setStartingFen(Fen::FULL_STARTING_FEN);
    gui.loop();
}

void runEngineGame(){
    auto whitePlayer = ChessEngine();
    whitePlayer.setEngineID("Good Eval");

    auto blackPlayer = ChessEngine();
    blackPlayer.setEngineID("Bad Eval");

    auto gui = ChessGui(&whitePlayer, &blackPlayer);
    gui.getMatchManager()->setStartingFen(Fen::FULL_STARTING_FEN);
    gui.loop();
}

void searchTest(){
    auto engine = ChessEngine();
    engine.setFullFen(Fen::FULL_STARTING_FEN);
    engine.Search(15, 10000);
}

int main(int argc, char** argv){
    runEngineStandaloneGame();
    return 0;
}