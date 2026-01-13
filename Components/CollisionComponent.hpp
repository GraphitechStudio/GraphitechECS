#ifndef COLLISION_COMPONENT_HPP
#define COLLISION_COMPONENT_HPP

#include <Graphitech/ECS/Core/AComponent.hpp>

namespace ECS {
    /**
     * @brief Simple AABB collision component.
     *
     * @details Stores half-extents around entity position to build an axis-aligned box.
     * `solid` can be used by systems to decide if overlap should block/resolve.
     * Consumed by collision/physics systems.
     *
     * @ingroup ECS
     */
    class CollisionComponent : public ECS::AComponent {
    public:
        float hx{0.f};  /**< Half-width in world units */
        float hy{0.f};  /**< Half-height in world units */
        bool solid{true}; /**< True if overlaps should block/resolve */

        CollisionComponent() = default;
        CollisionComponent(float halfWidth, float halfHeight, bool isSolid = true)
            : hx(halfWidth), hy(halfHeight), solid(isSolid) {}
    };
}

#endif // COLLISION_COMPONENT_HPP
