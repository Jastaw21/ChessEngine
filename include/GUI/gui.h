#ifndef GUI_H
#define GUI_H

#include <memory>
#include <vector>
#include <SDL3/SDL.h>

#include "HumanPlayer.h"
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

    explicit ChessGui(ChessPlayer* whitePlayer, ChessPlayer* blackPlayer);
    ChessGui();

    [[nodiscard]] bool wasInit() const;

    [[nodiscard]] SDL_Renderer *getRenderer() const;
    BoardManager *getBoardManager(){ return &boardManager_; }
    void registerEntity(DrawableEntity* entity);


    ChessPlayer *getWhitePlayer() const{ return whitePlayer_; }
    ChessPlayer *getBlackPlayer() const{ return blackPlayer_; }

    HumanPlayer *getWhitePlayerAsHuman() const;
    HumanPlayer *getBlackPlayerAsHuman() const;


    void loop();

    void addMouseClick(int x, int y);
    void addMouseRelease(int x, int y);

    [[nodiscard]] int clicked_square(){ return clickedSquare; }

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

    ChessPlayer *getCurrentPlayer() const;
};


#endif //GUI_H