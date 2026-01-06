/**
 * @file ComponentRegistry.hpp
 * @brief Global registry for component parsers
 *
 * Allows games to register custom component parsers that will be automatically
 * used by GMapLoader without modifying the loader code.
 *
 * @ingroup ECS
 */

#ifndef COMPONENT_REGISTRY_HPP
#define COMPONENT_REGISTRY_HPP

#include <Loaders/GMapLoader.hpp>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <memory>

namespace ECS {

/**
 * @brief Global registry for component parsers
 *
 * This is a singleton that collects all component parsers from various sources
 * (engines, games, plugins) and makes them available to GMapLoader.
 *
 * @details
 * Usage in your component header file:
 * @code
 * struct MyComponent : public AComponent {
 *     int value;
 *
 *     // Static registration
 *     static void registerParser() {
 *         ComponentRegistry::get().registerParser("MyComponent",
 *             [](Scene& s, Entity e, const json& data) {
 *                 MyComponent comp;
 *                 if (data.contains("value")) {
 *                     comp.value = data["value"].get<int>();
 *                 }
 *                 s.addComponent(e, comp);
 *             });
 *     }
 * };
 * @endcode
 *
 * Usage in scene loader:
 * @code
 * GMapLoader loader;
 * ComponentRegistry::get().applyAllParsers(loader);
 * loader.loadMap(scene, "map", "map.gmap");
 * @endcode
 */
class ComponentRegistry {
private:
    std::unordered_map<std::string, ComponentParser> parsers_;

    // Singleton
    ComponentRegistry() = default;

public:
    // Delete copy/move
    ComponentRegistry(const ComponentRegistry&) = delete;
    ComponentRegistry& operator=(const ComponentRegistry&) = delete;

    /**
     * @brief Gets the singleton instance
     * @return Reference to the global registry
     */
    static ComponentRegistry& get() {
        static ComponentRegistry instance;
        return instance;
    }

    /**
     * @brief Registers a component parser globally
     *
     * @param componentType The component type name (e.g., "PositionComponent")
     * @param parser The parser function
     *
     * @note If a parser already exists for this type, it will be overwritten with a warning
     */
    void registerParser(const std::string& componentType, ComponentParser parser) {
        if (parsers_.find(componentType) != parsers_.end()) {
            std::cerr << "[ComponentRegistry] Warning: Overwriting parser for "
                      << componentType << std::endl;
        }
        parsers_[componentType] = parser;
        std::cout << "[ComponentRegistry] ✓ Registered: " << componentType << std::endl;
    }

    /**
     * @brief Applies all registered parsers to a GMapLoader
     *
     * This should be called once before loading maps.
     *
     * @param loader The loader to configure with all registered parsers
     */
    void applyAllParsers(GMapLoader& loader) {
        for (const auto& [componentType, parser] : parsers_) {
            loader.registerParser(componentType, parser);
        }
        std::cout << "[ComponentRegistry] Applied " << parsers_.size()
                  << " parser(s) to loader" << std::endl;
    }

    /**
     * @brief Gets the number of registered parsers
     * @return Number of parsers currently registered
     */
    size_t count() const {
        return parsers_.size();
    }

    /**
     * @brief Checks if a parser is registered
     *
     * @param componentType Component type to check
     * @return true if parser exists, false otherwise
     */
    bool hasParser(const std::string& componentType) const {
        return parsers_.find(componentType) != parsers_.end();
    }

    /**
     * @brief Clears all registered parsers
     *
     * Useful for testing or resetting state.
     */
    void clear() {
        parsers_.clear();
        std::cout << "[ComponentRegistry] Cleared all parsers" << std::endl;
    }

    /**
     * @brief Lists all registered component types
     * @return Vector of registered component type names
     */
    std::vector<std::string> listRegistered() const {
        std::vector<std::string> types;
        for (const auto& [type, _] : parsers_) {
            types.push_back(type);
        }
        return types;
    }
};

} // namespace ECS

#endif // COMPONENT_REGISTRY_HPP

