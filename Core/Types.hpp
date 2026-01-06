#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>
#include <string>
#include <functional>
#include <ostream>
#include <memory>
#include <typeindex>
#include <array>
#include <bitset>
#include <vector>
#include <algorithm>

// Include IComponent interface (needed for setActive usage)
#include <Graphitech/ECS/Core/IComponent.hpp>
#include <Graphitech/ECS/Core/AComponent.hpp>

namespace ECS {

class IEntity;

/**
 * @brief Type alias for component identifiers.
 *
 * @details ComponentID is used to uniquely identify different component types
 * within the ECS system. Each component type gets a unique ID assigned at runtime.
 */
using ComponentID = std::size_t;

/**
 * @brief Generates a new unique component ID.
 *
 * @details Each call returns a unique identifier starting from 0.
 *
 * @return A new unique component identifier.
 */
inline ComponentID getNewComponentID() {
    static ComponentID lastID = 0;
    return lastID++;
}

/**
 * @brief Gets the unique type ID for a component type.
 *
 * @details This template function assigns a unique ID to each component type T.
 * The ID is generated once per type and reused for all instances of that type.
 *
 * @tparam T The component type (must inherit from ECS::IComponent).
 * @return The unique type ID for component type T.
 */
template<typename T>
inline ComponentID getComponentTypeID() noexcept {
    static_assert(std::is_base_of<IComponent, T>::value, "T must inherit from ECS::IComponent");
    static ComponentID typeID = getNewComponentID();
    return typeID;
}

/**
 * @brief Maximum number of component types supported.
 *
 * @details This constant defines the maximum number of different component types
 * that can be registered in the ECS system. It determines the size of bitsets
 * and arrays used for component tracking.
 */
constexpr std::size_t maxComponents = 64;

/**
 * @brief Type alias for component presence tracking.
 *
 * @details ComponentBitSet is used to efficiently track which components
 * are attached to an entity. Each bit represents a component type.
 */
using ComponentBitSet = std::bitset<maxComponents>;

/**
 * @brief Type alias for fast component lookup array.
 *
 * @details ComponentArray provides O(1) access to components by their type ID.
 * It stores pointers to components for quick retrieval.
 */
using ComponentArray = std::array<IComponent*, maxComponents>;

/**
 * @brief Represents an entity in the ECS architecture.
 *
 * @details The Entity class is a lightweight identifier that holds an ID and manages
 * component attachments. Entities themselves don't store component data directly;
 * instead, they reference components managed by the Scene. This design allows for
 * efficient entity management and component querying.
 *
 * Key features:
 * - Unique ID-based identification
 * - Component attachment and retrieval via templates
 * - Bitset-based component tracking for fast queries
 * - Copy semantics that preserve ID but not component data
 * - Support for up to 64 different component types
 *
 * @note Components are not copied during entity copy operations.
 * @note Entity with ID 0 is considered invalid/null.
 *
 * @ingroup ECS
 */
class Entity {
    uint32_t id_; /**< Unique identifier for the entity */
    std::vector<std::unique_ptr<IComponent>> components_; /**< Component storage (managed by Scene) */
    ComponentArray componentArray_; /**< Fast lookup array for component types */
    ComponentBitSet componentBitSet_; /**< Bitset tracking attached component types */

public:
    /**
     * @brief Default constructor creating an invalid entity.
     */
    Entity() : id_(0), componentArray_{}, componentBitSet_{} {}

    /**
     * @brief Constructs an entity with the specified ID.
     *
     * @param id The unique identifier for this entity.
     */
    explicit Entity(uint32_t id) : id_(id), componentArray_{}, componentBitSet_{} {}
    
    Entity(const Entity& other) 
        : id_(other.id_), componentArray_{}, componentBitSet_(other.componentBitSet_) {}
    
    Entity& operator=(const Entity& other) {
        if (this != &other) {
            id_ = other.id_;
            componentBitSet_ = other.componentBitSet_;
            componentArray_ = {};
        }
        return *this;
    }
    
    Entity(Entity&&) = default;
    Entity& operator=(Entity&&) = default;

    /**
     * @brief Gets the entity's unique identifier.
     *
     * @return The entity's ID.
     */
    uint32_t id() const { return id_; }
    
    /**
     * @brief Equality comparison operator.
     *
     * @param other The entity to compare with.
     * @return true if both entities have the same ID.
     */
    bool operator==(const Entity& other) const { return id_ == other.id_; }

    /**
     * @brief Inequality comparison operator.
     *
     * @param other The entity to compare with.
     * @return true if entities have different IDs.
     */
    bool operator!=(const Entity& other) const { return id_ != other.id_; }

    /**
     * @brief Less-than comparison operator for ordering.
     *
     * @param other The entity to compare with.
     * @return true if this entity's ID is less than the other's.
     */
    bool operator<(const Entity& other) const { return id_ < other.id_; }
    
    /**
     * @brief Boolean conversion operator.
     *
     * @return true if the entity is valid (ID != 0), false otherwise.
     */
    explicit operator bool() const { return id_ != 0; }
    
    /**
     * @brief Adds a component to this entity.
     *
     * @details Creates a new instance of the component type T and attaches it to this entity.
     * The component is stored in the entity's component array and marked as present in the bitset.
     *
     * @tparam T The component type to add (must inherit from ECS::IComponent).
     * @param component The component instance to add.
     * @return Reference to this entity for method chaining.
     */
    template<typename T>
    Entity& addComponent(T component) {
        static_assert(std::is_base_of<IComponent, T>::value, "Component must inherit from ECS::IComponent");
        
        T* c = new T(std::forward<T>(component));
        std::unique_ptr<IComponent> ptr(c);
        components_.emplace_back(std::move(ptr));
        
        componentArray_[getComponentTypeID<T>()] = c;
        componentBitSet_[getComponentTypeID<T>()] = true;
        
        return *this;
    }
    
    /**
     * @brief Gets a component from this entity.
     *
     * @details Retrieves a pointer to the component of type T attached to this entity.
     * Returns nullptr if the component is not present.
     *
     * @tparam T The component type to retrieve (must inherit from ECS::IComponent).
     * @return Pointer to the component, or nullptr if not found.
     */
    template<typename T>
    T* getComponent() {
        static_assert(std::is_base_of<IComponent, T>::value, "Component must inherit from ECS::IComponent");
        
        if (!hasComponent<T>())
            return nullptr;
        
        auto* ptr = componentArray_[getComponentTypeID<T>()];
        return static_cast<T*>(ptr);
    }
    
    /**
     * @brief Gets a component from this entity (const version).
     *
     * @details Retrieves a const pointer to the component of type T attached to this entity.
     * Returns nullptr if the component is not present.
     *
     * @tparam T The component type to retrieve (must inherit from ECS::IComponent).
     * @return Const pointer to the component, or nullptr if not found.
     */
    template<typename T>
    const T* getComponent() const {
        static_assert(std::is_base_of<IComponent, T>::value, "Component must inherit from ECS::IComponent");
        
        if (!hasComponent<T>())
            return nullptr;
        
        auto* ptr = componentArray_[getComponentTypeID<T>()];
        return static_cast<const T*>(ptr);
    }
    
    /**
     * @brief Checks if this entity has a component of the specified type.
     *
     * @details Uses the component bitset to efficiently check component presence
     * without accessing the component array.
     *
     * @tparam T The component type to check (must inherit from ECS::IComponent).
     * @return true if the component is present, false otherwise.
     */
    template<typename T>
    bool hasComponent() const {
        static_assert(std::is_base_of<IComponent, T>::value, "Component must inherit from ECS::IComponent");
        return componentBitSet_[getComponentTypeID<T>()];
    }
    
    /**
     * @brief Removes a component from this entity.
     *
     * @details Deactivates and removes the component of type T from this entity.
     * The component is marked as inactive and removed from the tracking structures.
     *
     * @tparam T The component type to remove (must inherit from ECS::IComponent).
     */
    template<typename T>
    void removeComponent() {
        static_assert(std::is_base_of<IComponent, T>::value, "Component must inherit from ECS::IComponent");
        
        if (!hasComponent<T>())
            return;
        
        ComponentID typeID = getComponentTypeID<T>();
        
        if (componentArray_[typeID]) {
            componentArray_[typeID]->setActive(false);
        }
        
        componentArray_[typeID] = nullptr;
        componentBitSet_[typeID] = false;
    }
    
    /**
     * @brief Gets the number of components attached to this entity.
     *
     * @details Counts the number of bits set in the component bitset,
     * which represents the number of different component types attached.
     *
     * @return The number of component types attached to this entity.
     */
    size_t componentCount() const {
        return componentBitSet_.count();
    }
    
    /**
     * @brief Gets the component bitset for this entity.
     *
     * @details Provides access to the internal bitset used for component tracking.
     * This is useful for efficient querying and matching operations.
     *
     * @return Const reference to the component bitset.
     */
    const ComponentBitSet& getComponentBitSet() const {
        return componentBitSet_;
    }
};

/**
 * @brief Output stream operator for Entity.
 *
 * @details Allows Entity objects to be written to output streams,
 * outputting the entity's ID.
 *
 * @param os The output stream.
 * @param e The entity to output.
 * @return Reference to the output stream.
 */
inline std::ostream& operator<<(std::ostream& os, const Entity& e) {
    return os << e.id();
}

} // namespace ECS

namespace std {
    /**
     * @brief Hash function specialization for ECS::Entity.
     *
     * @details Enables Entity objects to be used as keys in unordered containers
     * like std::unordered_map and std::unordered_set. The hash is based on the entity's ID.
     */
    template<>
    struct hash<ECS::Entity> {
        /**
         * @brief Computes hash value for an Entity.
         *
         * @param e The entity to hash.
         * @return Hash value based on the entity's ID.
         */
        size_t operator()(const ECS::Entity& e) const {
            return hash<uint32_t>()(e.id());
        }
    };
}

namespace ECS {

/**
 * @brief Event structure for map-related input events.
 *
 * @details Contains information about mouse or keyboard events that occur within the game map.
 * Used for handling user input and triggering game logic based on interaction events.
 *
 * @ingroup ECS
 */
struct MapEvent {
    /**
     * @brief Enumeration of possible input event types.
     */
    enum class Type { 
        Mouse = 0, /**< Mouse input event */
        Keyboard = 1 /**< Keyboard input event */
    } type = Type::Mouse; /**< Type of input event */
    int x = 0; /**< X-coordinate of mouse event or unused for keyboard */
    int y = 0; /**< Y-coordinate of mouse event or unused for keyboard */
    int button = 0; /**< Mouse button or unused for keyboard */
    int key = 0; /**< Key code for keyboard events or unused for mouse */
    int action = 0; /**< Action type (press, release, etc.) */
};

/**
 * @brief Structure representing a UI item for display.
 *
 * @details Defines the properties of a user interface element including text content,
 * position, and color. Used for rendering text-based UI elements in the game.
 *
 * @ingroup ECS
 */
struct UIItem {
    std::string text; /**< Text content to display */
    int x = 0; /**< X-coordinate position */
    int y = 0; /**< Y-coordinate position */
    float r = 1.f, g = 1.f, b = 1.f; /**< RGB color components (0.0-1.0) */
};

/**
 * @brief Factory class for generating unique entity IDs.
 *
 * @details Provides a simple mechanism for creating new entities with sequentially
 * assigned unique IDs. Ensures that each entity has a distinct identifier within
 * the ECS system.
 *
 * @ingroup ECS
 */
class EntityGenerator {
    uint32_t next = 1; /**< Next available entity ID */

public:
    /**
     * @brief Creates a new entity with a unique ID.
     *
     * @return A new Entity instance with the next available ID.
     */
    Entity create() { return Entity(next++); }
};

} // namespace ECS

#endif // TYPES_HPP
