/*
** EPITECH PROJECT, 2025
** GraphitechEngine
** File description:
** PositionComponent.hpp
*/
#ifndef POSITIONCOMPONENT_HPP
#define POSITIONCOMPONENT_HPP
#include <Graphitech/ECS/Core/AComponent.hpp>

namespace ECS {
    /**
     * @brief Position component for 2D spatial positioning.
     *
     * @details Represents the position of an entity in 2D space using floating-point coordinates.
     * This component is commonly used for rendering, collision detection, and spatial queries.
     *
     * @ingroup ECS
     */
    class PositionComponent : public AComponent {
        public:
            float x = 0.f; /**< X-coordinate in world space */
            float y = 0.f; /**< Y-coordinate in world space */

            /**
             * @brief Default constructor.
             *
             * @details Initializes position to (0, 0).
             */
            PositionComponent() = default;

            /**
             * @brief Constructs a position with specified coordinates.
             *
             * @param x_ The X-coordinate.
             * @param y_ The Y-coordinate.
             */
            PositionComponent(float x_, float y_) : x(x_), y(y_)
            {
            }
    };
}// namespace ECS

#endif//POSITIONCOMPONENT_HPP
