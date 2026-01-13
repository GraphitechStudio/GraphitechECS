/*
** EPITECH PROJECT, 2025
** GraphitechEngine
** File description:
** RotationComponent.hpp
*/
#ifndef ROTATIONCOMPONENT_HPP
#define ROTATIONCOMPONENT_HPP
#include <Graphitech/ECS/Core/AComponent.hpp>

namespace ECS {
    /**
     * @brief Rotation component for angular orientation.
     *
     * @details Stores the rotation angle of an entity in radians. Used for rendering rotated
     * sprites, collision detection with rotated shapes, and directional calculations.
     *
     * @ingroup ECS
     */
    class RotationComponent : public AComponent {
    public:
        float angle = 0.f; /**< Rotation angle in radians */

        /**
         * @brief Default constructor.
         *
         * @details Initializes rotation angle to 0 radians.
         */
        RotationComponent() = default;

        /**
         * @brief Constructs a rotation with specified angle.
         *
         * @param a The rotation angle in radians.
         */
        explicit RotationComponent(float a) : angle(a)
        {
        }
    };
}// namespace ECS

#endif//ROTATIONCOMPONENT_HPP
