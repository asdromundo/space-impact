#ifndef COMPONENTS_SPACE_IMPACT_H_
#define COMPONENTS_SPACE_IMPACT_H_

#include <flecs.h>

namespace ecs {

struct Player {};
struct Enemy {};

struct components_space_impact {
    components_space_impact(flecs::world& world) {
        world.component<Player>();
        world.component<Enemy>();
    };
};
}  // namespace ecs

#endif  // COMPONENTS_SPACE_IMPACT_H_