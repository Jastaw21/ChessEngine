#include <bitset>

#include "GUI/gui.h"
#include <SDL3/SDL.h>
#include "SDL3/SDL_main.h"
#include "GUI/VisualBoard.h"
#include "Engine/TestEngine.h"
#include "BoardManager/BitBoards.h"


int main(int argc, char **argv){
    auto engine = TestEngine();
    auto gui = ChessGui(&engine);

    auto eng = gui.getEngine();
    int i = 1 + 8;

    gui.getBoardManager()->getBitboards()->loadFEN(Fen::STARTING_FEN);

    gui.loop();
    return 0;
}
