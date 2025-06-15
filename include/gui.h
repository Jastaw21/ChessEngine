#ifndef GUI_H
#define GUI_H

#include <vector>
#include <SDL3/SDL.h>


class PieceGUI;

using SDLVec2D = SDL_Point;

class DrawableEntity;
class BoardGUI;

class ChessGui {
public:
    ChessGui();


    [[nodiscard]] bool wasInit() const;

    void loop();

    void render() const;

    void registerEntity(DrawableEntity *entity);

    [[nodiscard]] SDL_Renderer *getRenderer() const;

    inline void setBoardBackground(BoardGUI *background) { board_background_ = background; };
    [[nodiscard]] inline BoardGUI *getBoardBackground() const { return board_background_; }

private:
    bool running;

    SDL_Window *window;
    SDL_Renderer *renderer;

    static void handleMouseDown(Uint8 button);

    void pollEvents();

    void handleKeypress(SDL_Keycode keycode);

    std::vector<DrawableEntity *> drawables;

    BoardGUI *board_background_;
};


#endif //GUI_H
