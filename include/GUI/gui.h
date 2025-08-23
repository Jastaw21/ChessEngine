#ifndef GUI_H
#define GUI_H

#include <memory>
#include <vector>
#include <SDL3/SDL.h>

#include "EvaluationBar.h"
#include "HumanPlayer.h"
#include "BoardManager/BoardManager.h"
#include "MatchManager/MatchManager.h"


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
    void initSDLStuff();
    ChessGui();

    [[nodiscard]] bool wasInit() const;

    [[nodiscard]] SDL_Renderer *getRenderer() const;
    std::shared_ptr<MatchManager> getMatchManager(){ return matchManager_; }
    void registerEntity(const std::shared_ptr<DrawableEntity>& entity);
    void updateGame(int deltaTime);

    void loop();

    void addMouseClick(int x, int y);
    void addMouseRelease(int x, int y) const;

    [[nodiscard]] int clicked_square(){ return clickedSquare; }

private:

    // loop stuff
    bool running;
    void pollEvents();
    void render() const;

    // drawing
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<std::shared_ptr<DrawableEntity> > drawables;
    std::shared_ptr<VisualBoard> visualBoard;
    std::shared_ptr<EvaluationBar> evaluationBar_;

    // chess game
    std::shared_ptr<MatchManager> matchManager_;
    int runningTime = 0;
    bool gameStarted = false;

    // event handling
    void handleMouseDown(Uint8 button);
    void handleMouseUp(Uint8 button) const;
    void handleKeyDown(SDL_Keycode keycode);
    static void handleKeyUp(SDL_Keycode key);
    std::shared_ptr<VisualPiece> heldPiece;

    int clickedSquare = -1;
};


#endif //GUI_H