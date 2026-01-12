/*
** EPITECH PROJECT, 2025
** GraphitechEngine
** File description:
** UIFactory.hpp
*/

#ifndef UIFACTORY_HPP
#define UIFACTORY_HPP

#include <Graphitech/ECS/Factories/EntityFactory.hpp>
#include <string>

namespace Graphitech {
namespace UI {
    class UIButton;
    class UIPanel;
    class UITextInput;
    class UISlider;
    class UICheckbox;
    class UIContext;
    class UIContextMenu;
    class UISplitter;
}
}

namespace ECS {
    class Scene;
    
    /**
     * @brief Factory for creating UI entities
     */
    class UIFactory {
    public:
        /**
         * @brief Create a button entity in the scene
         */
        static Entity createButton(Scene& scene, float x, float y, float width, float height, const std::string& text = "");
        
        /**
         * @brief Create a panel entity in the scene
         */
        static Entity createPanel(Scene& scene, float x, float y, float width, float height);
        
        /**
         * @brief Create a text input entity in the scene
         */
        static Entity createTextInput(Scene& scene, float x, float y, float width, float height);
        
        /**
         * @brief Create a slider entity in the scene
         */
        static Entity createSlider(Scene& scene, float x, float y, float width, float height);
        
        /**
         * @brief Create a checkbox entity in the scene
         */
        static Entity createCheckbox(Scene& scene, float x, float y, float width, float height, bool checked = false);
        
        /**
         * @brief Create a context menu entity in the scene
         */
        static Entity createContextMenu(Scene& scene, float x, float y, float width, float height);
        
        /**
         * @brief Create a splitter entity in the scene
         */
        static Entity createSplitter(Scene& scene, float x, float y, float width, float height);
    };
}

#endif // UIFACTORY_HPP
