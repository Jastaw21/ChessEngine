#include <iostream>

#include "GUI/gui.h"
#include "ChessPlayer.h"
#include "HumanPlayer.h"
#include "Engine/EngineBase.h"

#include "GUI/DrawableEntity.h"
#include "GUI/VisualBoard.h"

#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "minimp3_ex.h"

constexpr int deltaTime = 16;


Sound buildAudioStream(const std::string& path){
    Sound sound;
    mp3dec_t mp3dec;
    mp3dec_file_info_t info;
    mp3dec_load(&mp3dec, path.c_str(),
                &info,
                NULL, NULL);

    std::vector<float> float_samples(info.samples);
    for (size_t i = 0; i < info.samples; i++) {
        float_samples[i] = info.buffer[i] / 32768.0f; // Convert int16 to float
    }

    free(info.buffer);

    SDL_AudioSpec sourceSpec;
    sourceSpec.format = SDL_AUDIO_F32;
    sourceSpec.channels = info.channels;
    sourceSpec.freq = info.hz;

    sound.spec = sourceSpec;
    sound.data = float_samples;

    return sound;
}


ChessGui::ChessGui(ChessPlayer* whitePlayer, ChessPlayer* blackPlayer){
    initSDLStuff();
    initChessStuff(whitePlayer, blackPlayer);
}

void ChessGui::intiGuiStuff(){
    visualBoard = std::make_shared<VisualBoard>(Vec2D{800, 800}, this, Vec2D{0, 20});
    registerEntity(visualBoard);

    evaluationBar_ = std::make_shared<EvaluationBar>(Vec2D{0, 0}, Vec2D{800, 20});
    registerEntity(evaluationBar_);
}

void ChessGui::initSDLStuff(){
    window = SDL_CreateWindow("Chess", 800, 900, 0);
    renderer = SDL_CreateRenderer(window, nullptr);
    running = true;

    intiGuiStuff();

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);

    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_F32;
    spec.channels = 2;
    spec.freq = 44100;
    audioID = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    movePiecesSound = buildAudioStream("C:/Users/jacks/source/repos/Chess/resources/sounds/move-self.mp3");
    captureSound = buildAudioStream("C:/Users/jacks/source/repos/Chess/resources/sounds/capture.mp3");
}

void ChessGui::initChessStuff(ChessPlayer* whitePlayer, ChessPlayer* blackPlayer){
    matchManager_ = std::make_shared<MatchManager>(whitePlayer, blackPlayer);
    matchManager_->setGUI(this);
    evaluator_ = std::make_shared<GoodEvaluator>(&matchManager_.get()->getBoardManager());
}

ChessGui::ChessGui(){
    initSDLStuff();
    initChessStuff(nullptr, nullptr);
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

    if (bMouseHeld) {
        float mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        visualBoard->updateHeldPieceLocation(Vec2D{mouseX, mouseY});
    }
}

void ChessGui::loop(){
    if (!wasInit()) { return; }

    while (running) {
        pollEvents();
        render();
        SDL_Delay(deltaTime); // Simulate ~60 FPS
        updateGame(deltaTime);
        auto eval = evaluator_->evaluate();
        evaluationBar_->set_evaluation(eval);
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

void ChessGui::playSound(Sound& sound){
    SDL_AudioSpec targetSpec = {
                .format = SDL_AUDIO_F32,
                .channels = 2,
                .freq = sound.spec.freq
            };

    SDL_AudioStream* stream = SDL_CreateAudioStream(&sound.spec, &targetSpec);
    SDL_BindAudioStream(audioID, stream);

    SDL_PutAudioStreamData(stream, sound.data.data(), sound.data.size() * sizeof(sound.data[0]));

    SDL_FlushAudioStream(stream);
}

void ChessGui::handleMouseDown(const Uint8 button){
    if (matchManager_->currentPlayer()->playerType != HUMAN) { return; }

    switch (button) {
        case SDL_BUTTON_LEFT: {
            float x, y;
            SDL_GetMouseState(&x, &y);
            addMouseClick(x, y);
            bMouseHeld = true;
            break;
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
            bMouseHeld = false;
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

    visualBoard->clearHighlights();
    // where on the screen did we click?
    const auto rankAndFile = getRankAndFile(x, y);
    if (rankAndFile.file < 1 || rankAndFile.file > 8 || rankAndFile.rank < 1 || rankAndFile.rank > 8) { return; }
    const int candidateClickedSquare = rankAndFileToSquare(rankAndFile.rank, rankAndFile.file);

    // check if we clicked on a piece
    const auto clickedPiece = matchManager_->getBoardManager().getBitboards()->getPiece(
        rankAndFile.rank, rankAndFile.file);

    if (!clickedPiece.has_value()) { return; } // no piece clicked
    // wrong colour clicked
    if (pieceColours[clickedPiece.value()] != matchManager_->getBoardManager().getCurrentTurn()) { return; }

    clickedSquare = candidateClickedSquare;
    visualBoard->highlightSquare(rankAndFile, HighlightType::RECENT_MOVE);
    visualBoard->pickUpPiece(rankAndFile, clickedPiece.value());
}

void ChessGui::addMouseRelease(const int x, const int y){
    if (matchManager_->currentPlayer()->playerType != HUMAN) { return; }

    if (bMouseHeld) { visualBoard->dropPiece(); }

    // where did we click?
    const auto rankAndFile = getRankAndFile(x, y);

    if (clickedSquare == -1) { return; } // no piece selected

    const auto candidateMovePiece = matchManager_->getBoardManager().getBitboards()->getPiece(clickedSquare);
    // do we need this? In theory there's deffo a piece there
    if (!candidateMovePiece.has_value()) {
        visualBoard->clearHighlights();
        clickedSquare = -1;
        return;
    }

    auto candidateMove = Move(candidateMovePiece.value(), clickedSquare,
                              rankAndFileToSquare(rankAndFile.rank, rankAndFile.file));

    // move isn't valid
    if (!matchManager_->getBoardManager().checkMove(candidateMove)) {
        visualBoard->clearHighlights();
        visualBoard->highlightSquare(rankAndFile, HighlightType::INVALID_MOVE);
        clickedSquare = -1;
        return;
    }

    const auto humanPlayer = static_cast<HumanPlayer *>(matchManager_->currentPlayer());
    humanPlayer->addMessage("bestmove " + candidateMove.toUCI());
    visualBoard->highlightSquare(rankAndFile, HighlightType::RECENT_MOVE);
    visualBoard->dropPiece();
}

void ChessGui::recieveInfoOfEngineMove(Move move){
    visualBoard->clearHighlights();
    visualBoard->highlightSquare({move.rankFrom, move.fileFrom}, HighlightType::RECENT_MOVE);
    visualBoard->highlightSquare({move.rankTo, move.fileTo}, HighlightType::RECENT_MOVE);

    if (move.resultBits & MoveResult::CAPTURE) { playSound(captureSound); } else if (
        move.resultBits & MoveResult::PUSH) { playSound(movePiecesSound); }
}