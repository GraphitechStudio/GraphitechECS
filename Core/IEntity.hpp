/*
** EPITECH PROJECT, 2025
** GraphitechEngine
** File description:
** IEntity.hpp
*/

#ifndef IENTITY_HPP
#define IENTITY_HPP
#include <string>

namespace ECS {
    class IComponent;

    /**
     * @class IEntity
     * @brief Interface for entities in the Entity Component System
     *
     * IEntity defines the basic contract that all entities in the ECS must implement.
     * An entity represents a game object that can have various components attached to it.
     * This interface provides methods for managing the entity's lifecycle and basic properties.
     *
     * @note This is an interface that must be implemented by concrete entity classes.
     */
    class IEntity {
        public:
            /**
             * @brief Virtual destructor for proper cleanup of derived classes
             */
            virtual ~IEntity() = default;

        protected:
            /**
             * @brief Get the name of the entity
             * @return A string containing the entity's name
             *
             * Returns the human-readable name assigned to this entity.
             * Names are typically used for debugging and identification purposes.
             */
            virtual std::string getName() const = 0;

            /**
             * @brief Set the name of the entity
             * @param name The new name to assign to the entity
             *
             * Changes the human-readable name of this entity.
             * Names should be unique within a scene for better identification.
             */
            virtual void setName(const std::string &name) = 0;

            /**
             * @brief Check if the entity is currently active
             * @return true if the entity is active, false otherwise
             *
             * An active entity participates in the game loop and can be updated/rendered.
             * Inactive entities are typically skipped during processing.
             */
            virtual bool isActive() const = 0;

            /**
             * @brief Mark the entity for destruction
             *
             * This method signals that the entity should be removed from the scene
             * and its resources cleaned up. The actual destruction may be deferred
             * until the end of the current frame to avoid issues during iteration.
             */
            virtual void destroy() = 0;
    };
}

#endif //IENTITY_HPP
