#include "HumanPlayer.h"

#include "../include/Engine/ProcessChessEngine.h"
#include "Engine/MainEngine.h"

#include "GUI/gui.h"

void runGame()
{
    auto whitePlayer = HumanPlayer(WHITE);
    auto blackPlayer = MainEngine();

    auto gui = ChessGui(&whitePlayer, &blackPlayer);
    gui.getMatchManager()->setStartingFen(Fen::FULL_STARTING_FEN);
    gui.loop();
}

void runEngineStandaloneGame()
{
    ProcessChessEngine whiteEngine { "StandaloneEngine.exe", "w" };
    whiteEngine.setEngineID("White");

    ProcessChessEngine blackEngine { "StandaloneEngine.exe", "b" };
    blackEngine.setEngineID("Black");

    auto gui = ChessGui(&whiteEngine, &blackEngine);
    gui.getMatchManager()->setStartingFen(Fen::FULL_STARTING_FEN);
    gui.loop();
}

void runEngineGame()
{
    auto whitePlayer = MainEngine();
    whitePlayer.setEngineID("Good Eval");

    auto blackPlayer = MainEngine();
    blackPlayer.setEngineID("Bad Eval");

    auto gui = ChessGui(&whitePlayer, &blackPlayer);
    gui.getMatchManager()->setStartingFen(Fen::FULL_STARTING_FEN);
    gui.loop();
}

void searchTest()
{
    auto engine = MainEngine();
    engine.setFullFen(Fen::FULL_STARTING_FEN);
    engine.Search(15, 10000);
}

int main(int argc, char** argv)
{
    searchTest();
    return 0;
}