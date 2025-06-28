#ifndef COMPONENTS_CORE_H_
#define COMPONENTS_CORE_H_

#include <SDL3/SDL_render.h>
#include <flecs.h>

namespace ecs {

// Components
struct Position {
    float x, y = 0.0f;  // Axis position
};

struct Velocity {
    float x, y = 0.0f;  // Direction (direction) Must be normalized
    float s{1.0f};      // Speed (scalar)
};

struct BoundingBox {
    SDL_FRect box{};
};

struct Sprite {
    SDL_Texture* texture{nullptr};
};

// Relationships

struct HasSprite {};

struct components_core_2d {
    components_core_2d(flecs::world& world) {
        world.component<Position>();
        world.component<Velocity>();
        world.component<BoundingBox>();
        world.component<Sprite>();
    };
};

}  // namespace ecs

#endif  // COMPONENTS_CORE_H_