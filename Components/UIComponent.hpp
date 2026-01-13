/*
** EPITECH PROJECT, 2025
** GraphitechEngine
** File description:
** UIComponent.hpp
*/

#ifndef UICOMPONENT_HPP
#define UICOMPONENT_HPP

#include <Graphitech/ECS/Core/AComponent.hpp>
#include <memory>

namespace Graphitech {
namespace UI {
    class UIWidget;
}
}

namespace ECS {
    /**
     * @brief Component for attaching UI widgets to entities
     */
    struct UIComponent : public AComponent {
        std::shared_ptr<Graphitech::UI::UIWidget> widget;
        bool active = true;
        
        UIComponent() = default;
        UIComponent(std::shared_ptr<Graphitech::UI::UIWidget> w) 
            : widget(w), active(true) {}
    };
}

#endif // UICOMPONENT_HPP
