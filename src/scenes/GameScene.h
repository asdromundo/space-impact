#ifndef SCENES_GAME_SCENE_H
#define SCENES_GAME_SCENE_H

#include <RmlUi/Core/ElementDocument.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <vector>

#include "core/scene/Scene.h"
#include "game/Components.h"
#include "game/Mode.h"

// FLECS
#include <flecs.h>

class GameScene : public core::scene::Scene {
   public:
    explicit GameScene(AppContext* context, game::mode::Mode gameMode);
    GameScene() = delete;
    ~GameScene() override;

    // Lifecycle
    bool Init() override;
    void Ready() override;
    void OnEnter() override;
    void OnExit() override;
    void CleanUp() override;

    // Main loop
    SDL_AppResult HandleEvent(SDL_Event* event) override;
    void Update(float deltaTime) override;
    void Render() override;

    void onSecondCounterTimer();

   private:
    // Game constants
    const game::mode::Mode gameMode;
    const int initialSpeed = 360;

    // Game variables
    flecs::world* world{nullptr};
    Size2D lastKnownRenderSize{};
    float timeAfterGameEnded{-1.0f};
    flecs::entity player;
    MovementInput2D movement{};

    // RmlUi
    Rml::ElementDocument* doc{nullptr};

    // Helper functions
    void adjustToScreen();
    void CheckGameOver();
};

#endif  // SCENES_GAME_SCENE_H
