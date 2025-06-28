#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <cmath>
#include <filesystem>

#include "core/ui/core_ui_rmlui.h"
#include "scenes/ScreenManager.h"

constexpr uint32_t windowStartWidth = 1280;
constexpr uint32_t windowStartHeight = 720;

Uint64 lastTick = 0;
Uint64 currentTick = 0;
float delta_time = 0;

core::scene::Manager* sceneManager{nullptr};

// Auxiliar function to throw error and end the program.
SDL_AppResult SDL_Fail() {
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

// Initialize tne program
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    // init the library, here we make a window so we only need the Video capabilities.
    if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        return SDL_Fail();
    }

    // create a window
    SDL_Window* window = SDL_CreateWindow("Space Impact", windowStartWidth, windowStartHeight,
                                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (not window) {
        return SDL_Fail();
    }

    // Setup the app icon for the window
    SDL_Surface* icon = IMG_Load("resources/logo.svg");
    if (icon) {
        SDL_SetWindowIcon(window, icon);
        SDL_DestroySurface(icon);
    } else {
        SDL_LogError(SDL_LOG_PRIORITY_WARN, "Failed to load icon: %s", SDL_GetError());
    }

    // SDL_SetHint(SDL_HINT_RENDER_DRIVER, "vulkan");
    // create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (not renderer) {
        return SDL_Fail();
    }

    // init SDL Mixer
    auto audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (not audioDevice) {
        return SDL_Fail();
    }
    if (not Mix_OpenAudio(audioDevice, NULL)) {
        return SDL_Fail();
    }

    // print some information about the window
    SDL_ShowWindow(window);
    {
        int width, height, bbwidth, bbheight;
        SDL_GetWindowSize(window, &width, &height);
        SDL_GetWindowSizeInPixels(window, &bbwidth, &bbheight);
        SDL_Log("Window size: %ix%i", width, height);
        SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
        if (width != bbwidth) {
            SDL_Log("This is a highdpi environment.");
        }
    }

    // set up the application data
    *appstate = new AppContext{
        .window = window,
        .renderer = renderer,
        .audioDevice = audioDevice,
    };

    SDL_SetRenderVSync(renderer, -1);  // enable vysnc

    SDL_Log("Application started successfully!");

    // Submit click events when focusing the window.
    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");

    // RmlUi
    core::ui::RmlUi_Init(*appstate, windowStartWidth, windowStartHeight);

    sceneManager = new core::scene::Manager{};
    InitScreenManager(sceneManager, (AppContext*)*appstate);

    return SDL_APP_CONTINUE;
}

// This function substitutes polling for a callback style
// You receive an event at some point, and then you can handle it
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    AppContext* app = static_cast<AppContext*>(appstate);
    switch (event->type) {
        case SDL_EVENT_QUIT:
            app->app_quit = SDL_APP_SUCCESS;
            break;

        default:
            break;
    }
    if (app->context) {
        core::ui::RmlUi_ProcessEvent(appstate, event);
    }
    if (sceneManager) {
        return HandleScreenEvents(event, sceneManager, app);
    }

    return SDL_APP_CONTINUE;
}

// Main loop iteration, here the Update and Render cycle occurs.
SDL_AppResult SDL_AppIterate(void* appstate) {
    AppContext* app = static_cast<AppContext*>(appstate);

    lastTick = currentTick;
    currentTick = SDL_GetTicks();
    delta_time = (currentTick - lastTick) * .001f;

    if (sceneManager) {
        sceneManager->Update(delta_time);
        sceneManager->Render();
    }

    return app->app_quit;
}

// On exit, you should disable and clean.
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    AppContext* app = static_cast<AppContext*>(appstate);

    if (app) {
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);

        Mix_FadeOutMusic(1000);  // prevent the music from abruptly ending.
        Mix_CloseAudio();
        SDL_CloseAudioDevice(app->audioDevice);
        SDL_Log("Closing app");
        Rml::Shutdown();
        delete app->render_interface;
        delete app->system_interface;

        delete app;
    }
    Mix_Quit();
    SDL_Log("Application quit successfully!");
    SDL_Quit();
}