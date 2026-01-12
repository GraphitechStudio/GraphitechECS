/*
** EPITECH PROJECT, 2025
** GraphitechEngine
** File description:
** AEntity.hpp
*/
#ifndef AENTITY_HPP
#define AENTITY_HPP
#include <array>
#include <bitset>
#include <memory>
#include <vector>

#include "Core/IComponent.hpp"
#include "Core/IEntity.hpp"

using ComponentID = std::size_t;
using Group = std::size_t;

inline ComponentID getNewComponentID()
{
    static ComponentID lastID = 0;
    return lastID++;
}

template<typename T> inline ComponentID getComponentTypeID() noexcept
{
    static ComponentID typeID = getNewComponentID();
    return typeID;
}

constexpr std::size_t maxComponents = 32;
constexpr std::size_t maxGroups = 32;

using ComponentBitSet = std::bitset<maxComponents>;
using GroupBitSet = std::bitset<maxGroups>;
using ComponentArray = std::array<ECS::IComponent *, maxComponents>;

namespace ECS {
    class Scene;

    /**
     * @brief Concrete implementation of the IEntity interface in the ECS architecture.
     *
     * @details The AEntity class represents an entity in the Entity-Component-System (ECS) framework.
     * It manages components attached to the entity, handles grouping for efficient querying,
     * and provides lifecycle management. Entities are created within a Scene and can have
     * multiple components of different types attached to them. The class uses bitsets for
     * fast component and group lookups, and templates for type-safe component operations.
     *
     * Key features:
     * - Component management with type-safe add/get operations
     * - Group membership for entity categorization
     * - Activity state control (active/inactive)
     * - Name-based identification
     * - Automatic component ID generation for type registration
     *
     * @note This class is final and cannot be inherited from.
     * @note Maximum component types and groups are limited to 32 each for performance.
     *
     * @ingroup ECS
     */
    class AEntity final : public IEntity {
        private:
            Scene &_scene; /**< Reference to the scene this entity belongs to */
            std::string _name; /**< Name identifier for the entity */
            bool _active = true; /**< Activity state of the entity */
            std::vector<std::unique_ptr<IComponent> > components; /**< Storage for all attached components */

            ComponentArray _componentArray = {}; /**< Array mapping component types to instances */
            ComponentBitSet _componentBitSet; /**< Bitset tracking which component types are present */
            GroupBitSet _groupBitSet; /**< Bitset tracking group memberships */

        public:
            /**
             * @brief Constructs a new entity within the specified scene.
             *
             * @param scene Reference to the scene that will contain this entity.
             */
            explicit AEntity(Scene &scene) : _scene(scene) {}

            /**
             * @brief Checks if the entity belongs to the specified group.
             *
             * @param group The group ID to check membership for.
             * @return true if the entity is in the group, false otherwise.
             */
            bool hasGroup(const Group group) { return this->_groupBitSet[group]; }

            /**
             * @brief Adds the entity to the specified group.
             *
             * @param group The group ID to add the entity to.
             */
            void addGroup(Group group);

            /**
             * @brief Removes the entity from the specified group.
             *
             * @param group The group ID to remove the entity from.
             */
            void delGroup(const Group group) { this->_groupBitSet[group] = false; }

            /**
             * @brief Checks if the entity has a component of the specified type.
             *
             * @tparam T The component type to check for.
             * @return true if the entity has the component, false otherwise.
             */
            template<typename T> bool hasComponent() const { return this->_componentBitSet[getComponentTypeID<T>()]; };

            /**
             * @brief Adds a component of the specified type to the entity.
             *
             * @tparam T The component type to add. Must be derived from IComponent.
             * @param component The component instance to add (moved into the entity).
             * @return Reference to this entity for method chaining.
             *
             * @note The component's entity pointer is automatically set to this entity.
             */
            template<typename T> IEntity &addComponent(T component)
            {
                T *c(new T(std::forward<T>(component)));
                c->setEntity(this);
                std::unique_ptr<IComponent> ptr(c);
                this->components.emplace_back(std::move(ptr));
                this->_componentArray[getComponentTypeID<T>()] = c;
                this->_componentBitSet[getComponentTypeID<T>()] = true;
                return *this;
            }

            /**
             * @brief Retrieves a reference to the component of the specified type.
             *
             * @tparam T The component type to retrieve.
             * @return Reference to the component instance.
             *
             * @throws std::runtime_error if the component is not present on the entity.
             * @note Assumes the component exists; check with hasComponent() first.
             */
            template<typename T> T &getComponent() const
            {
                auto ptr(this->_componentArray[getComponentTypeID<T>()]);
                return *static_cast<T *>(ptr);
            }

            /**
             * @brief Checks if the entity is currently active.
             *
             * @return true if the entity is active, false if it has been destroyed.
             */
            bool isActive() const override { return this->_active; };

            /**
             * @brief Marks the entity for destruction, making it inactive.
             *
             * @details Once destroyed, the entity will be removed from the scene during
             * the next update cycle. The entity remains in memory until cleanup.
             */
            void destroy() override { this->_active = false; };

            /**
             * @brief Gets the name of the entity.
             *
             * @return The entity's name as a string.
             */
            std::string getName() const override { return this->_name; };

            /**
             * @brief Sets the name of the entity.
             *
             * @param name The new name for the entity.
             */
            void setName(const std::string &name) override { this->_name = name; };
    };
}

#endif //AENTITY_HPP
