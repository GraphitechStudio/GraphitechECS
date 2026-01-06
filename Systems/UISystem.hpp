/*
** EPITECH PROJECT, 2025
** GraphitechEngine
** File description:
** UISystem.hpp
*/

#ifndef UISYSTEM_HPP
#define UISYSTEM_HPP

#include <Graphitech/ECS/Core/Scene.hpp>

namespace ECS {
    /**
     * @brief System for rendering and updating UI components in a scene
     */
    class UISystem {
    public:
        /**
         * @brief Register UI update system to a scene
         */
        static void registerUISystem(Scene& scene);
        
        /**
         * @brief Render all UI widgets in a scene
         */
        static void renderUI(Scene& scene);
        
        /**
         * @brief Update all UI widgets in a scene
         */
        static void updateUI(Scene& scene, float deltaTime);
    };
}

#endif // UISYSTEM_HPP
