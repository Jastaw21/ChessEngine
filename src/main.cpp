#include <bitset>

#include "gui.h"
#include <SDL3/SDL.h>
#include "SDL3/SDL_main.h"
#include "VisualBoard.h"
#include "TestEngine.h"
#include "BitBoards.h"


int main(int argc, char **argv) {
    auto engine = TestEngine();
    auto gui = ChessGui(&engine);

    auto bb = BitBoards();
    bb.loadFEN(Fen::STARTING_FEN);

    std::cout <<  bb.toFEN();




    gui.loop();
    return 0;
}
