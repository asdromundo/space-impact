#ifndef SYSTEMS_SPACE_IMPACT_H
#define SYSTEMS_SPACE_IMPACT_H

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <flecs.h>

#include "ecs/components/components_space_impact.h"
#include "systems_core.h"

namespace ecs {

struct systems_space_impact {
    inline static void setFixedTimeStep(float timeStep) { _fixedTimeStep = timeStep; };

    systems_space_impact(flecs::world& world) {
        world.module<systems_space_impact>();
        components_space_impact{world};
        ecs::systems_core_2d::setFixedTimeStep(_fixedTimeStep);
        world.import <systems_core_2d>();

        // Module systems
        world.system<Velocity>("EnemyTrayectory")
            .with<Enemy>()
            .interval(_fixedTimeStep)
            .each([](flecs::iter& it, size_t, Velocity& v) {
                float time = SDL_GetTicks() / 1000.0f;
                v.y = SDL_cosf(time) / 2;
            });
    };

   private:
    inline static FixedTimeStep _fixedTimeStep;
};

}  // namespace ecs

#endif  // COMPONENTS_CORE_H