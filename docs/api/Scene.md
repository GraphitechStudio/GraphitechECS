# Scene Class Reference

The `Scene` class is the core container of the GraphitechECS architecture. It manages entities, components, and game maps, providing the main interface for game logic implementation.

## Navigation

### Documentation Files
- [**Home**](../README.md) - Documentation overview
- [**API Reference**](../API.md) - Complete API overview
- [**RFC Document**](../ECS_RFC.md) - Architecture specification
- [**Contributing Guide**](../CONTRIBUTING.md) - Development guidelines

### Core Classes
- [**Scene**](../api/Scene.md) - This Scene documentation
- [**EntityFactory**](../api/EntityFactory.md) - Entity creation factory

### Loading & Serialization
- [**GMapLoader**](../api/GMapLoader.md) - Map file loader
- [**MapLoader**](../api/MapLoader.md) - Dynamic library loader

### Registry Systems
- [**ComponentRegistry**](../api/ComponentRegistry.md) - Component parser registry

### Components
- [**Built-in Components**](../api/Components.md) - Pre-implemented components

---

## Table of Contents
- [Entity Management](#entity-management)
- [Component Management](#component-management)
- [Querying](#querying)
- [System Management](#system-management)
- [Map Management](#map-management)
- [Map Callbacks](#map-callbacks)
- [Map Values](#map-values)
- [Entity-Map Association](#entity-map-association)
- [Update and Render](#update-and-render)
- [Utility](#utility)

---

## Entity Management

### `ECS::Entity createEntity()`
Creates a new entity and returns its ID.

**Returns:** New entity ID

**Example:**
```cpp
ECS::Scene scene("MyScene");
ECS::Entity player = scene.createEntity();
```

### `void destroyEntity(ECS::Entity entity)`
Removes an entity and all its components from the scene.

**Parameters:**
- `entity`: The entity to destroy

**Example:**
```cpp
scene.destroyEntity(player);
```

### `bool isEntityValid(ECS::Entity entity) const`
Checks if an entity exists in the scene.

**Parameters:**
- `entity`: The entity to check

**Returns:** true if entity exists, false otherwise

**Example:**
```cpp
if (scene.isEntityValid(player)) {
    // Entity exists
}
```

---

## Component Management

### `template<typename T> void addComponent(ECS::Entity entity, T&& component)`
Adds a component to an entity.

**Template Parameters:**
- `T`: Component type

**Parameters:**
- `entity`: The entity to add the component to
- `component`: The component instance

**Example:**
```cpp
scene.addComponent(player, ECS::PositionComponent{10.0f, 20.0f});
```

### `template<typename T> T* getComponent(ECS::Entity entity)`
Gets a component from an entity.

**Template Parameters:**
- `T`: Component type

**Parameters:**
- `entity`: The entity to get the component from

**Returns:** Pointer to component, or nullptr if not found

**Example:**
```cpp
auto* pos = scene.getComponent<ECS::PositionComponent>(player);
if (pos) {
    pos->x += 5.0f;
}
```

### `template<typename T> bool hasComponent(ECS::Entity entity) const`
Checks if an entity has a specific component.

**Template Parameters:**
- `T`: Component type

**Parameters:**
- `entity`: The entity to check

**Returns:** true if component exists, false otherwise

**Example:**
```cpp
if (scene.hasComponent<ECS::VelocityComponent>(player)) {
    // Has velocity
}
```

### `template<typename T> void removeComponent(ECS::Entity entity)`
Removes a component from an entity.

**Template Parameters:**
- `T`: Component type

**Parameters:**
- `entity`: The entity to remove the component from

**Example:**
```cpp
scene.removeComponent<ECS::VelocityComponent>(player);
```

---

## Querying

### `template<typename... Components> std::vector<ECS::Entity> getEntitiesWith() const`
Gets all entities that have all specified component types.

**Template Parameters:**
- `Components`: Component types to filter by

**Returns:** Vector of entities with all specified components

**Example:**
```cpp
auto movingEntities = scene.getEntitiesWith<ECS::PositionComponent, ECS::VelocityComponent>();
for (auto entity : movingEntities) {
    // Process moving entities
}
```

### `const std::unordered_set<ECS::Entity>& allEntities() const`
Gets all entities in the scene.

**Returns:** Const reference to set of all entities

**Example:**
```cpp
for (auto entity : scene.allEntities()) {
    // Process all entities
}
```

---

## System Management

### `void addSystem(std::function<void(Scene&, float)> system)`
Adds a global update system to the scene.

**Parameters:**
- `system`: Function taking scene and delta time

**Example:**
```cpp
scene.addSystem([](ECS::Scene& s, float dt) {
    // Update logic
});
```

### `bool addMapSystem(const std::string& mapName, std::function<void(Scene&, float)> system)`
Adds an update system to a specific map.

**Parameters:**
- `mapName`: The map identifier
- `system`: Function taking scene and delta time

**Returns:** true if successful, false if map doesn't exist

**Example:**
```cpp
scene.addMapSystem("Gameplay", physicsSystem);
```

---

## Map Management

### `void createMap(const std::string& name)`
Creates a new callback-based map.

**Parameters:**
- `name`: The map identifier

**Example:**
```cpp
scene.createMap("MainMenu");
```

### `void addMap(std::unique_ptr<ECS::AGameMap> map)`
Adds a pre-constructed map to the scene.

**Parameters:**
- `map`: The map to add (ownership transferred)

**Example:**
```cpp
auto customMap = std::make_unique<MyGameMap>("Custom");
scene.addMap(std::move(customMap));
```

### `std::vector<std::string> listMaps() const`
Lists all available map names.

**Returns:** Vector of map identifiers

**Example:**
```cpp
auto maps = scene.listMaps();
for (const auto& mapName : maps) {
    std::cout << "Map: " << mapName << std::endl;
}
```

### `bool setCurrentMap(const std::string& name)`
Sets the active map.

**Parameters:**
- `name`: The map identifier to activate

**Returns:** true if successful, false if map not found

**Example:**
```cpp
if (scene.setCurrentMap("Gameplay")) {
    // Map activated
}
```

### `const std::string& getCurrentMap() const`
Gets the name of the currently active map.

**Returns:** Current map identifier (empty if none)

**Example:**
```cpp
std::string current = scene.getCurrentMap();
```

### `ECS::AGameMap* getMap(const std::string& name)`
Gets a map by name.

**Parameters:**
- `name`: The map identifier

**Returns:** Pointer to map, or nullptr if not found

**Example:**
```cpp
auto* map = scene.getMap("Gameplay");
if (map) {
    // Use map
}
```

---

## Map Callbacks

### `bool setMapEnterCallback(const std::string& mapName, std::function<void(Scene&)> callback)`
Sets the enter callback for a map.

**Parameters:**
- `mapName`: The map identifier
- `callback`: Function called when entering the map

**Returns:** true if successful, false if map doesn't exist

**Example:**
```cpp
scene.setMapEnterCallback("Gameplay", [](ECS::Scene& s) {
    // Initialize gameplay
});
```

### `bool setMapExitCallback(const std::string& mapName, std::function<void(Scene&)> callback)`
Sets the exit callback for a map.

**Parameters:**
- `mapName`: The map identifier
- `callback`: Function called when exiting the map

**Returns:** true if successful, false if map doesn't exist

**Example:**
```cpp
scene.setMapExitCallback("Gameplay", [](ECS::Scene& s) {
    // Cleanup gameplay
});
```

### `bool setMapStartCallback(const std::string& mapName, std::function<void(Scene&)> callback)`
Sets the start callback for a map.

**Parameters:**
- `mapName`: The map identifier
- `callback`: Function called when starting the map

**Returns:** true if successful, false if map doesn't exist

**Example:**
```cpp
scene.setMapStartCallback("Gameplay", [](ECS::Scene& s) {
    // Load resources
});
```

### `bool setMapStopCallback(const std::string& mapName, std::function<void(Scene&)> callback)`
Sets the stop callback for a map.

**Parameters:**
- `mapName`: The map identifier
- `callback`: Function called when stopping the map

**Returns:** true if successful, false if map doesn't exist

**Example:**
```cpp
scene.setMapStopCallback("Gameplay", [](ECS::Scene& s) {
    // Unload resources
});
```

### `bool setMapTickCallback(const std::string& mapName, std::function<void(Scene&, float)> callback)`
Sets the tick callback for a map.

**Parameters:**
- `mapName`: The map identifier
- `callback`: Function called every frame

**Returns:** true if successful, false if map doesn't exist

**Example:**
```cpp
scene.setMapTickCallback("Gameplay", [](ECS::Scene& s, float dt) {
    // Update game logic
});
```

### `bool setMapRenderCallback(const std::string& mapName, std::function<void(Scene&)> callback)`
Sets the render callback for a map.

**Parameters:**
- `mapName`: The map identifier
- `callback`: Function called for rendering

**Returns:** true if successful, false if map doesn't exist

**Example:**
```cpp
scene.setMapRenderCallback("Gameplay", [](ECS::Scene& s) {
    // Render game
});
```

### `bool setMapEventCallback(const std::string& mapName, std::function<bool(Scene&, const MapEvent&)> callback)`
Sets the event callback for a map.

**Parameters:**
- `mapName`: The map identifier
- `callback`: Function called for events

**Returns:** true if successful, false if map doesn't exist

**Example:**
```cpp
scene.setMapEventCallback("Gameplay", [](ECS::Scene& s, const MapEvent& e) {
    // Handle event
    return true;
});
```

---

## Map Values

### `template<typename T> bool setMapValue(const std::string& mapName, const std::string& key, T value)`
Stores a value in a map's key-value storage.

**Template Parameters:**
- `T`: Value type

**Parameters:**
- `mapName`: The map identifier
- `key`: The key identifier
- `value`: The value to store

**Returns:** true if successful, false if map doesn't exist

**Example:**
```cpp
scene.setMapValue("Gameplay", "score", 1000);
```

### `template<typename T> T* getMapValue(const std::string& mapName, const std::string& key)`
Retrieves a value from a map's storage.

**Template Parameters:**
- `T`: Expected value type

**Parameters:**
- `mapName`: The map identifier
- `key`: The key identifier

**Returns:** Pointer to value, or nullptr if not found/wrong type

**Example:**
```cpp
int* score = scene.getMapValue<int>("Gameplay", "score");
if (score) {
    std::cout << "Score: " << *score << std::endl;
}
```

### `bool removeMapValue(const std::string& mapName, const std::string& key)`
Removes a value from a map's storage.

**Parameters:**
- `mapName`: The map identifier
- `key`: The key to remove

**Returns:** true if value was removed, false otherwise

**Example:**
```cpp
scene.removeMapValue("Gameplay", "score");
```

---

## Entity-Map Association

### `bool addEntityToMap(const std::string& mapName, ECS::Entity entity)`
Adds an entity to a specific map.

**Parameters:**
- `mapName`: The map identifier
- `entity`: The entity to add

**Returns:** true if successful, false if map doesn't exist

**Example:**
```cpp
scene.addEntityToMap("Gameplay", player);
```

### `std::vector<ECS::Entity> listEntitiesInMap(const std::string& mapName) const`
Lists all entities in a specific map.

**Parameters:**
- `mapName`: The map identifier

**Returns:** Vector of entities in the map (empty if map doesn't exist)

**Example:**
```cpp
auto entities = scene.listEntitiesInMap("Gameplay");
```

---

## Update and Render

### `void update(float deltaTime)`
Updates all systems and the current map.

**Parameters:**
- `deltaTime`: Time elapsed since last update in seconds

**Example:**
```cpp
scene.update(0.016f); // 60 FPS
```

### `void render()`
Renders the current map.

**Example:**
```cpp
scene.render();
```

---

## Utility

### `std::unordered_map<std::string, size_t> componentCounts() const`
Gets component counts by type.

**Returns:** Map of component type names to instance counts

**Example:**
```cpp
auto counts = scene.componentCounts();
for (const auto& [type, count] : counts) {
    std::cout << type << ": " << count << std::endl;
}
```

### `void dumpInfo(std::ostream& os) const`
Dumps scene information to an output stream.

**Parameters:**
- `os`: The output stream to write to

**Example:**
```cpp
scene.dumpInfo(std::cout);
```

### `ECS::Entity createEntityFromFactory(const std::string& factoryName, const std::string& entityName)`
Creates an entity using a registered factory.

**Parameters:**
- `factoryName`: The factory identifier
- `entityName`: The entity name

**Returns:** Created entity, or invalid entity if factory not found

**Example:**
```cpp
ECS::Entity player = scene.createEntityFromFactory("Player", "Hero");
```

### `bool switchToMap(const std::string& mapName)`
Alias for setCurrentMap.

**Parameters:**
- `mapName`: The map identifier

**Returns:** true if successful

**Example:**
```cpp
scene.switchToMap("MainMenu");
```

[⬆️ Back to API Index](../API.md)