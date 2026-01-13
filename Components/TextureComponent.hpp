/*
** EPITECH PROJECT, 2025
** GraphitechEngine
** File description:
** TextureComponent.hpp
*/
#ifndef TEXTURECOMPONENT_HPP
#define TEXTURECOMPONENT_HPP
#include <Graphitech/ECS/Core/AComponent.hpp>
#include <string>

namespace ECS {
    /**
     * @brief Texture component for rendering with textures.
     *
     * @details Stores the path to a texture file that can be applied to any rendered entity.
     * Works with circles, rectangles, and other shapes.
     *
     * @ingroup ECS
     */
    class TextureComponent : public AComponent {
        public:
            std::string texturePath; /**< Path to the texture file (relative or absolute) */
            bool enabled = true;     /**< Whether the texture should be rendered */

            /**
             * @brief Default constructor.
             *
             * @details Initializes with an empty texture path.
             */
            TextureComponent() = default;

            /**
             * @brief Constructs a texture component with a specified path.
             *
             * @param path The path to the texture file.
             */
            TextureComponent(const std::string& path) : texturePath(path), enabled(true)
            {
            }
    };
}// namespace ECS

#endif//TEXTURECOMPONENT_HPP
