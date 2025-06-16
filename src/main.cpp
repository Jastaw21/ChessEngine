#include "gui.h"
#include <SDL3/SDL.h>
#include "SDL3/SDL_main.h"
#include "BoardGUI.h"
#include "PieceGUI.h"
#include "Fen.h"


int main(int argc, char **argv) {
    auto gui = ChessGui();

    constexpr auto boardSize = Vec2D(800.f, 800.f);

    gui.loop();

    return 0;
}
