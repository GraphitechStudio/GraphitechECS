/**
 * @file Scene.hpp
 * @brief Core ECS scene and game map management.
 *
 * @details This file contains the Scene class which manages entities, components,
 * and game maps. It also defines the map lifecycle interfaces (IGameMap, AGameMap)
 * and a callback-based implementation (CallbackGameMap).
 */

#ifndef SCENE_HPP
#define SCENE_HPP

#include <Graphitech/ECS/Core/Types.hpp>
#include <Graphitech/ECS/Core/SpatialGrid.hpp>
#include <Graphitech/ECS/Component.hpp>
#include <Graphitech/ECS/MapLoader.hpp>
#include <Graphitech/ECS/Factories/EntityFactory.hpp>
#include <any>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ECS {

class Scene;

/**
 * @brief Interface for game map lifecycle management.
 *
 * @details Defines the contract for game maps with lifecycle hooks for
 * starting, stopping, entering, exiting, ticking, rendering, and event handling.
 *
 * @ingroup ECS
 */
class IGameMap {
public:
    /**
     * @brief Virtual destructor for proper cleanup.
     */
    virtual ~IGameMap() = default;
    
    /**
     * @brief Gets the name of this map.
     *
     * @return The map's unique identifier.
     */
    virtual const std::string& getName() const = 0;
    
    /**
     * @brief Called when the map is started for the first time.
     *
     * @param scene The scene containing this map.
     */
    virtual void onStart(Scene& scene) = 0;
    
    /**
     * @brief Called when the map is stopped.
     *
     * @param scene The scene containing this map.
     */
    virtual void onStop(Scene& scene) = 0;
    
    /**
     * @brief Called when transitioning to this map.
     *
     * @param scene The scene containing this map.
     */
    virtual void onEnter(Scene& scene) = 0;
    
    /**
     * @brief Called when leaving this map.
     *
     * @param scene The scene containing this map.
     */
    virtual void onExit(Scene& scene) = 0;
    
    /**
     * @brief Called every frame to update the map.
     *
     * @param scene The scene containing this map.
     * @param dt Delta time since last tick in seconds.
     */
    virtual void onTick(Scene& scene, float dt) = 0;
    
    /**
     * @brief Called to render the map.
     *
     * @param scene The scene containing this map.
     */
    virtual void onRender(Scene& scene) = 0;
    
    /**
     * @brief Called when an event occurs in this map.
     *
     * @param scene The scene containing this map.
     * @param event The event data.
     * @return true if the event was handled, false otherwise.
     */
    virtual bool onEvent(Scene& scene, const MapEvent& event) = 0;
};

/**
 * @brief Abstract base implementation of IGameMap.
 *
 * @details Provides concrete implementations for entity management, system execution,
 * and value storage. Maps can store entities, register update systems, and maintain
 * arbitrary key-value data.
 *
 * @ingroup ECS
 */
class AGameMap : public IGameMap {
protected:
    std::string name_; /**< Name of the map */
    std::unordered_set<Entity> entities_; /**< Entities belonging to this map */
    std::vector<std::function<void(Scene&, float)>> systems_; /**< Update systems */
    std::unordered_map<std::string, std::any> values_; /**< Arbitrary key-value storage */
    std::optional<SpatialGrid> spatialGrid_; /**< Optional spatial partitioning grid */
    bool started_ = false; /**< Whether the map has been started */

public:
    /**
     * @brief Constructs a map with the given name.
     *
     * @param name The unique identifier for this map.
     */
    explicit AGameMap(std::string name) : name_(std::move(name)) {}
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~AGameMap() = default;

    /**
     * @brief Gets the map name.
     *
     * @return The map's identifier.
     */
    const std::string& getName() const override { return name_; }
    
    /**
     * @brief Adds an entity to this map.
     *
     * @param e The entity to add.
     */
    void addEntity(Entity e) { entities_.insert(e); }
    
    /**
     * @brief Removes an entity from this map.
     *
     * @param e The entity to remove.
     */
    void removeEntity(Entity e) { entities_.erase(e); }
    
    /**
     * @brief Gets all entities in this map.
     *
     * @return Const reference to the entity set.
     */
    const std::unordered_set<Entity>& getEntities() const { return entities_; }
    
    /**
     * @brief Checks if an entity belongs to this map.
     *
     * @param e The entity to check.
     * @return true if the entity is in this map, false otherwise.
     */
    bool hasEntity(Entity e) const { return entities_.count(e) > 0; }
    
    /**
     * @brief Adds an update system to this map.
     *
     * @param sys The system function to execute during updates.
     */
    void addSystem(std::function<void(Scene&, float)> sys) {
        systems_.push_back(std::move(sys));
    }
    
    /**
     * @brief Stores a value in the map's key-value storage.
     *
     * @tparam T The type of value to store.
     * @param key The key identifier.
     * @param value The value to store.
     */
    template<typename T>
    void setValue(const std::string& key, T value) {
        values_[key] = std::any(std::move(value));
    }
    
    /**
     * @brief Retrieves a value from the map's storage.
     *
     * @tparam T The expected type of the value.
     * @param key The key identifier.
     * @return Pointer to the value, or nullptr if not found or wrong type.
     */
    template<typename T>
    T* getValue(const std::string& key) {
        auto it = values_.find(key);
        if (it == values_.end()) return nullptr;
        return std::any_cast<T>(&it->second);
    }
    
    /**
     * @brief Removes a value from the storage.
     *
     * @param key The key to remove.
     * @return true if a value was removed, false if key didn't exist.
     */
    bool removeValue(const std::string& key) {
        return values_.erase(key) > 0;
    }
    
    /**
     * @brief Checks if the map has been started.
     *
     * @return true if started, false otherwise.
     */
    bool isStarted() const { return started_; }
    
    /**
     * @brief Sets the started state.
     *
     * @param s The new started state.
     */
    void setStarted(bool s) { started_ = s; }
    
    /**
     * @brief Executes all registered systems.
     *
     * @param scene The scene containing this map.
     * @param dt Delta time since last update.
     */
    void runSystems(Scene& scene, float dt) {
        for (auto& sys : systems_) {
            sys(scene, dt);
        }
    }

    /**
     * @brief Enables spatial partitioning for this map.
     *
     * @param cellSize Size of each grid cell (default: 10.0f).
     */
    void enableSpatialGrid(float cellSize = 10.0f) {
        spatialGrid_.emplace(cellSize);
    }

    /**
     * @brief Checks if spatial grid is enabled.
     *
     * @return true if spatial grid is active, false otherwise.
     */
    bool hasSpatialGrid() const {
        return spatialGrid_.has_value();
    }

    /**
     * @brief Updates an entity's position in the spatial grid.
     *
     * @param e The entity to update.
     * @param pos The entity's world position.
     */
    void updateEntityPosition(Entity e, const glm::vec3& pos) {
        if (spatialGrid_) {
            spatialGrid_->updateEntity(e, pos);
        }
    }

    /**
     * @brief Queries entities near a position.
     *
     * @param pos Center position for the query.
     * @param radius Search radius.
     * @return Vector of entities within the radius (empty if spatial grid not enabled).
     */
    std::vector<Entity> queryNearby(const glm::vec3& pos, float radius) const {
        if (spatialGrid_) {
            return spatialGrid_->getEntitiesNear(pos, radius);
        }
        return {};
    }

    /**
     * @brief Removes an entity from the spatial grid.
     *
     * @param e The entity to remove.
     */
    void removeEntityFromGrid(Entity e) {
        if (spatialGrid_) {
            spatialGrid_->removeEntity(e);
        }
    }

    /**
     * @brief Gets direct access to the spatial grid.
     *
     * @return Pointer to the spatial grid, or nullptr if not enabled.
     */
    SpatialGrid* getSpatialGrid() {
        return spatialGrid_ ? &(*spatialGrid_) : nullptr;
    }

    /**
     * @brief Gets direct access to the spatial grid (const version).
     *
     * @return Const pointer to the spatial grid, or nullptr if not enabled.
     */
    const SpatialGrid* getSpatialGrid() const {
        return spatialGrid_ ? &(*spatialGrid_) : nullptr;
    }
};

/**
 * @brief Callback-based implementation of game maps.
 *
 * @details Allows setting custom callbacks for each lifecycle event instead of
 * subclassing. This is useful for dynamically configured maps or scripted behavior.
 *
 * @ingroup ECS
 */
class CallbackGameMap : public AGameMap {
    std::function<void(Scene&)> onStartCb_;
    std::function<void(Scene&)> onStopCb_;
    std::function<void(Scene&)> onEnterCb_;
    std::function<void(Scene&)> onExitCb_;
    std::function<void(Scene&, float)> onTickCb_;
    std::function<void(Scene&)> onRenderCb_;
    std::function<bool(Scene&, const MapEvent&)> onEventCb_;

public:
    /**
     * @brief Constructs a callback-based map.
     *
     * @param name The map identifier.
     */
    explicit CallbackGameMap(std::string name) : AGameMap(std::move(name)) {}

    /** @brief Sets the onStart callback. */
    void setOnStart(std::function<void(Scene&)> cb) { onStartCb_ = std::move(cb); }
    
    /** @brief Sets the onStop callback. */
    void setOnStop(std::function<void(Scene&)> cb) { onStopCb_ = std::move(cb); }
    
    /** @brief Sets the onEnter callback. */
    void setOnEnter(std::function<void(Scene&)> cb) { onEnterCb_ = std::move(cb); }
    
    /** @brief Sets the onExit callback. */
    void setOnExit(std::function<void(Scene&)> cb) { onExitCb_ = std::move(cb); }
    
    /** @brief Sets the onTick callback. */
    void setOnTick(std::function<void(Scene&, float)> cb) { onTickCb_ = std::move(cb); }
    
    /** @brief Sets the onRender callback. */
    void setOnRender(std::function<void(Scene&)> cb) { onRenderCb_ = std::move(cb); }
    
    /** @brief Sets the onEvent callback. */
    void setOnEvent(std::function<bool(Scene&, const MapEvent&)> cb) { onEventCb_ = std::move(cb); }

    void onStart(Scene& scene) override { if (onStartCb_) onStartCb_(scene); }
    void onStop(Scene& scene) override { if (onStopCb_) onStopCb_(scene); }
    void onEnter(Scene& scene) override { if (onEnterCb_) onEnterCb_(scene); }
    void onExit(Scene& scene) override { if (onExitCb_) onExitCb_(scene); }
    void onTick(Scene& scene, float dt) override { 
        if (onTickCb_) onTickCb_(scene, dt);
        runSystems(scene, dt);
    }
    void onRender(Scene& scene) override { if (onRenderCb_) onRenderCb_(scene); }
    bool onEvent(Scene& scene, const MapEvent& event) override { 
        return onEventCb_ ? onEventCb_(scene, event) : false; 
    }
};

/**
 * @brief Main scene class managing entities, components, and game maps.
 *
 * @details The Scene is the core container of the ECS architecture. It:
 * - Creates and manages entities
 * - Stores components in type-indexed storage
 * - Manages multiple game maps with lifecycle support
 * - Executes update systems and rendering
 * - Provides entity querying by component types
 *
 * @ingroup ECS
 */
class Scene {
public:
    using MapEx = AGameMap; /**< Map extension type alias */

private:
    EntityGenerator gen; /**< Entity ID generator */
    std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>>
        components; /**< Component storage by type */
    std::unordered_set<Entity> entities; /**< All entities in the scene */
    std::vector<std::function<void(Scene &, float)>> systems; /**< Global update systems */
    
    std::unordered_map<std::string, std::unique_ptr<AGameMap>> maps; /**< Game maps */
    std::string currentMap; /**< Currently active map name */
    std::string scene_id; /**< Scene identifier */

public:
    /**
     * @brief Default constructor.
     */
    Scene() = default;
    
    /**
     * @brief Constructs a scene with an identifier.
     *
     * @param id The scene's unique identifier.
     */
    Scene(std::string id) : scene_id(std::move(id))
    {
    }

    /**
     * @brief Gets the scene identifier.
     *
     * @return The scene's ID.
     */
    const std::string &id() const
    {
        return scene_id;
    }

    /**
     * @brief Creates a new entity in the scene.
     *
     * @details The entity is automatically added to the current map if one is active.
     *
     * @return The newly created entity.
     */
    Entity createEntity()
    {
        Entity e = gen.create();
        entities.insert(e);
        if (!currentMap.empty() && maps.count(currentMap))
            maps[currentMap]->addEntity(e);
        return e;
    }

    /**
     * @brief Creates an entity using a registered factory.
     *
     * @param factoryName The name of the entity factory to use.
     * @param instanceName The instance name passed to the factory.
     * @return The created entity, or an invalid entity if the factory doesn't exist.
     */
    Entity createEntityFromFactory(const std::string &factoryName, const std::string &instanceName)
    {
        Entity e = createEntity();
        const auto* fn = ECS::getEntityFactory().getFactory(factoryName);
        if (!fn) {
            removeEntity(e);
            std::cerr << "[Scene] Factory '" << factoryName << "' not found\n";
            return Entity();
        }
        (*fn)(*this, instanceName, e);
        return e;
    }

    /**
     * @brief Creates an entity using a factory with post-initialization.
     *
     * @param factoryName The name of the entity factory to use.
     * @param instanceName The instance name passed to the factory.
     * @param postInit Optional callback to run after factory initialization.
     * @return The created entity, or an invalid entity if the factory doesn't exist.
     */
    Entity createEntityFromFactory(const std::string &factoryName, const std::string &instanceName, std::function<void(Entity)> postInit)
    {
        Entity e = createEntity();
        const auto* fn = ECS::getEntityFactory().getFactory(factoryName);
        if (!fn) {
            removeEntity(e);
            std::cerr << "[Scene] Factory '" << factoryName << "' not found\n";
            return Entity();
        }
        (*fn)(*this, instanceName, e);
        if (e && postInit) postInit(e);
        return e;
    }

    /**
     * @brief Removes an entity from the scene.
     *
     * @details Also removes all its components and removes it from all maps.
     *
     * @param e The entity to remove.
     */
    void removeEntity(Entity e)
    {
        entities.erase(e);
        for (auto &p : components)
            p.second->remove(e);
        for (auto &m : maps)
            m.second->removeEntity(e);
    }

    /**
     * @brief Creates a new callback-based map.
     *
     * @param name The map identifier.
     */
    void createMap(const std::string &name)
    {
        maps[name] = std::make_unique<CallbackGameMap>(name);
    }
    
    /**
     * @brief Adds a pre-constructed map to the scene.
     *
     * @param map The map to add (ownership is transferred).
     */
    void addMap(std::unique_ptr<AGameMap> map)
    {
        std::string name = map->getName();
        maps[name] = std::move(map);
    }
    
    /**
     * @brief Lists all available map names.
     *
     * @return Vector of map identifiers.
     */
    std::vector<std::string> listMaps() const
    {
        std::vector<std::string> out;
        out.reserve(maps.size());
        for (auto &p : maps)
            out.push_back(p.first);
        return out;
    }
    
    /**
     * @brief Sets the active map.
     *
     * @details Manages the lifecycle transitions:
     * - Calls onExit/onStop on the old map if needed
     * - Loads the map from a library if not found
     * - Calls onStart/onEnter on the new map
     *
     * @param name The map identifier to activate.
     * @return true if successful, false if the map couldn't be found or loaded.
     */
    bool setCurrentMap(const std::string &name)
    {
        auto it = maps.find(name);
        if (it == maps.end()) {
            if (!try_load_map_library(*this, name))
                return false;
            it = maps.find(name);
            if (it == maps.end())
                return false;
        }
        if (currentMap == name) return true;
        
        if (!currentMap.empty()) {
            auto pit = maps.find(currentMap);
            if (pit != maps.end()) {
                pit->second->onExit(*this);
                if (pit->second->isStarted()) {
                    pit->second->onStop(*this);
                    pit->second->setStarted(false);
                }
            }
        }
        
        currentMap = name;
        if (!it->second->isStarted()) {
            it->second->onStart(*this);
            it->second->setStarted(true);
        }
        it->second->onEnter(*this);
        return true;
    }
    
    /**
     * @brief Gets the name of the currently active map.
     *
     * @return The current map identifier (empty if none).
     */
    const std::string &getCurrentMap() const
    {
        return currentMap;
    }
    
    /**
     * @brief Gets a map by name.
     *
     * @param name The map identifier.
     * @return Pointer to the map, or nullptr if not found.
     */
    AGameMap* getMap(const std::string &name)
    {
        auto it = maps.find(name);
        if (it == maps.end())
            return nullptr;
        return it->second.get();
    }
    
    /**
     * @brief Gets a map by name (const version).
     *
     * @param name The map identifier.
     * @return Const pointer to the map, or nullptr if not found.
     */
    const AGameMap* getMap(const std::string &name) const
    {
        auto it = maps.find(name);
        if (it == maps.end())
            return nullptr;
        return it->second.get();
    }
    
    /**
     * @brief Gets the currently active map.
     *
     * @return Pointer to the current map, or nullptr if none.
     */
    AGameMap* getCurrentMapPtr()
    {
        return getMap(currentMap);
    }
    
    /**
     * @brief Gets the currently active map (const version).
     *
     * @return Const pointer to the current map, or nullptr if none.
     */
    const AGameMap* getCurrentMapPtr() const
    {
        return getMap(currentMap);
    }

    /**
     * @brief Adds a component to an entity.
     *
     * @tparam T The component type.
     * @param e The entity to add the component to.
     * @param comp The component instance.
     */
    template<typename T> void addComponent(Entity e, T comp)
    {
        std::type_index idx = typeid(T);
        if (!components.count(idx))
            components[idx] = std::make_unique<ComponentStorage<T>>();
        auto *store =
            static_cast<ComponentStorage<T> *>(components[idx].get());
        store->data[e] = std::move(comp);
    }

    /**
     * @brief Checks if an entity has a specific component.
     *
     * @tparam T The component type.
     * @param e The entity to check.
     * @return true if the entity has the component, false otherwise.
     */
    template<typename T> bool hasComponent(Entity e) const
    {
        auto it = components.find(std::type_index(typeid(T)));
        if (it == components.end())
            return false;
        auto *store = static_cast<ComponentStorage<T> *>(it->second.get());
        return store->data.find(e) != store->data.end();
    }

    /**
     * @brief Gets a component from an entity.
     *
     * @tparam T The component type.
     * @param e The entity to get the component from.
     * @return Pointer to the component, or nullptr if not found.
     */
    template<typename T> T *getComponent(Entity e)
    {
        auto it = components.find(std::type_index(typeid(T)));
        if (it == components.end())
            return nullptr;
        auto *store = static_cast<ComponentStorage<T> *>(it->second.get());
        auto jt = store->data.find(e);
        if (jt == store->data.end())
            return nullptr;
        return &jt->second;
    }

    /**
     * @brief Removes a component from an entity.
     *
     * @tparam T The component type.
     * @param e The entity to remove the component from.
     */
    template<typename T> void removeComponent(Entity e)
    {
        auto it = components.find(std::type_index(typeid(T)));
        if (it == components.end())
            return;
        it->second->remove(e);
    }

    /**
     * @brief Queries entities that have all specified component types.
     *
     * @tparam Components The component types to filter by.
     * @return Vector of entities having all the specified components.
     */
    template<typename... Components>
    std::vector<Entity> getEntitiesWith() const
    {
        std::vector<Entity> out;
        out.reserve(entities.size());
        for (Entity e : entities) {
            bool ok = true;
            ((void) (ok = ok && hasComponent<Components>(e)), ...);
            if (ok)
                out.push_back(e);
        }
        return out;
    }

    /**
     * @brief Adds a global update system to the scene.
     *
     * @param sys The system function to execute every update.
     */
    void addSystem(std::function<void(Scene &, float)> sys)
    {
        systems.push_back(std::move(sys));
    }

    /**
     * @brief Adds an update system to a specific map.
     *
     * @param mapName The map identifier.
     * @param sys The system function to execute.
     * @return true if successful, false if the map doesn't exist.
     */
    bool addMapSystem(const std::string &mapName,
        std::function<void(Scene &, float)> sys)
    {
        auto it = maps.find(mapName);
        if (it == maps.end())
            return false;
        it->second->addSystem(std::move(sys));
        return true;
    }
    
    /**
     * @brief Sets the enter callback for a map.
     *
     * @param mapName The map identifier.
     * @param cb The callback function.
     * @return true if successful, false if the map doesn't exist.
     */
    bool setMapEnterCallback(const std::string &mapName,
        std::function<void(Scene &)> cb)
    {
        auto it = maps.find(mapName);
        if (it == maps.end())
            return false;
        if (auto* cbMap = dynamic_cast<CallbackGameMap*>(it->second.get()))
            cbMap->setOnEnter(std::move(cb));
        return true;
    }
    
    /**
     * @brief Sets the exit callback for a map.
     *
     * @param mapName The map identifier.
     * @param cb The callback function.
     * @return true if successful, false if the map doesn't exist.
     */
    bool setMapExitCallback(const std::string &mapName,
        std::function<void(Scene &)> cb)
    {
        auto it = maps.find(mapName);
        if (it == maps.end())
            return false;
        if (auto* cbMap = dynamic_cast<CallbackGameMap*>(it->second.get()))
            cbMap->setOnExit(std::move(cb));
        return true;
    }

    /**
     * @brief Sets the start callback for a map.
     *
     * @param mapName The map identifier.
     * @param cb The callback function.
     * @return true if successful, false if the map doesn't exist.
     */
    bool setMapStartCallback(const std::string &mapName,
        std::function<void(Scene &)> cb)
    {
        auto it = maps.find(mapName);
        if (it == maps.end())
            return false;
        if (auto* cbMap = dynamic_cast<CallbackGameMap*>(it->second.get()))
            cbMap->setOnStart(std::move(cb));
        return true;
    }

    /**
     * @brief Sets the tick callback for a map.
     *
     * @param mapName The map identifier.
     * @param cb The callback function.
     * @return true if successful, false if the map doesn't exist.
     */
    bool setMapTickCallback(const std::string &mapName,
        std::function<void(Scene &, float)> cb)
    {
        auto it = maps.find(mapName);
        if (it == maps.end())
            return false;
        if (auto* cbMap = dynamic_cast<CallbackGameMap*>(it->second.get()))
            cbMap->setOnTick(std::move(cb));
        return true;
    }

    /**
     * @brief Sets the stop callback for a map.
     *
     * @param mapName The map identifier.
     * @param cb The callback function.
     * @return true if successful, false if the map doesn't exist.
     */
    bool setMapStopCallback(const std::string &mapName,
        std::function<void(Scene &)> cb)
    {
        auto it = maps.find(mapName);
        if (it == maps.end())
            return false;
        if (auto* cbMap = dynamic_cast<CallbackGameMap*>(it->second.get()))
            cbMap->setOnStop(std::move(cb));
        return true;
    }

    /**
     * @brief Sets the render callback for a map.
     *
     * @param mapName The map identifier.
     * @param cb The callback function.
     * @return true if successful, false if the map doesn't exist.
     */
    bool setMapRenderCallback(const std::string &mapName,
        std::function<void(Scene &)> cb)
    {
        auto it = maps.find(mapName);
        if (it == maps.end())
            return false;
        if (auto* cbMap = dynamic_cast<CallbackGameMap*>(it->second.get()))
            cbMap->setOnRender(std::move(cb));
        return true;
    }

    /**
     * @brief Sets the event callback for a map.
     *
     * @param mapName The map identifier.
     * @param cb The callback function.
     * @return true if successful, false if the map doesn't exist.
     */
    bool setMapEventCallback(const std::string &mapName,
        std::function<bool(Scene &, const MapEvent &)> cb)
    {
        auto it = maps.find(mapName);
        if (it == maps.end())
            return false;
        if (auto* cbMap = dynamic_cast<CallbackGameMap*>(it->second.get()))
            cbMap->setOnEvent(std::move(cb));
        return true;
    }

    /**
     * @brief Dispatches an event to a specific map.
     *
     * @param mapName The map identifier.
     * @param ev The event to dispatch.
     * @return true if the event was handled, false otherwise.
     */
    bool dispatchMapEvent(const std::string &mapName, const MapEvent &ev)
    {
        auto it = maps.find(mapName);
        if (it == maps.end()) return false;
        return it->second->onEvent(*this, ev);
    }

    /**
     * @brief Stores a value in a map's key-value storage.
     *
     * @tparam T The type of value to store.
     * @param mapName The map identifier.
     * @param key The key identifier.
     * @param value The value to store.
     * @return true if successful, false if the map doesn't exist.
     */
    template<typename T>
    bool setMapValue(const std::string &mapName, const std::string &key, T value)
    {
        auto it = maps.find(mapName);
        if (it == maps.end())
            return false;
        it->second->setValue(key, std::move(value));
        return true;
    }

    /**
     * @brief Retrieves a value from a map's storage.
     *
     * @tparam T The expected type of the value.
     * @param mapName The map identifier.
     * @param key The key identifier.
     * @return Pointer to the value, or nullptr if not found or wrong type.
     */
    template<typename T>
    T *getMapValue(const std::string &mapName, const std::string &key)
    {
        auto it = maps.find(mapName);
        if (it == maps.end())
            return nullptr;
        return it->second->getValue<T>(key);
    }

    /**
     * @brief Removes a value from a map's storage.
     *
     * @param mapName The map identifier.
     * @param key The key to remove.
     * @return true if a value was removed, false otherwise.
     */
    bool removeMapValue(const std::string &mapName, const std::string &key)
    {
        auto it = maps.find(mapName);
        if (it == maps.end())
            return false;
        return it->second->removeValue(key);
    }

    /**
     * @brief Updates all systems and the current map.
     *
     * @details Executes all global systems, then calls onTick on the current map.
     *
     * @param dt Delta time since last update in seconds.
     */
    void update(float dt)
    {
        for (auto &s : systems)
            s(*this, dt);
        if (!currentMap.empty()) {
            auto it = maps.find(currentMap);
            if (it != maps.end()) {
                it->second->onTick(*this, dt);
            }
        }
    }

    /**
     * @brief Renders the current map.
     *
     * @details Calls onRender on the currently active map.
     */
    void render()
    {
        if (!currentMap.empty()) {
            auto it = maps.find(currentMap);
            if (it != maps.end())
                it->second->onRender(*this);
        }
    }

    /**
     * @brief Adds an entity to a specific map.
     *
     * @param mapName The map identifier.
     * @param e The entity to add.
     * @return true if successful, false if the map doesn't exist.
     */
    bool addEntityToMap(const std::string &mapName, Entity e)
    {
        auto it = maps.find(mapName);
        if (it == maps.end())
            return false;
        it->second->addEntity(e);
        return true;
    }

    /**
     * @brief Gets all entities in the scene.
     *
     * @return Const reference to the entity set.
     */
    const std::unordered_set<Entity> &allEntities() const
    {
        return entities;
    }

    /**
     * @brief Lists all entities in a specific map.
     *
     * @param mapName The map identifier.
     * @return Vector of entities in the map (empty if map doesn't exist).
     */
    std::vector<Entity> listEntitiesInMap(const std::string &mapName) const
    {
        std::vector<Entity> out;
        auto it = maps.find(mapName);
        if (it == maps.end()) return out;
        const auto& ents = it->second->getEntities();
        out.reserve(ents.size());
        for (auto e : ents) out.push_back(e);
        return out;
    }

    /**
     * @brief Gets component counts by type.
     *
     * @return Map of component type names to their instance counts.
     */
    std::unordered_map<std::string, size_t> componentCounts() const
    {
        std::unordered_map<std::string, size_t> out;
        for (auto &p : components) {
            std::string tn = p.first.name();
            out[tn] = p.second->size();
        }
        return out;
    }

    /**
     * @brief Dumps scene information to an output stream.
     *
     * @details Outputs scene ID, current map, entity counts, map details, and component statistics.
     *
     * @param os The output stream to write to.
     */
    void dumpInfo(std::ostream &os) const
    {
        os << "Scene '" << scene_id << "'\n";
        os << "  Current map: '" << currentMap << "'\n";
        os << "  Total entities: " << entities.size() << "\n";
        os << "  Maps:\n";
        for (auto &m : maps) {
            os << "    - " << m.first << " (entities=" << m.second->getEntities().size() << ")\n";
        }
        os << "  Components:\n";
        for (auto &c : componentCounts()) {
            os << "    - " << c.first << " : " << c.second << "\n";
        }
    }
};

}  // namespace ECS

#endif // SCENE_HPP
