#ifndef COMPONENT_HPP
    #define COMPONENT_HPP

#include <Graphitech/ECS/Core/Types.hpp>
#include <unordered_map>

namespace ECS {

    /**
     * @brief Interface for component storage.
     *
     * @details Provides a type-erased interface for storing and managing components
     * of any type. This allows the ECS system to handle different component types
     * uniformly without knowing their specific types at compile time.
     *
     * @ingroup ECS
     */
    struct IComponentStorage {
        /**
         * @brief Virtual destructor for proper cleanup of derived classes.
         */
        virtual ~IComponentStorage() = default;
        
        /**
         * @brief Removes the component associated with an entity.
         *
         * @param e The entity whose component should be removed.
         */
        virtual void remove(Entity e) = 0;
        
        /**
         * @brief Gets the number of stored components.
         *
         * @return The number of components currently stored.
         */
        virtual size_t size() const { return 0; }
    };

    /**
     * @brief Template implementation of component storage.
     *
     * @details Stores components of type T using an unordered map with entities as keys.
     * This provides O(1) average-case lookup, insertion, and removal of components.
     *
     * @tparam T The component type to store (must be copyable/movable).
     *
     * @ingroup ECS
     */
    template<typename T>
    struct ComponentStorage : IComponentStorage {
        std::unordered_map<Entity, T> data; /**< Map of entities to their components */
        
        /**
         * @brief Removes the component for the specified entity.
         *
         * @param e The entity whose component should be removed.
         */
        void remove(Entity e) override {
            data.erase(e);
        }
        
        /**
         * @brief Gets the number of components stored.
         *
         * @return The number of components in this storage.
         */
        size_t size() const override { 
            return data.size(); 
        }
    };

} // namespace ECS
#endif