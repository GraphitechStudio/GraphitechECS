/**
 * @file ComponentRegistration.hpp
 * @brief Helper macros for easy component registration
 *
 * Provides convenient macros to register components with minimal boilerplate.
 */

#ifndef COMPONENT_REGISTRATION_HPP
#define COMPONENT_REGISTRATION_HPP

#include "ComponentRegistry.hpp"
#include <Graphitech/ECS/Components/TextureComponent.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * @brief Macro to register a component parser at static initialization time.
 *
 * Usage in your component header:
 * @code
 * class MyComponent : public ECS::AComponent {
 * public:
 *     int value = 0;
 * };
 *
 * // At the end of the header file (or in a .cpp)
 * REGISTER_COMPONENT_PARSER(MyComponent, {
 *     MyComponent comp;
 *     if (data.contains("value")) {
 *         comp.value = data["value"].get<int>();
 *     }
 *     s.addComponent(e, comp);
 * })
 * @endcode
 */
#define REGISTER_COMPONENT_PARSER(ComponentType, parser_body) \
    namespace { \
        struct _ComponentParser_##ComponentType { \
            _ComponentParser_##ComponentType() { \
                ECS::ComponentRegistry::get().registerParser(#ComponentType, \
                    [](ECS::Scene& s, ECS::Entity e, const json& data) { \
                        parser_body \
                    }); \
            } \
        }; \
        static _ComponentParser_##ComponentType _parser_instance_##ComponentType; \
    }

/**
 * @brief Simplified macro for simple field-to-field mapping.
 *
 * Usage:
 * @code
 * class Position : public ECS::AComponent {
 * public:
 *     float x = 0, y = 0;
 * };
 *
 * REGISTER_COMPONENT_SIMPLE(Position, {
 *     {"x", &Position::x},
 *     {"y", &Position::y}
 * })
 * @endcode
 */
#define REGISTER_COMPONENT_SIMPLE(ComponentType, field_map) \
    namespace { \
        struct _ComponentParser_##ComponentType { \
            _ComponentParser_##ComponentType() { \
                ECS::ComponentRegistry::get().registerParser(#ComponentType, \
                    [](ECS::Scene& s, ECS::Entity e, const json& data) { \
                        ComponentType comp; \
                        for (const auto& [key, value] : data.items()) { \
                            if constexpr (requires { value.get<typename decltype(field_map.begin()->second)::element_type>(); }) { \
                                /* Attempt to deserialize */ \
                            } \
                        } \
                        s.addComponent(e, comp); \
                    }); \
            } \
        }; \
        static _ComponentParser_##ComponentType _parser_instance_##ComponentType; \
    }

/**
 * @brief Helper: Auto-register all default ECS components
 *
 * Call this once in your main scene loader to register all built-in components.
 */
/**
 * @brief Register the built-in engine components with the component registry.
 *
 * @details Installs parsers for Position/Velocity/Scale/Rotation so GMapLoader can
 * populate scenes. Call once during loader bootstrap before loading maps.
 */
inline void registerDefaultComponents() {
    // PositionComponent (2D only: x, y)
    ECS::ComponentRegistry::get().registerParser("PositionComponent",
        [](ECS::Scene& s, ECS::Entity e, const json& data) {
            ECS::PositionComponent pos;
            if (data.contains("x")) pos.x = data["x"].get<float>();
            if (data.contains("y")) pos.y = data["y"].get<float>();
            s.addComponent(e, pos);
        });

    // VelocityComponent (2D only: vx, vy)
    ECS::ComponentRegistry::get().registerParser("VelocityComponent",
        [](ECS::Scene& s, ECS::Entity e, const json& data) {
            ECS::VelocityComponent vel;
            if (data.contains("vx")) vel.vx = data["vx"].get<float>();
            if (data.contains("vy")) vel.vy = data["vy"].get<float>();
            s.addComponent(e, vel);
        });

    // ScaleComponent (2D only: x, y)
    ECS::ComponentRegistry::get().registerParser("ScaleComponent",
        [](ECS::Scene& s, ECS::Entity e, const json& data) {
            ECS::ScaleComponent scale;
            if (data.contains("x")) scale.x = data["x"].get<float>();
            if (data.contains("y")) scale.y = data["y"].get<float>();
            s.addComponent(e, scale);
        });

    // RotationComponent (angle only in radians)
    ECS::ComponentRegistry::get().registerParser("RotationComponent",
        [](ECS::Scene& s, ECS::Entity e, const json& data) {
            ECS::RotationComponent rot;
            if (data.contains("angle")) rot.angle = data["angle"].get<float>();
            s.addComponent(e, rot);
        });

    // TextureComponent (image rendering)
    ECS::ComponentRegistry::get().registerParser("TextureComponent",
        [](ECS::Scene& s, ECS::Entity e, const json& data) {
            if (!data.contains("path")) return; // ignore if no path
            std::cout << "ici" << std::endl;
            ECS::TextureComponent tex;
            tex.texturePath = data.value("path", "");
            tex.enabled = data.value("enabled", true);
            s.addComponent(e, tex);
        });
}
#endif // COMPONENT_REGISTRATION_HPP

