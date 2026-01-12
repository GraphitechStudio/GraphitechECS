/*
** EPITECH PROJECT, 2025
** GraphitechEngine
** File description:
** UIFactory.cpp
*/

#include "UIFactory.hpp"
#include <Graphitech/ECS/Core/Scene.hpp>
#include <Graphitech/ECS/Components/UIComponent.hpp>
#include <Graphitech/Engine/UI/UIButton.hpp>
#include <Graphitech/Engine/UI/UIPanel.hpp>
#include <Graphitech/Engine/UI/UITextInput.hpp>
#include <Graphitech/Engine/UI/UISlider.hpp>
#include <Graphitech/Engine/UI/UICheckbox.hpp>
#include <Graphitech/Engine/UI/UIContextMenu.hpp>
#include <Graphitech/Engine/UI/UISplitter.hpp>
#include <algorithm>

namespace ECS {

Entity UIFactory::createButton(Scene& scene, float x, float y, float width, float height, const std::string& text)
{
    Entity e = scene.createEntity();
    auto button = std::make_shared<Graphitech::UI::UIButton>(x, y, width, height);
    button->SetText(text);
    scene.addComponent(e, UIComponent(button));
    return e;
}

Entity UIFactory::createPanel(Scene& scene, float x, float y, float width, float height)
{
    Entity e = scene.createEntity();
    auto panel = std::make_shared<Graphitech::UI::UIPanel>(x, y, width, height);
    scene.addComponent(e, UIComponent(panel));
    return e;
}

Entity UIFactory::createTextInput(Scene& scene, float x, float y, float width, float height)
{
    Entity e = scene.createEntity();
    auto textInput = std::make_shared<Graphitech::UI::UITextInput>(x, y, width, height);
    scene.addComponent(e, UIComponent(textInput));
    return e;
}

Entity UIFactory::createSlider(Scene& scene, float x, float y, float width, float height)
{
    Entity e = scene.createEntity();
    auto slider = std::make_shared<Graphitech::UI::UISlider>(x, y, width, height);
    scene.addComponent(e, UIComponent(slider));
    return e;
}

Entity UIFactory::createCheckbox(Scene& scene, float x, float y, float width, float height, bool checked)
{
    Entity e = scene.createEntity();
    float size = std::min(width, height);
    auto checkbox = std::make_shared<Graphitech::UI::UICheckbox>(x, y, size);
    checkbox->SetChecked(checked);
    scene.addComponent(e, UIComponent(checkbox));
    return e;
}

Entity UIFactory::createContextMenu(Scene& scene, float x, float y, float width, float height)
{
    Entity e = scene.createEntity();
    auto contextMenu = std::make_shared<Graphitech::UI::UIContextMenu>();
    contextMenu->SetPosition(x, y);
    contextMenu->SetSize(width, height);
    scene.addComponent(e, UIComponent(contextMenu));
    return e;
}

Entity UIFactory::createSplitter(Scene& scene, float x, float y, float width, float height)
{
    Entity e = scene.createEntity();
    // Use Vertical orientation by default
    auto splitter = std::make_shared<Graphitech::UI::UISplitter>(x, y, width, Graphitech::UI::SplitterOrientation::Vertical);
    splitter->SetLength(height);
    scene.addComponent(e, UIComponent(splitter));
    return e;
}

}
