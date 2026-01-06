#ifndef ENTITY_FACTORY_HPP
    #define ENTITY_FACTORY_HPP
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include "../Core/Types.hpp"

namespace ECS {
class Scene; // forward
class Entity; // forward
class MapTag; // forward

/**
 * @brief Factory class for creating entities with predefined configurations.
 *
 * @details EntityFactory provides a registry system for entity creation functions.
 * Factory functions can be registered with unique names and later called to create
 * entities with specific component configurations. This pattern is useful for:
 * - Creating entities with complex component setups
 * - Defining entity templates (e.g., "player", "enemy", "projectile")
 * - Reusing entity creation logic across different parts of the application
 * - Supporting data-driven entity creation
 *
 * @ingroup ECS
 */
class EntityFactory {
    public:
        /**
         * @brief Type alias for factory functions.
         *
         * @details Factory functions receive a Scene reference, entity name, and Entity.
         * They are responsible for adding components and configuring the entity.
         */
        using FactoryFn = std::function<void(Scene&, const std::string&, Entity)>;

        /**
         * @brief Registers a factory function with a given name.
         *
         * @param name The unique name identifier for this factory.
         * @param fn The factory function to register.
         * @return true if registration succeeded, false if the name already exists.
         */
        bool registerFactory(const std::string& name, FactoryFn fn);

        /**
         * @brief Unregisters a factory function.
         *
         * @param name The name of the factory to remove.
         * @return true if a factory was removed, false if the name wasn't found.
         */
        bool unregisterFactory(const std::string& name);

        /**
         * @brief Checks if a factory with the given name exists.
         *
         * @param name The factory name to check.
         * @return true if the factory exists, false otherwise.
         */
        bool hasFactory(const std::string& name) const;

        /**
         * @brief Retrieves a factory function by name.
         *
         * @param name The factory name to look up.
         * @return Pointer to the factory function, or nullptr if not found.
         */
        const FactoryFn* getFactory(const std::string& name) const;

        /**
         * @brief Lists all registered factory names.
         *
         * @return Vector containing the names of all registered factories.
         */
        std::vector<std::string> listFactories() const;
    private:
        std::unordered_map<std::string, FactoryFn> factories; /**< Map of factory names to functions */
};

/**
 * @brief Gets the global EntityFactory instance.
 *
 * @details Provides access to a singleton EntityFactory that can be used
 * throughout the application for registering and retrieving entity factories.
 *
 * @return Reference to the global EntityFactory instance.
 */
EntityFactory& getEntityFactory();

} // namespace ECS
#endif