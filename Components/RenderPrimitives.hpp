/*
** EPITECH PROJECT, 2025
** GraphitechEngine
** File description:
** RenderPrimitives.hpp - simple rendering helper components (engine-level)
*/
#ifndef RENDERPRIMITIVES_HPP
#define RENDERPRIMITIVES_HPP

#include <Graphitech/ECS/Core/AComponent.hpp>

namespace ECS {
    /**
     * @brief Line primitive for debug rendering.
     * @details Consumed by rendering systems to draw a line in world space.
     */
    class RenderLine : public AComponent {
    public:
        float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f; /**< Endpoints in world units */
        float width = 2.0f; /**< Line width in pixels */
        float r = 0.6f, g = 0.6f, b = 0.6f; /**< RGB color in 0–1 linear space */

        RenderLine() = default;
        RenderLine(float ax1, float ay1, float ax2, float ay2,
                   float aw = 2.0f, float ar = 0.6f, float ag = 0.6f, float ab = 0.6f)
            : x1(ax1), y1(ay1), x2(ax2), y2(ay2), width(aw), r(ar), g(ag), b(ab) {}
    };

    /**
     * @brief Circle primitive for debug rendering.
     * @details Consumed by rendering systems to draw a circle in world space.
     */
    class RenderCircle : public AComponent {
    public:
        float radius = 16.0f; /**< Circle radius in world units */
        float r = 1.0f, g = 1.0f, b = 1.0f; /**< RGB color in 0–1 linear space */
        bool filled = false; /**< True for filled disk, false for outline */

        RenderCircle() = default;
        RenderCircle(float rad, float rr, float gg, float bb, bool f = false)
            : radius(rad), r(rr), g(gg), b(bb), filled(f) {}
    };

    /**
     * @brief Cross primitive for debug rendering.
     * @details Consumed by rendering systems to draw a cross centered on the entity.
     */
    class RenderCross : public AComponent {
    public:
        float half = 16.0f; /**< Half arm length in world units */
        float width = 2.0f; /**< Line width in pixels */
        float r = 1.0f, g = 1.0f, b = 1.0f; /**< RGB color in 0–1 linear space */

        RenderCross() = default;
        RenderCross(float hh, float ww, float rr, float gg, float bb)
            : half(hh), width(ww), r(rr), g(gg), b(bb) {}
    };

} // namespace ECS

#endif // RENDERPRIMITIVES_HPP
