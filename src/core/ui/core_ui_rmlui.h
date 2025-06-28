#ifndef CORE_UI_RMLUI_H_
#define CORE_UI_RMLUI_H_

namespace core::ui {

bool RmlUi_Init(void* appstate, int initialWidth, int initialHeight);

void RmlUi_ProcessEvent(void* appstate, SDL_Event* event);

}  // namespace core::ui
#endif