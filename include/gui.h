#ifndef GUI_H
#define GUI_H

#include <memory>
#include <vector>
#include <SDL3/SDL.h>


class PieceGUI;

using SDLVec2D = SDL_Point;

class DrawableEntity;
class BoardGUI;

class ChessGui {
public:
    ChessGui();


    [[nodiscard]] bool wasInit();

    void loop();

    void render();

    void registerEntity(DrawableEntity *entity);

    [[nodiscard]] SDL_Renderer *getRenderer();

    inline void setBoardBackground(BoardGUI *background) { board_background_ = background; };
    [[nodiscard]] inline BoardGUI *getBoardBackground() const { return board_background_; }

private:
    bool running;

    SDL_Window *window;
    SDL_Renderer *renderer;

    void handleMouseDown(Uint8 button);
    void handleMouseUp(Uint8 button);

    void pollEvents();

    void handleKeypress(SDL_Keycode keycode);

    std::vector<DrawableEntity *> drawables;

    BoardGUI *board_background_;

    std::shared_ptr<PieceGUI> heldPiece;
};


#endif //GUI_H
