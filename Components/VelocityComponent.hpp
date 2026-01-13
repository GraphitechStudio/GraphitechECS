#ifndef VELOCITY_COMPONENT_HPP
#define VELOCITY_COMPONENT_HPP

#include <Graphitech/ECS/Core/AComponent.hpp>

namespace ECS {
    /**
     * @brief Linear velocity component (units per second).
     *
     * @details Stores horizontal and vertical velocity used by movement systems.
     * Positive values move right/up depending on the coordinate system in use.
     * Consumed by movement/physics systems to integrate position each tick.
     *
     * @ingroup ECS
     */
    class VelocityComponent : public ECS::AComponent {
    public:
        float vx{0.f}; /**< Horizontal speed in world units per second */
        float vy{0.f}; /**< Vertical speed in world units per second */

        VelocityComponent() = default;
        VelocityComponent(float x, float y) : vx(x), vy(y) {}
    };
}

#endif // VELOCITY_COMPONENT_HPP
