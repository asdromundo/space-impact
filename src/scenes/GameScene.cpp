#include "GameScene.h"

#include <RmlUi/Core.h>
#include <RmlUi/Core/Context.h>
#include <SDL3_image/SDL_image.h>

#include <format>

#include "core/scene/Events.h"

flecs::system render_sys, input_sys;

GameScene::GameScene(AppContext* context, game::mode::Mode mode) : Scene("Game", context), gameMode(mode) {}

GameScene::~GameScene() {
    CleanUp();
}

// AABB Collision
inline bool isAABBColliding(const SDL_FRect& a, const SDL_FRect& b) {
    return (a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y);
}

bool GameScene::Init() {
    // flecs
    world = new flecs::world();  // Manually progress on Update()
    world->set_threads(4);
    world->set_ctx((void*)app);  // Send it the AppContext
    flecs::timer secondTimer = world->timer().interval(1.0);
    float fixedTimeStep{1.0f / 60.0f};
    // Register systems
    // Executed only in Render()
    render_sys = world->system<const BoundingBox, const Sprite>("Render").kind(0).each(
        [](flecs::iter& it, size_t, const BoundingBox& p, const Sprite& s) {
            AppContext* app = static_cast<AppContext*>(it.world().get_ctx());
            SDL_RenderTexture(app->renderer, s.sprite, nullptr, &p.box);
        });
    // Executed only by HandleEvent()
    input_sys = world->system<const MovementInput2D, Velocity>("NormalizeInputVelocity")
                    .kind(0)
                    .each([](flecs::iter& it, size_t, const MovementInput2D& i, Velocity& v) {
                        float fx = static_cast<float>(i.x);
                        float fy = static_cast<float>(i.y);
                        float len = SDL_sqrt(fx * fx + fy * fy);
                        if (len > 1e-6f) {
                            v.x = fx / len;
                            v.y = fy / len;
                        }
                    });

    // FixedStepExecution
    world->system<BoundingBox, const Velocity>("Move")
        .interval(fixedTimeStep)
        .each([](flecs::iter& it, size_t, BoundingBox& p, const Velocity& v) {
            p.box.x += v.x * it.delta_system_time() * v.s;
            p.box.y += v.y * it.delta_system_time() * v.s;
            p.box.x = SDL_clamp(p.box.x, 0, 1280.0f - p.box.w);
            p.box.y = SDL_clamp(p.box.y, 0, 720.0f - p.box.h);
        });

    world->system<Velocity>("EnemyTrayectory")
        .with<Enemy>()
        .interval(fixedTimeStep)
        .each([](flecs::iter& it, size_t, Velocity& v) {
            float time = SDL_GetTicks() / 1000.0f;
            v.y = SDL_cosf(time) / 2;
        });
    // OnFlecs Lifecycle

    // Query para encontrar colisiones entre jugadores y enemigos
    flecs::query<const BoundingBox> enemies = world->query_builder<const BoundingBox>().with<Enemy>().build();
    world->system<const BoundingBox>("PlayerCollisions")
        .with<Player>()
        .interval(0.5)
        .each([enemies](const BoundingBox& player) {
            enemies.each([player](const BoundingBox& enemy) {
                if (isAABBColliding(player.box, enemy.box)) {
                    SDL_Log("Collision");
                }
            });
        });

    return true;
}

void GameScene::CleanUp() {
    if (world) {
        world->release();
    }
}

static Size2D GetCurrentRenderSize(const AppContext* app) {
    int w, h;
    SDL_GetCurrentRenderOutputSize(app->renderer, &w, &h);
    return Size2D{static_cast<float>(w), static_cast<float>(h)};
}

void GameScene::Ready() {
    lastKnownRenderSize = GetCurrentRenderSize(app);
    // Fonts should be loaded before any documents are loaded.
    if (Rml::LoadFontFace("resources/fonts/monogram.ttf")) {
        SDL_LogDebug(SDL_LOG_PRIORITY_DEBUG, "Loaded font");
    }
    SDL_Texture* ship_sprite = IMG_LoadTexture(app->renderer, "resources/sprites/Player_Blue_03.png");
    SDL_Texture* enemy_sprite = IMG_LoadTexture(app->renderer, "resources/sprites/Enemy_Red_03.png");
    float speed = SDL_min(lastKnownRenderSize.width, lastKnownRenderSize.height) / 3;
    //  Register Entities
    player = world->entity("Player 1")
                 .add<Player>()
                 .set<MovementInput2D>({0, 0})
                 .set<BoundingBox>({{500.0f, 500.0f, 60.0f, 60.0f}})
                 .set<Velocity>({0.0f, 0.0f, 256.0f})
                 .set<Sprite>({ship_sprite});
    world->entity("Enemy1")
        .add<Enemy>()
        .set<BoundingBox>({{1000.0f, 150.0f, 50.0f, 50.0f}})
        .set<Velocity>({-1.0f, 0.0f, 128.0f})
        .set<Sprite>({enemy_sprite});

    world->entity("Enemy2")
        .add<Enemy>()
        .set<BoundingBox>({{1000.0f, 600.0f, 50.0f, 50.0f}})
        .set<Velocity>({-1.0f, 0.0f, 128.0f})
        .set<Sprite>({enemy_sprite});
    movement = player.get_mut<MovementInput2D>();
}

void GameScene::OnEnter() {
    doc = app->context->LoadDocument("resources/ui/game_screen.rml");
    if (!doc) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't read RmlUi document");
    }

    doc->Show();
}

void GameScene::OnExit() {
    if (doc) {
        doc->Close();  // Esto también lo remueve del Context
        doc = nullptr;
    }
}

SDL_AppResult GameScene::HandleEvent(SDL_Event* event) {
    switch (event->type) {
        case SDL_EVENT_KEY_DOWN:
            switch (event->key.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    core::scene::events::EmitSceneFinishedEvent();  // end the scene
                    break;
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP: {
                    movement.y = SDL_MIN_SINT16;
                    player.set<MovementInput2D>(movement);
                    input_sys.run();
                    break;
                }
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN: {
                    movement.y = SDL_MAX_SINT16;
                    player.set<MovementInput2D>(movement);
                    input_sys.run();
                    break;
                }
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT: {
                    movement.x = SDL_MAX_SINT16;
                    player.set<MovementInput2D>(movement);
                    input_sys.run();
                    break;
                }
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT: {
                    movement.x = SDL_MIN_SINT16;
                    player.set<MovementInput2D>(movement);
                    input_sys.run();
                    break;
                }
                default:
                    break;
            }
            break;
        case SDL_EVENT_KEY_UP:
            switch (event->key.scancode) {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_UP:
                case SDL_SCANCODE_DOWN: {
                    movement.y = 0;
                    player.set<MovementInput2D>(movement);
                    input_sys.run();
                    break;
                }
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT: {
                    movement.x = 0;
                    player.set<MovementInput2D>(movement);
                    input_sys.run();
                    break;
                }
                default:
                    break;
            }
            break;
        case SDL_EVENT_WINDOW_RESTORED:
        case SDL_EVENT_WINDOW_RESIZED:
            adjustToScreen();
            break;
        default:
            break;
    }
    return SDL_APP_CONTINUE;
}

void GameScene::Update(float deltatime) {
    if (timeAfterGameEnded >= 0.0) {  // If our counter has started
        timeAfterGameEnded += deltatime;
        if (timeAfterGameEnded >= 1.5) {                    // Wait 1.5 seconds
            core::scene::events::EmitSceneFinishedEvent();  // end the scene
        }
    }
    world->progress(deltatime);
}

void GameScene::Render() {
    SDL_SetRenderDrawColor(app->renderer, 0x8A, 0x9A, 0x5A, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(app->renderer);

    render_sys.run();

    if (app->context) {
        app->context->Update();
        // app->render_interface->BeginFrame();
        app->context->Render();
        // app->render_interface->EndFrame();
    }
    SDL_RenderPresent(app->renderer);
}

void GameScene::adjustToScreen() {
    Size2D newRenderSize = GetCurrentRenderSize(app);
    float xDiff = newRenderSize.width / lastKnownRenderSize.width;
    float yDiff = newRenderSize.height / lastKnownRenderSize.height;
    lastKnownRenderSize = newRenderSize;
}

void GameScene::CheckGameOver() {
    Rml::Element* score_label = doc->GetElementById("score");
    if (!score_label)
        return;

    std::string scoreText;

    score_label->SetInnerRML(scoreText);
    timeAfterGameEnded = 0.0f;
}