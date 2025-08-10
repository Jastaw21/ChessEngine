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
    whitePlayer.setEvaluator(std::make_shared<GoodEvaluator>(whitePlayer.boardManager()));
    auto blackPlayer = TestEngine();
    blackPlayer.setEvaluator(std::make_shared<BadEvaluator>(blackPlayer.boardManager()));

    auto gui = ChessGui(&whitePlayer, &blackPlayer);
    gui.getMatchManager()->setStartingFen(Fen::FULL_STARTING_FEN);
    gui.loop();
}

int main(int argc, char** argv){
    runEngineGame();
    return 0;
}