# Map Loading System Reference

The map loading system provides functionality for loading maps from shared libraries and registering map factories. It supports both dynamic library loading and programmatic map creation.

## Navigation

### Documentation Files
- [**Home**](../README.md) - Documentation overview
- [**API Reference**](../API.md) - Complete API overview
- [**RFC Document**](../ECS_RFC.md) - Architecture specification
- [**Contributing Guide**](../CONTRIBUTING.md) - Development guidelines

### Core Classes
- [**Scene**](../api/Scene.md) - Scene container class
- [**EntityFactory**](../api/EntityFactory.md) - Entity creation factory

### Loading & Serialization
- [**GMapLoader**](../api/GMapLoader.md) - Map file loader
- [**MapLoader**](../api/MapLoader.md) - This MapLoader documentation

### Registry Systems
- [**ComponentRegistry**](../api/ComponentRegistry.md) - Component parser registry

### Components
- [**Built-in Components**](../api/Components.md) - Pre-implemented components

---

## Table of Contents
- [Map Factory Functions](#map-factory-functions)
- [Library Loading](#library-loading)
- [Map Creation](#map-creation)

---

## Map Factory Functions

### `void register_map_factory(const std::string& name, MapFactory factory)`
Registers a map factory function with a name.

**Parameters:**
- `name`: The unique identifier for this map
- `factory`: The factory function that initializes the map

**Example:**
```cpp
ECS::register_map_factory("MainMenu",
    [](ECS::Scene& scene) {
        // Create menu entities
        ECS::Entity title = scene.createEntity();
        scene.addComponent(title, ECS::PositionComponent{400, 100});
        // Add UI components...
    });
```

### `bool tryCreateMapFromFactory(ECS::Scene& scene, const std::string& mapName)`
Creates a map using a registered factory.

**Parameters:**
- `scene`: The scene to create in
- `mapName`: The map identifier

**Returns:** true if created successfully, false otherwise

**Example:**
```cpp
if (ECS::tryCreateMapFromFactory(scene, "MainMenu")) {
    std::cout << "Menu map created!" << std::endl;
}
```

---

## Library Loading

### `bool try_load_map_library(ECS::Scene& scene, const std::string& name)`
Attempts to load a map from a shared library (.so file).

**Parameters:**
- `scene`: The scene to initialize with the loaded map
- `name`: The name of the map to load

**Returns:** true if the map was successfully loaded, false otherwise

**Search Paths (Unix):**
1. `build/maps/{name}/libmap_{name}.so`
2. `./build/maps/{name}/libmap_{name}.so`
3. `src/maps/{name}/libmap_{name}.so`
4. `maps/{name}/libmap_{name}.so`
5. `build/src/maps/{name}/libmap_{name}.so`
6. `./build/src/maps/{name}/libmap_{name}.so`
7. `{cwd}/build/maps/{name}/libmap_{name}.so`
8. `{cwd}/src/maps/{name}/libmap_{name}.so`
9. `{cwd}/build/src/maps/{name}/libmap_{name}.so`

**Example:**
```cpp
if (ECS::try_load_map_library(scene, "Gameplay")) {
    std::cout << "Map loaded from library!" << std::endl;
} else {
    std::cout << "Failed to load map library" << std::endl;
}
```

---

## Map Creation

### `std::unordered_map<std::string, MapFactory>& get_map_factories()`
Gets the global registry of map factories.

**Returns:** Reference to the global map factory registry

**Example:**
```cpp
auto& factories = ECS::get_map_factories();
for (const auto& [name, factory] : factories) {
    std::cout << "Available map: " << name << std::endl;
}
```

---

## Type Definitions

### `using MapFactory = std::function<void(Scene&)>`
Type alias for map factory functions.

Map factory functions receive:
- `Scene&`: Reference to the scene to initialize

**Example:**
```cpp
ECS::MapFactory createGameplayMap = [](ECS::Scene& scene) {
    // Initialize gameplay map
    scene.createMap("Gameplay");

    // Create player entity
    ECS::Entity player = scene.createEntity();
    scene.addComponent(player, ECS::PositionComponent{100, 100});
    scene.addComponent(player, ECS::VelocityComponent{0, 0});

    // Add to gameplay map
    scene.addEntityToMap("Gameplay", player);

    // Add systems
    scene.addMapSystem("Gameplay", [](ECS::Scene& s, float dt) {
        // Physics system
    });
};
```

---

## Library Format

### Shared Library Structure
Map libraries must export either:
- `register_map_{name}` function, or
- `register_map` function

**Example library implementation:**
```cpp
// maps/Gameplay/libmap_Gameplay.cpp
#include <Graphitech/ECS/ECS.hpp>

extern "C" void register_map_Gameplay(ECS::Scene& scene) {
    // Initialize gameplay map
    scene.createMap("Gameplay");

    // Create entities, add components, systems...
}

extern "C" void register_map(ECS::Scene& scene) {
    // Alternative entry point
    register_map_Gameplay(scene);
}
```

### Compilation
```bash
# Compile the library
g++ -shared -fPIC -o build/maps/Gameplay/libmap_Gameplay.so \
    maps/Gameplay/libmap_Gameplay.cpp \
    -Iinclude -Lbuild -lGraphitechECS
```

---

## Usage Patterns

### Programmatic Map Creation
```cpp
// Register map at startup
ECS::register_map_factory("Level1", [](ECS::Scene& scene) {
    scene.createMap("Level1");

    // Create terrain
    for (int x = 0; x < 100; ++x) {
        for (int y = 0; y < 100; ++y) {
            ECS::Entity tile = scene.createEntity();
            scene.addComponent(tile, ECS::PositionComponent{x * 32, y * 32});
            scene.addEntityToMap("Level1", tile);
        }
    }
});

// Load the map
ECS::tryCreateMapFromFactory(scene, "Level1");
scene.setCurrentMap("Level1");
```

### Dynamic Library Loading
```cpp
// Load map from external library
if (ECS::try_load_map_library(scene, "BossArena")) {
    scene.setCurrentMap("BossArena");
    std::cout << "Boss arena loaded!" << std::endl;
} else {
    std::cerr << "Failed to load boss arena" << std::endl;
}
```

### Map Switching
```cpp
// Switch between maps
scene.setCurrentMap("MainMenu");
// ... menu logic ...
scene.setCurrentMap("Gameplay");
// ... gameplay logic ...
```

---

## Error Handling

- **Missing libraries**: If no library is found, the function returns false
- **Missing symbols**: If the required function is not exported, loading fails
- **Factory not found**: If a factory name doesn't exist, creation fails
- **Library conflicts**: Multiple libraries with the same name may cause issues

**Example error messages:**
```
[MapLoader] candidate exists: build/maps/Gameplay/libmap_Gameplay.so
[MapLoader] getSymbol failed to find 'register_map_Gameplay'
[MapLoader] Failed to load map 'Gameplay'
```

---

## Performance Considerations

- **Library loading**: Shared libraries are loaded once and kept in memory
- **Factory registration**: Factories are stored in a global registry for fast access
- **Map switching**: Maps maintain their state when not active
- **Memory management**: Libraries are unloaded when the application exits

[⬆️ Back to API Index](../API.md)