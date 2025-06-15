#include "gui.h"
#include <SDL3/SDL.h>
#include "SDL3/SDL_main.h"
#include "BoardGUI.h"
#include "PieceGUI.h"
#include "Fen.h"


int main(int argc, char **argv) {
    auto gui = ChessGui();

    constexpr auto boardSize = SDLVec2D(800, 800);
    auto board = BoardGUI(boardSize, &gui);

    gui.registerEntity(&board);
    gui.loop();

    return 0;
}
