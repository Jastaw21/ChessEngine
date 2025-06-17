#ifndef GUI_H
#define GUI_H

#include <memory>
#include <vector>
#include <SDL3/SDL.h>


class VisualPiece;



class DrawableEntity;
class VisualBoard;
class EngineBase;

class ChessGui {
public:
    ChessGui();
    ChessGui(EngineBase* engine);


    [[nodiscard]] bool wasInit() const;

    void loop();

    void render();

    void registerEntity(DrawableEntity *entity);



    [[nodiscard]] SDL_Renderer *getRenderer() const;

    inline void setBoardBackground(VisualBoard *background) { board_background_ = background; };
    [[nodiscard]] inline VisualBoard *getBoardBackground() const { return board_background_; }

private:
    bool running;

    SDL_Window *window;
    SDL_Renderer *renderer;
    EngineBase* engine_;

    void handleMouseDown(Uint8 button);
    void handleMouseUp(Uint8 button) const;

    void pollEvents();

    void handleKeypress(SDL_Keycode keycode);

    std::vector<DrawableEntity *> drawables;

    VisualBoard *board_background_;

    std::shared_ptr<VisualPiece> heldPiece;
};


#endif //GUI_H
