/**
 * @file GameComponentParsers.hpp
 * @brief Game-specific component parsers for GMapLoader
 *
 * @note IMPORTANT: You must include game_components.hpp BEFORE including this file!
 *       Example:
 *       #include <game_components.hpp>
 *       #include <GameComponentParsers.hpp>
 */

#ifndef GAME_COMPONENT_PARSERS_HPP
#define GAME_COMPONENT_PARSERS_HPP

#include <ECS.hpp>

#include <Graphitech/Engine/Loaders/GMapLoader.hpp>

namespace ECS {

/**
 * @brief Registers all game-specific component parsers.
 *
 * @param loader The GMapLoader to register parsers with
 *
 * @note Requires game_components.hpp to be included before this function is called.
 */
inline void registerGameComponentParsers(GMapLoader& loader) {

    // MapTag
    loader.registerParser("MapTag", [](Scene& s, Entity e, const json& data) {
        MapTag tag;
        if (data.contains("name")) {
            tag.name = data["name"].get<std::string>();
        }
        s.addComponent(e, tag);
    });

    // TicPiece
    loader.registerParser("TicPiece", [](Scene& s, Entity e, const json& data) {
        TicPiece piece;
        if (data.contains("player")) {
            std::string p = data["player"].get<std::string>();
            piece.player = p.empty() ? ' ' : p[0];
        }
        if (data.contains("cell")) {
            piece.cell = data["cell"].get<int>();
        }
        s.addComponent(e, piece);
    });

    // RenderLine
    loader.registerParser("RenderLine", [](Scene& s, Entity e, const json& data) {
        RenderLine line;
        if (data.contains("x1")) line.x1 = data["x1"].get<float>();
        if (data.contains("y1")) line.y1 = data["y1"].get<float>();
        if (data.contains("x2")) line.x2 = data["x2"].get<float>();
        if (data.contains("y2")) line.y2 = data["y2"].get<float>();
        if (data.contains("width")) line.width = data["width"].get<float>();
        if (data.contains("r")) line.r = data["r"].get<float>();
        if (data.contains("g")) line.g = data["g"].get<float>();
        if (data.contains("b")) line.b = data["b"].get<float>();
        s.addComponent(e, line);
    });

    // RenderCircle
    loader.registerParser("RenderCircle", [](Scene& s, Entity e, const json& data) {
        RenderCircle circle;
        if (data.contains("radius")) circle.radius = data["radius"].get<float>();
        if (data.contains("r")) circle.r = data["r"].get<float>();
        if (data.contains("g")) circle.g = data["g"].get<float>();
        if (data.contains("b")) circle.b = data["b"].get<float>();
        if (data.contains("filled")) circle.filled = data["filled"].get<bool>();
        s.addComponent(e, circle);
    });

    // RenderCross
    loader.registerParser("RenderCross", [](Scene& s, Entity e, const json& data) {
        RenderCross cross;
        if (data.contains("half")) cross.half = data["half"].get<float>();
        if (data.contains("width")) cross.width = data["width"].get<float>();
        if (data.contains("r")) cross.r = data["r"].get<float>();
        if (data.contains("g")) cross.g = data["g"].get<float>();
        if (data.contains("b")) cross.b = data["b"].get<float>();
        s.addComponent(e, cross);
    });

    // Health
    loader.registerParser("Health", [](Scene& s, Entity e, const json& data) {
        Health health;
        if (data.contains("hp")) health.hp = data["hp"].get<int>();
        if (data.contains("max")) health.max = data["max"].get<int>();
        s.addComponent(e, health);
    });

    // Damage
    loader.registerParser("Damage", [](Scene& s, Entity e, const json& data) {
        Damage damage;
        if (data.contains("value")) damage.value = data["value"].get<int>();
        s.addComponent(e, damage);
    });

    // Tags
    loader.registerParser("PlayerTag", [](Scene& s, Entity e, const json&) {
        s.addComponent(e, PlayerTag{});
    });

    loader.registerParser("EnemyTag", [](Scene& s, Entity e, const json&) {
        s.addComponent(e, EnemyTag{});
    });

    loader.registerParser("ProjectileTag", [](Scene& s, Entity e, const json&) {
        s.addComponent(e, ProjectileTag{});
    });

    loader.registerParser("Click", [](Scene& s, Entity e, const json&) {
        s.addComponent(e, Click{});
    });
}

} // namespace ECS

#endif // GAME_COMPONENT_PARSERS_HPP

