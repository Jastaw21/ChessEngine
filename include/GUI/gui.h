#ifndef GUI_H
#define GUI_H

#include <memory>
#include <vector>
#include <SDL3/SDL.h>

#include "EvaluationBar.h"
#include "PromotionPiecePicker.h"

#include "BoardManager/BoardManager.h"
#include "MatchManager/MatchManager.h"


class GoodEvaluator;
class VisualPiece;
class DrawableEntity;
class VisualBoard;
class EngineBase;
class ChessPlayer;


inline std::unordered_map<SDL_Keycode, bool> modifiersSet = {
            {SDLK_LCTRL, false}, {SDLK_LSHIFT, false}, {SDLK_LALT, false}
        };


struct Sound {
    SDL_AudioSpec spec;
    std::vector<float> data;
};

class ChessGui {
public:

    explicit ChessGui(ChessPlayer* whitePlayer, ChessPlayer* blackPlayer);
    void initGuiStuff();
    void initSDLStuff();
    void initChessStuff(ChessPlayer* whitePlayer, ChessPlayer* blackPlayer);
    ChessGui();


    [[nodiscard]] bool wasInit() const;

    [[nodiscard]] SDL_Renderer* getRenderer() const;
    std::shared_ptr<MatchManager> getMatchManager(){ return matchManager_; }
    void registerEntity(const std::shared_ptr<DrawableEntity>& entity);
    void updateGame(int deltaTime_);

    void loop();

    void addMouseClick(int x, int y);
    void handleInvalidMove();
    void updateEvaluationBar();
    void addMouseRelease(int x, int y);

    [[nodiscard]] int clicked_square(){ return clickedSquare; }

    void receiveInfoOfEngineMove(const Move& move);

private:

    // loop stuff
    bool running;
    bool bEvaluationDirty = true;
    void pollEvents();
    void render() const;

    // drawing
    SDL_Window* window;
    SDL_Renderer* renderer;

    // visuals
    std::vector<std::shared_ptr<DrawableEntity> > drawables;
    std::shared_ptr<VisualBoard> visualBoard;
    std::shared_ptr<EvaluationBar> evaluationBar_;
    std::shared_ptr<PromotionPiecePicker> picker_;

    // chess game
    std::shared_ptr<MatchManager> matchManager_;
    int runningTime = 0;
    bool gameStarted = false;

    // event handling
    void handleMouseDown(Uint8 button);
    void handleMouseUp(Uint8 button);
    RankAndFile getRankAndFile(int x, int y) const;
    void handleKeyDown(SDL_Keycode keycode);
    static void handleKeyUp(SDL_Keycode key);
    std::shared_ptr<VisualPiece> heldPiece;

    int clickedSquare = -1;

    std::shared_ptr<GoodEvaluator> evaluator_;

    bool bMouseHeld = false;

    SDL_AudioDeviceID audioID;
    Sound movePiecesSound;
    Sound captureSound;
    Sound checkmateSound;
    Sound illegalSound;
    Sound clickSound;
    void playSound(const Sound& sound) const;

    Move outboundMove;
    bool outboundMoveIsReady = false;
    void completeMove();
    void beginMove(const RankAndFile rankAndFile, const Piece candidatePiece);
};


#endif //GUI_H