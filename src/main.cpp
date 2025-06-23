#include <iostream>

#include "HumanPlayer.h"
#include "BoardManager/BitBoards.h"
#include "Engine/TestEngine.h"
#include "GUI/gui.h"
#include "GUI/VisualBoard.h"


int main(int argc, char** argv){

    auto whitePlayer = HumanPlayer(Colours::WHITE);
    auto blackPlayer = HumanPlayer(Colours::BLACK);
    auto gui = ChessGui(&whitePlayer, &blackPlayer);


    gui.getBoardManager()->getBitboards()->loadFEN(Fen::STARTING_FEN);
    gui.loop();
    return 0;
}