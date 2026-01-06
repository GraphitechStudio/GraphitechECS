#ifndef TTL_COMPONENT_HPP
#define TTL_COMPONENT_HPP

#include <Graphitech/ECS/Core/AComponent.hpp>

namespace ECS {
    /**
     * @brief Time-to-live component.
     *
     * @details Tracks remaining lifetime in seconds. Systems are expected to decrement
     * `ttl` each tick and destroy the entity once it reaches zero or below. Consumed by
     * lifetime/cleanup systems only.
     *
     * @ingroup ECS
     */
    class TTLComponent : public ECS::AComponent {
    public:
        float ttl{0.f}; /**< Remaining lifetime in seconds; entity removed when <= 0 */

        TTLComponent() = default;
        explicit TTLComponent(float seconds) : ttl(seconds) {}
    };
}

#endif // TTL_COMPONENT_HPP
