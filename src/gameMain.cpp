#include "HumanPlayer.h"
#include "BoardManager/BitBoards.h"
#include "Engine/TestEngine.h"
#include "GUI/gui.h"
#include "GUI/VisualBoard.h"

void runGame(){
    auto whitePlayer = HumanPlayer(WHITE);

    auto blackPlayer = TestEngine();

    auto gui = ChessGui(&whitePlayer, &blackPlayer);
    gui.getMatchManager()->getBitboards()->setFenPositionOnly(Fen::STARTING_FEN);
    gui.loop();
}

void runEngineGame(){
    auto whitePlayer = TestEngine();
    auto blackPlayer = TestEngine();

    auto gui = ChessGui(&whitePlayer, &blackPlayer);
    gui.getMatchManager()->setStartingFen("rnbqkbnr/pppp1ppp/8/4p3/6P1/5P2/PPPPP2P/RNBQKBNR b KQkq - 0 1");
    gui.loop();
}

void testSearch(){
    auto engine = TestEngine();
    engine.loadFEN(Fen::FULL_STARTING_FEN);

    auto result = engine.search(3);
}

int main(int argc, char** argv){
    //testSearch();
    runEngineGame();
    return 0;
}