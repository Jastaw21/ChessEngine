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
    ProcessChessEngine whiteEngine{"StandaloneEngine.exe", "w"};
    whiteEngine.setEngineID("White");

    ProcessChessEngine blackEngine{"StandaloneEngine.exe", "b"};
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