#ifndef GUI_H
#define GUI_H

#include <memory>
#include <vector>
#include <SDL3/SDL.h>

#include "BoardManager/BoardManager.h"


class VisualPiece;
class DrawableEntity;
class VisualBoard;
class EngineBase;
class ChessPlayer;

inline std::unordered_map<SDL_Keycode, bool> modifiersSet = {
            {SDLK_LCTRL, false}, {SDLK_LSHIFT, false}, {SDLK_LALT, false}
        };

class ChessGui {
public:

    ChessGui();

    explicit ChessGui(EngineBase* engine);
    explicit ChessGui(ChessPlayer* whitePlayer, ChessPlayer* blackPlayer);

    [[nodiscard]] bool wasInit() const;

    [[nodiscard]] EngineBase *getEngine() const;
    [[nodiscard]] SDL_Renderer *getRenderer() const;
    BoardManager *getBoardManager(){ return &boardManager_; };
    [[nodiscard]] inline VisualBoard *getVisualBoard() const{ return visualBoard; }
    void setVisualBoard(VisualBoard* background){ visualBoard = background; }

    void registerEntity(DrawableEntity* entity);

    void loop();

    void addMouseClick(const int x, const int y);
    void addMouseRelease(const int x, const int y);

    [[nodiscard]] int clicked_square() const{ return clickedSquare; }

private:

    // loop stuff
    bool running;

    void pollEvents();
    void render() const;

    // drawing
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<DrawableEntity *> drawables;
    VisualBoard* visualBoard;

    // chess engine
    EngineBase* engine_;
    BoardManager boardManager_;

    ChessPlayer* whitePlayer_;
    ChessPlayer* blackPlayer_;

    // event handling
    void handleMouseDown(Uint8 button);
    void handleMouseUp(Uint8 button);
    void handleKeyDown(SDL_Keycode keycode);
    static void handleKeyUp(SDL_Keycode key);
    std::shared_ptr<VisualPiece> heldPiece;

    int clickedSquare = -1;
};


#endif //GUI_H