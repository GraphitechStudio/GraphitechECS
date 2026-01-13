/*
** EPITECH PROJECT, 2025
** GraphitechEngine
** File description:
** ScaleComponent.hpp
*/
#ifndef SCALECOMPONENT_HPP
#define SCALECOMPONENT_HPP
#include <Graphitech/ECS/Core/AComponent.hpp>

namespace ECS {
    /**
     * @brief Scale component for size transformation.
     *
     * @details Defines the scaling factors for an entity's width and height. Values of 1.0
     * represent normal size, while values greater than 1.0 enlarge and less than 1.0 shrink.
     *
     * @ingroup ECS
     */
    class ScaleComponent : public AComponent {
        public:
            float x = 1.f; /**< Horizontal scaling factor */
            float y = 1.f; /**< Vertical scaling factor */

            /**
             * @brief Default constructor.
             *
             * @details Initializes scale to (1, 1) representing normal size.
             */
            ScaleComponent() = default;

            /**
             * @brief Constructs a scale with specified factors.
             *
             * @param x_ The horizontal scaling factor.
             * @param y_ The vertical scaling factor.
             */
            ScaleComponent(float x_, float y_) : x(x_), y(y_)
            {
            }
    };
}// namespace ECS

#endif//SCALECOMPONENT_HPP
