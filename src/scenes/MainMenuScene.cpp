#include "MainMenuScene.h"

#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/ElementDocument.h>
#include <SDL3_image/SDL_image.h>

#include <cmath>
#include <filesystem>

#include "core/scene/Events.h"

class RmlUiEventListener : public Rml::EventListener {
   public:
    explicit RmlUiEventListener(MainMenuScene* scene) : owner(scene) {}
    void ProcessEvent(Rml::Event& event) override {
        Rml::Element* target = event.GetCurrentElement();
        std::string id = target->GetId().c_str();

        if (event.GetType() == "focus") {
            // Reproduce el sonido al enfocar un botón
            Mix_PlayChannel(-1, owner->moveSound, 0);
            return;
        }

        if (event.GetType() == "click") {
            Mix_PlayChannel(-1, owner->enterSound, 0);
            if (id == "solo") {
                game::menu::EmitStartGameEvent(game::mode::SOLO);
                SDL_LogDebug(SDL_LOG_CATEGORY_INPUT, "Solo button");
            }
        }
    }

   private:
    MainMenuScene* owner;
};

MainMenuScene::MainMenuScene(AppContext* context) : Scene("MainMenu", context) {}

MainMenuScene::~MainMenuScene() {
    CleanUp();
}

bool MainMenuScene::Init() {
    moveSound = Mix_LoadWAV("resources/sounds/ping.wav");
    enterSound = Mix_LoadWAV("resources/sounds/pong.wav");

    return moveSound && enterSound;
}

void MainMenuScene::Ready() {
    // Fonts should be loaded before any documents are loaded.
    if (Rml::LoadFontFace("resources/fonts/monogram.ttf")) {
        SDL_LogDebug(SDL_LOG_PRIORITY_DEBUG, "Loaded font");
    }
}

void MainMenuScene::OnEnter() {
    if (music) {
        Mix_PlayMusic(music, 0);
    }

    doc = app->context->LoadDocument("resources/ui/main_menu_screen.rml");
    if (!doc) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't read RmlUi document");
    } else {
        doc->Show();
        // Conectar eventos a botones
        Rml::Element* btn_solo = doc->GetElementById("solo");

        RmlUiEventListener* listener = new RmlUiEventListener(this);
        if (btn_solo) {
            btn_solo->Focus();
            btn_solo->SetPseudoClass("focus-visible", true);
            btn_solo->AddEventListener("click", listener);
            btn_solo->AddEventListener("focus", listener);
        }
        // if (btn_single) {
        //     btn_single->AddEventListener("click", listener);
        //     btn_single->AddEventListener("focus", listener);
        // }
    }
}

void MainMenuScene::OnExit() {
    Mix_HaltMusic();
    if (doc) {
        doc->Close();
    }
}

void MainMenuScene::CleanUp() {
    if (music) {
        Mix_FreeMusic(music);
        music = nullptr;
    }
    if (moveSound) {
        Mix_FreeChunk(moveSound);
        moveSound = nullptr;
    }
    if (enterSound) {
        Mix_FreeChunk(enterSound);
        enterSound = nullptr;
    }
}

SDL_AppResult MainMenuScene::HandleEvent(SDL_Event* event) {
    switch (event->type) {
        case SDL_EVENT_KEY_DOWN:
            switch (event->key.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    core::scene::events::EmitSceneFinishedEvent();  // end the scene
                    break;
                default:
                    break;
            }
        case SDL_EVENT_WINDOW_RESIZED:
        case SDL_EVENT_WINDOW_RESTORED:
            break;
        default:
            break;
    }
    return SDL_APP_CONTINUE;
}

void MainMenuScene::Update(float deltaTime) {
    // Add animation or logic if needed
}

void MainMenuScene::Render() {
    SDL_SetRenderDrawColor(app->renderer, 0x8A, 0x9A, 0x5A, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(app->renderer);

    if (app->context) {
        app->context->Update();
        // app->render_interface->BeginFrame();
        app->context->Render();
        // app->render_interface->EndFrame();
    }

    SDL_RenderPresent(app->renderer);
}

// Utility loaders

bool MainMenuScene::LoadImageTexture(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load image: %s", SDL_GetError());
        return false;
    }

    imageTex = SDL_CreateTextureFromSurface(app->renderer, surface);
    SDL_DestroySurface(surface);

    if (!imageTex) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create image texture: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool MainMenuScene::LoadMusic(const std::string& path) {
    music = Mix_LoadMUS(path.c_str());
    if (!music) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load music: %s", SDL_GetError());
        return false;
    }
    // Mix_PlayMusic(music, 0);
    return true;
}
