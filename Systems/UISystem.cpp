/*
** EPITECH PROJECT, 2025
** GraphitechEngine
** File description:
** UISystem.cpp
*/

#include "UISystem.hpp"
#include <Graphitech/ECS/Components/UIComponent.hpp>

namespace ECS {

void UISystem::registerUISystem(Scene& scene)
{
    scene.addSystem([](Scene& s, float dt) {
        UISystem::updateUI(s, dt);
    });
}

void UISystem::renderUI(Scene& scene)
{
    auto uiEntities = scene.getEntitiesWith<UIComponent>();
    
    for (Entity e : uiEntities) {
        auto* uiComp = scene.getComponent<UIComponent>(e);
        if (uiComp && uiComp->active && uiComp->widget) {
            uiComp->widget->Render();
        }
    }
}

void UISystem::updateUI(Scene& scene, float deltaTime)
{
    auto uiEntities = scene.getEntitiesWith<UIComponent>();
    
    for (Entity e : uiEntities) {
        auto* uiComp = scene.getComponent<UIComponent>(e);
        if (uiComp && uiComp->active && uiComp->widget) {
            uiComp->widget->Update(deltaTime);
        }
    }
}

}
