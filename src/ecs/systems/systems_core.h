#ifndef SYSTEMS_CORE_H_
#define SYSTEMS_CORE_H_

#include <flecs.h>

#include "ecs/components/components_core.h"

namespace ecs {
typedef float FixedTimeStep;

struct systems_core_2d {
    inline static void setFixedTimeStep(float timeStep) { _fixedTimeStep = timeStep; };

    systems_core_2d(flecs::world& world) {
        world.module<systems_core_2d>();
        components_core_2d{world};
        auto context = world.get_ctx();

        // Fixed time step
        world.system<BoundingBox, const Velocity>("Move")
            .interval(_fixedTimeStep)
            .each([](flecs::iter& it, size_t, BoundingBox& p, const Velocity& v) {
                p.box.x += v.x * it.delta_system_time() * v.s;
                p.box.y += v.y * it.delta_system_time() * v.s;
            });

        // Find collisions between the player and the enemies
        flecs::query<const BoundingBox> enemies = world.query_builder<const BoundingBox>().with<Enemy>().build();
        world.system<const BoundingBox>("PlayerBBCollisions")
            .with<Player>()
            .interval(0.5)
            .each([enemies](const BoundingBox& player) {
                enemies.each([player](const BoundingBox& enemy) {
                    if (SDL_HasRectIntersectionFloat(&player.box, &enemy.box)) {
                        SDL_Log("Collision");
                    }
                });
            });
    };

   private:
    inline static FixedTimeStep _fixedTimeStep;
};

}  // namespace ecs

#endif  // SYSTEMS_CORE_H_