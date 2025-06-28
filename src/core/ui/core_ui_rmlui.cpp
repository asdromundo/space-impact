#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Core.h>

#ifndef NDEBUG
#include <RmlUi/Debugger.h>
#endif

#include "core/AppContext.h"
#include "core/ui/RmlUi_Platform_SDL.h"
#include "core/ui/RmlUi_Renderer_SDL.h"

namespace core::ui {
bool RmlUi_Init(void* appstate, int initialWidth, int initialHeight) {
    auto app = (AppContext*)appstate;
    app->render_interface = new RenderInterface_SDL(app->renderer);
    app->system_interface = new SystemInterface_SDL();
    app->system_interface->SetWindow(app->window);

    // Begin by installing the custom interfaces.
    Rml::SetRenderInterface(app->render_interface);
    Rml::SetSystemInterface(app->system_interface);

    SDL_Log("Using renderer: %s", SDL_GetRendererName(app->renderer));
    // Now we can initialize RmlUi.
    Rml::Initialise();
    // Create a context next.
    Rml::Context* context =
        Rml::CreateContext("main", Rml::Vector2i(initialWidth, initialHeight), app->render_interface);
    if (!context) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't create RmlUi context");
        Rml::Shutdown();
        return false;
    }
// If you want to use the debugger, initialize it now.
#ifndef NDEBUG
    Rml::Debugger::Initialise(context);
#endif
    app->context = context;

    return true;
}

void RmlUi_ProcessEvent(void* appstate, SDL_Event* event) {
    AppContext* app = static_cast<AppContext*>(appstate);
    switch (event->type) {
        case SDL_EVENT_WINDOW_RESTORED:
        case SDL_EVENT_WINDOW_RESIZED:
            int w, h;
            SDL_GetCurrentRenderOutputSize(app->renderer, &w, &h);
            app->context->SetDimensions(Rml::Vector2i(w, h));
            break;
        case SDL_EVENT_MOUSE_MOTION:
            app->context->ProcessMouseMove(event->motion.x, event->motion.y, 0);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            int button = event->button.button;

            // SDL: 1=izq, 2=medio, 3=der. RmlUi usa 0=izq, 1=medio, 2=der
            int rml_button = button - 1;

            app->context->ProcessMouseButtonDown(rml_button, 0);
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            int button = event->button.button;
            int rml_button = button - 1;

            app->context->ProcessMouseButtonUp(rml_button, 0);
            break;
        }
        case SDL_EVENT_KEY_DOWN: {
            const SDL_Keycode keycode = event->key.key;

            // Convierte la tecla SDL a RmlUi
            Rml::Input::KeyIdentifier rml_key = RmlSDL::ConvertKey(keycode);
            app->context->ProcessKeyDown(rml_key, 0);

            // Simula un Enter si hace falta para que dispare eventos "click"
            if (keycode == SDLK_RETURN || keycode == SDLK_KP_ENTER) {
                app->context->ProcessTextInput("\n");
            }

            switch (event->key.scancode) {
#ifndef NDEBUG
                case SDL_SCANCODE_F8:
                    SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "Changing visibility of Debugger");
                    Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());
                    break;
#endif
                default:
                    break;
            }
        }
        default:
            break;
    }
};
}  // namespace core::ui