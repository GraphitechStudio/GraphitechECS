/*
** EPITECH PROJECT, 2025
** GraphitechEngine
** File description:
** AComponent.hpp
*/
#ifndef ACOMPONENT_HPP
#define ACOMPONENT_HPP
#include <iostream>

#include <Graphitech/ECS/Core/IComponent.hpp>

namespace ECS {
    /**
     * @brief Abstract base class for all components in the ECS architecture.
     *
     * @details The AComponent class provides a default implementation of the IComponent interface,
     * serving as a foundation for concrete component classes. It manages the component's active state
     * and its association with an entity. All game components should inherit from this class to
     * ensure consistent behavior and interface compliance.
     *
     * Key features:
     * - Active/inactive state management
     * - Entity association tracking
     * - Virtual method implementations for common component operations
     *
     * @note This class is intended to be subclassed; it provides default implementations
     * but concrete components may override methods as needed.
     *
     * @ingroup ECS
     */
    class AComponent : public ECS::IComponent {
        protected:
            bool _isActive = true; /**< Active state of the component */
            IEntity *_entity = nullptr; /**< Pointer to the entity this component belongs to */

        public:
            /**
             * @brief Checks if the component is currently active.
             *
             * @return true if the component is active, false otherwise.
             */
            bool isActive() const override { return this->_isActive; };

            /**
             * @brief Sets the active state of the component.
             *
             * @param active The new active state for the component.
             */
            void setActive(bool active) override { this->_isActive = active; };

            /**
             * @brief Associates the component with an entity.
             *
             * @param entity Pointer to the entity that owns this component.
             */
            void setEntity(IEntity *entity) override { this->_entity = entity; };
    };
}

#endif //ACOMPONENT_HPP
