/*
** EPITECH PROJECT, 2025
** GraphitechEngine
** File description:
** IComponent.hpp
*/

#ifndef ICOMPONENT_HPP
#define ICOMPONENT_HPP

namespace ECS {
    class IEntity;

    /**
     * @class IComponent
     * @brief Interface for components in the Entity Component System
     *
     * IComponent defines the basic contract that all components in the ECS must implement.
     * Components are data structures that can be attached to entities to give them specific
     * behaviors or properties. Each component type represents a different aspect of an entity
     * (position, rendering, physics, etc.).
     *
     * @note This is an interface that must be implemented by concrete component classes.
     * @note Components are typically managed by the ECS and attached to entities.
     */
    class IComponent {
        public:
            /**
             * @brief Virtual destructor for proper cleanup of derived classes
             */
            virtual ~IComponent() = default;

            /**
             * @brief Check if the component is currently active
             * @return true if the component is active, false otherwise
             *
             * An active component participates in the game loop and can be updated/processed.
             * Inactive components are typically skipped during processing to improve performance.
             */
            virtual bool isActive() const = 0;

            /**
             * @brief Set the active state of the component
             * @param active The new active state to set
             *
             * When set to false, the component will be ignored during game loop processing.
             * When set to true, the component will participate in updates and rendering.
             */
            virtual void setActive(bool active) = 0;

            /**
             * @brief Associate this component with an entity
             * @param entity Pointer to the entity that owns this component
             *
             * This method establishes the relationship between the component and its owning entity.
             * Components typically need access to their entity for various operations.
             *
             * @note The component does not own the entity pointer - it should not delete it.
             * @warning entity can be nullptr if the component is not attached to any entity.
             */
            virtual void setEntity(IEntity *entity) = 0;
    };
}

#endif //ICOMPONENT_HPP
