#ifndef GUI_H
#define GUI_H

#include <memory>
#include <vector>
#include <SDL3/SDL.h>

#include "BoardManager/BoardManager.h"


class VisualPiece;
class DrawableEntity;
class VisualBoard;

class BoardManager;
class EngineBase;

class ChessGui {
public:
    ChessGui();

    explicit ChessGui(EngineBase *engine);


    [[nodiscard]] bool wasInit() const;

    void loop();
    void render();
    void registerEntity(DrawableEntity *entity);
    [[nodiscard]] EngineBase *getEngine() const;
    [[nodiscard]] SDL_Renderer *getRenderer() const;

    inline void setBoardBackground(VisualBoard *background) { board_background_ = background; }

    BoardManager* getBoardManager(){return &manager_;};
    [[nodiscard]] inline VisualBoard *getBoardBackground() const { return board_background_; }

private:
    // loop stuff
    bool running;

    void pollEvents();

    // drawing
    SDL_Window *window;
    SDL_Renderer *renderer;
    std::vector<DrawableEntity *> drawables;
    VisualBoard *board_background_;

    // chess engine
    EngineBase *engine_;
    BoardManager manager_;

    // event handling
    void handleMouseDown(Uint8 button);
    void handleMouseUp(Uint8 button);
    void handleKeypress(SDL_Keycode keycode);
    std::shared_ptr<VisualPiece> heldPiece;
};


#endif //GUI_H
