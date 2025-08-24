#include <iostream>

#include "GUI/gui.h"
#include "ChessPlayer.h"
#include "HumanPlayer.h"
#include "Engine/EngineBase.h"

#include "GUI/DrawableEntity.h"
#include "GUI/VisualBoard.h"

constexpr int deltaTime = 16;


ChessGui::ChessGui(ChessPlayer* whitePlayer, ChessPlayer* blackPlayer){
    initSDLStuff();
    matchManager_ = std::make_shared<MatchManager>(whitePlayer, blackPlayer);
}

void ChessGui::initSDLStuff(){
    window = SDL_CreateWindow("Chess", 800, 900, 0);
    renderer = SDL_CreateRenderer(window, nullptr);
    running = true;

    visualBoard = std::make_shared<VisualBoard>(Vec2D{800, 800}, this, Vec2D(0, 20));
    registerEntity(visualBoard);

    evaluationBar_ = std::make_shared<EvaluationBar>(Vec2D(0, 0), Vec2D(800, 20));
    registerEntity(evaluationBar_);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);
}

ChessGui::ChessGui(){
    initSDLStuff();
    matchManager_ = std::make_shared<MatchManager>();
}

bool ChessGui::wasInit() const{ return window != nullptr && renderer != nullptr; }

SDL_Renderer *ChessGui::getRenderer() const{ return renderer; }

void ChessGui::registerEntity(const std::shared_ptr<DrawableEntity>& entity){ drawables.push_back(entity); }

void ChessGui::updateGame(const int deltaTime_){
    if (!gameStarted) {
        matchManager_->startGame();
        gameStarted = true;
    }

    runningTime += deltaTime_;
    if (runningTime >= 100) {
        matchManager_->tick();
        runningTime = 0;
    }
}

void ChessGui::loop(){
    if (!wasInit()) { return; }

    while (running) {
        pollEvents();
        render();
        SDL_Delay(deltaTime); // Simulate ~60 FPS
        updateGame(deltaTime);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}

void ChessGui::render() const{
    // draw black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // do the entities
    for (const auto& drawable: drawables) { drawable->draw(renderer); }

    // flip it
    SDL_RenderPresent(renderer);
}

void ChessGui::pollEvents(){
    SDL_Event event;

    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_EVENT_QUIT:
            running = false;
            break;

        case SDL_EVENT_KEY_DOWN:
            handleKeyDown(event.key.key);
            break;

        case SDL_EVENT_KEY_UP:
            handleKeyUp(event.key.key);
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            handleMouseDown(event.button.button);
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:

            handleMouseUp(event.button.button);
            break;

        default:
            break;
    }
}

void ChessGui::handleKeyDown(const SDL_Keycode keycode){
    switch (keycode) {
        case SDLK_ESCAPE:
            running = false;
            break;

        case SDLK_SPACE:
            break;

        case SDLK_LALT:
        case SDLK_LSHIFT:
        case SDLK_LCTRL:
            modifiersSet[keycode] = true;
            break;
        case SDLK_Z:
            if (modifiersSet[SDLK_LCTRL])
                matchManager_->getBoardManager().undoMove();
            break;
        default:
            break;
    }
}

void ChessGui::handleKeyUp(const SDL_Keycode key){
    for (std::pair<SDL_Keycode, bool> keycode: modifiersSet) {
        if (keycode.first == key) { modifiersSet[keycode.first] = false; }
    }
}

void ChessGui::handleMouseDown(const Uint8 button){
    if (matchManager_->currentPlayer()->playerType != HUMAN) { return; }

    switch (button) {
        case SDL_BUTTON_LEFT: {
            float x, y;
            SDL_GetMouseState(&x, &y);
            addMouseClick(x, y);
        }
        default:
            break;
    }
}

void ChessGui::handleMouseUp(const Uint8 button){
    switch (button) {
        case SDL_BUTTON_LEFT: {
            float x;
            float y;
            SDL_GetMouseState(&x, &y);
            addMouseRelease(x, y);
            break;
        }
        default:
            break;
    }
}

RankAndFile ChessGui::getRankAndFile(const int x, const int y) const{
    const int adjustedX = x - static_cast<int>(visualBoard->get_parent_offset().x);
    const int adjustedY = y - static_cast<int>(visualBoard->get_parent_offset().y);
    const int file = 1 + static_cast<int>(adjustedX / (visualBoard->boardSize().x / 8.f));
    const int rank = 1 + static_cast<int>(8 - adjustedY / (visualBoard->boardSize().y / 8.f));

    return {rank, file};
}

void ChessGui::addMouseClick(const int x, const int y){
    if (matchManager_->currentPlayer()->playerType != HUMAN) { return; }

    visualBoard->clear_highlights();
    // where on the screen did we click?
    const auto rankAndFile = getRankAndFile(x, y);
    if (rankAndFile.file < 1 || rankAndFile.file > 8 || rankAndFile.rank < 1 || rankAndFile.rank > 8) { return; }
    const int candidateClickedSquare = rankAndFileToSquare(rankAndFile.rank, rankAndFile.file);

    // check if we clicked on a piece
    const auto clickedPiece = matchManager_->getBoardManager().getBitboards()->getPiece(
        rankAndFile.rank, rankAndFile.file);

    if (clickedPiece.has_value()) {
        // now do something with the human player, check this though
        clickedSquare = candidateClickedSquare;
    }

    visualBoard->highlight_square(rankAndFile);
}

void ChessGui::addMouseRelease(const int x, const int y){
    if (matchManager_->currentPlayer()->playerType != HUMAN) { return; }

    // where did we click?
    const auto rankAndFile = getRankAndFile(x, y);

    if (clickedSquare == -1) { return; } // no piece selected

    const auto candidateMovePiece = matchManager_->getBoardManager().getBitboards()->getPiece(clickedSquare);
    // do we need this? In theory there's deffo a piece there
    if (!candidateMovePiece.has_value()) {
        visualBoard->clear_highlights();
        clickedSquare = -1;
        return;
    }

    auto candidateMove = Move(candidateMovePiece.value(), clickedSquare,
                              rankAndFileToSquare(rankAndFile.rank, rankAndFile.file));

    // move isn't valid
    if (!matchManager_->getBoardManager().checkMove(candidateMove)) {
        visualBoard->clear_highlights();
        clickedSquare = -1;
        return;
    }
    const auto humanPlayer = static_cast<HumanPlayer *>(matchManager_->currentPlayer());
    humanPlayer->addMessage("bestmove " + candidateMove.toUCI());
    visualBoard->highlight_square(rankAndFile);
}