# GMapLoader Class Reference

The `GMapLoader` class loads .gmap (JSON) and .gmapb (binary) files for scene initialization. It supports both human-readable JSON format and optimized binary format for performance.

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
- [**GMapLoader**](../api/GMapLoader.md) - This GMapLoader documentation
- [**MapLoader**](../api/MapLoader.md) - Dynamic library loader

### Registry Systems
- [**ComponentRegistry**](../api/ComponentRegistry.md) - Component parser registry

### Components
- [**Built-in Components**](../api/Components.md) - Pre-implemented components

---

## Table of Contents
- [Construction](#construction)
- [Parser Registration](#parser-registration)
- [Map Loading](#map-loading)
- [Prefab Application](#prefab-application)

---

## Construction

### `GMapLoader()`
Constructs a GMapLoader and registers default parsers.

**Example:**
```cpp
ECS::GMapLoader loader;
```

---

## Parser Registration

### `void registerParser(const std::string& componentType, ComponentParser parser)`
Registers a component parser for custom components.

**Parameters:**
- `componentType`: The component type name (e.g., "PositionComponent")
- `parser`: The parser function

**Example:**
```cpp
loader.registerParser("MyComponent",
    [](ECS::Scene& s, ECS::Entity e, const nlohmann::json& data) {
        ECS::MyComponent comp;
        if (data.contains("value")) {
            comp.value = data["value"].get<int>();
        }
        s.addComponent(e, comp);
    });
```

---

## Map Loading

### `bool loadMap(ECS::Scene& scene, const std::string& mapName, const std::string& filepath)`
Loads a .gmap or .gmapb file and populates the scene.

**Parameters:**
- `scene`: The scene to load into
- `mapName`: The map name in the scene
- `filepath`: Path to the .gmap/.gmapb file (without extension)

**Returns:** true if loaded successfully, false otherwise

**Details:**
Automatically detects format:
1. Tries .gmapb (binary - fast)
2. Falls back to .gmap (JSON)

**Example:**
```cpp
bool success = loader.loadMap(scene, "Level1", "maps/level1");
if (success) {
    std::cout << "Map loaded successfully!" << std::endl;
}
```

---

## Prefab Application

### `bool applyPrefabToEntity(ECS::Scene& scene, ECS::Entity entity, const std::string& prefabName)`
Applies a prefab to an existing entity.

**Parameters:**
- `scene`: The scene containing the entity
- `entity`: The entity to apply the prefab to
- `prefabName`: The name of the prefab to apply

**Returns:** true if prefab was found and applied, false otherwise

**Example:**
```cpp
loader.applyPrefabToEntity(scene, player, "PlayerPrefab");
```

---

## File Formats

### JSON Format (.gmap)
```json
{
  "prefabs": {
    "Player": {
      "PositionComponent": {"x": 0, "y": 0},
      "VelocityComponent": {"vx": 0, "vy": 0}
    }
  },
  "entities": [
    {
      "name": "player1",
      "prefab": "Player",
      "PositionComponent": {"x": 100, "y": 200}
    }
  ]
}
```

### Binary Format (.gmapb)
- Magic number: `0x50414D47` ("GMAP")
- Version: 1.0
- Optimized for fast loading
- 10x faster than JSON
- 75% smaller file size

---

## Component Parsers

### Built-in Parsers
The loader automatically registers parsers for built-in components:
- `PositionComponent`
- `VelocityComponent`
- `ScaleComponent`
- `RotationComponent`
- `RenderCircle`
- `RenderLine`
- `TTLComponent`
- `CollisionComponent`

### Custom Parser Example
```cpp
loader.registerParser("HealthComponent",
    [](ECS::Scene& s, ECS::Entity e, const nlohmann::json& data) {
        ECS::HealthComponent health;
        health.current = data.value("current", 100);
        health.maximum = data.value("maximum", 100);
        s.addComponent(e, health);
    });
```

---

## Error Handling

The loader provides detailed error messages for:
- Missing files
- Invalid JSON format
- Corrupted binary files
- Unknown component types
- Missing required prefab references

**Example error output:**
```
[MapLoader] Failed to load map 'Level1': File not found
[MapLoader] Component parser not found for 'UnknownComponent'
```

---

## Performance Notes

- **Binary format (.gmapb)**: Recommended for production builds
- **JSON format (.gmap)**: Recommended for development and debugging
- **Prefab caching**: Prefabs are cached after first load for faster subsequent access
- **Component validation**: Invalid component data is logged but doesn't stop loading

[⬆️ Back to API Index](../API.md)