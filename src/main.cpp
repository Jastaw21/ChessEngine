#include "HumanPlayer.h"
#include "BoardManager/BitBoards.h"
#include "Engine/TestEngine.h"
#include "GUI/gui.h"
#include "GUI/VisualBoard.h"


int main(int argc, char** argv){

    auto whitePlayer = HumanPlayer(WHITE);
    auto blackPlayer = TestEngine(BLACK);
    //auto blackPlayer = HumanPlayer(BLACK);
    auto gui = ChessGui(&whitePlayer, &blackPlayer);
    blackPlayer.setManager(gui.getBoardManager());


    gui.getBoardManager()->getBitboards()->loadFEN(Fen::STARTING_FEN);
    gui.loop();
    return 0;
}