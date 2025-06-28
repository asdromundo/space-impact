#ifndef GAME_COMPONENTS_H
#define GAME_COMPONENTS_H

#include <SDL3/SDL_stdinc.h>

struct Size2D {
    float width = 0.0f;
    float height = 0.0f;
};

struct MovementInput2D {
    Sint16 x, y;
};

#endif  // GAME_COMPONENTS_H
