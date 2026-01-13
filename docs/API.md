# GraphitechECS API Reference

This document provides a comprehensive reference for the GraphitechECS library functions, organized by module.

## 📑 Navigation

### 📚 Documentation Files
- [**🏠 Home**](../README.md) - Documentation overview
- [**📖 API Reference**](../API.md) - This overview page
- [**📋 RFC Document**](../ECS_RFC.md) - Architecture specification
- [**🤝 Contributing Guide**](../CONTRIBUTING.md) - Development guidelines

### 🔧 Core Classes
- [**Scene**](./api/Scene.md) - Main ECS container
- [**EntityFactory**](./api/EntityFactory.md) - Entity creation factory

### 🏭 Loading & Serialization
- [**GMapLoader**](./api/GMapLoader.md) - Map file loader
- [**MapLoader**](./api/MapLoader.md) - Dynamic library loader

### 📋 Registry Systems
- [**ComponentRegistry**](./api/ComponentRegistry.md) - Component parser registry

### 🧩 Components
- [**Built-in Components**](./api/Components.md) - Pre-implemented components

---

## Table of Contents

### Core Classes
- [**Scene**](./api/Scene.md) - Main ECS container managing entities, components, and maps
- [**EntityFactory**](./api/EntityFactory.md) - Factory system for entity creation

### Loading & Serialization
- [**GMapLoader**](./api/GMapLoader.md) - JSON and binary map file loader
- [**MapLoader**](./api/MapLoader.md) - Dynamic map loading from libraries

### Registry Systems
- [**ComponentRegistry**](./api/ComponentRegistry.md) - Global component parser registry

### Components
- [**Built-in Components**](./api/Components.md) - Pre-implemented components

---

## Core Classes

### Scene Class
The `Scene` class is the core container of the GraphitechECS architecture.

**Key Features:**
- Entity lifecycle management (create, destroy, validate)
- Component storage and retrieval by type
- Multi-map support with lifecycle callbacks
- System execution (global and map-specific)
- Entity querying by component combinations

**Quick Example:**
```cpp
ECS::Scene scene("MyGame");
ECS::Entity player = scene.createEntity();
scene.addComponent(player, ECS::PositionComponent{100, 100});
scene.addSystem([](ECS::Scene& s, float dt) { /* logic */ });
```

**[📖 Full Scene Documentation](./api/Scene.md)**

### EntityFactory Class
Factory system for creating entities with predefined configurations.

**Key Features:**
- Named factory registration
- Entity template instantiation
- Global factory registry
- Reusable entity creation logic

**Quick Example:**
```cpp
ECS::getEntityFactory().registerFactory("Player",
    [](ECS::Scene& s, const std::string& name, ECS::Entity e) {
        s.addComponent(e, ECS::PositionComponent{0, 0});
    });
ECS::Entity player = s.createEntityFromFactory("Player", "Hero");
```

**[Full EntityFactory Documentation](./api/EntityFactory.md)**

---

## Loading & Serialization

### GMapLoader Class
Loads .gmap (JSON) and .gmapb (binary) files for scene initialization.

**Key Features:**
- Dual format support (JSON + Binary)
- Component parser system
- Prefab entity templates
- Automatic format detection

**Quick Example:**
```cpp
ECS::GMapLoader loader;
bool success = loader.loadMap(scene, "Level1", "maps/level1");
```

**[Full GMapLoader Documentation](./api/GMapLoader.md)**

### Map Loading System
Dynamic map loading from shared libraries and factory registration.

**Key Features:**
- Runtime library loading
- Map factory registration
- Cross-platform support
- Memory-efficient loading

**Quick Example:**
```cpp
ECS::register_map_factory("MainMenu", [](ECS::Scene& s) {
    // Create menu...
});
ECS::tryCreateMapFromFactory(scene, "MainMenu");
```

**[Full MapLoader Documentation](./api/MapLoader.md)**

---

## Registry Systems

### ComponentRegistry Class
Global registry for component parsers used by map loading.

**Key Features:**
- Singleton access pattern
- Parser registration for custom components
- Integration with GMapLoader
- Type-safe component parsing

**Quick Example:**
```cpp
ECS::ComponentRegistry::get().registerParser("MyComponent",
    [](ECS::Scene& s, ECS::Entity e, const nlohmann::json& data) {
        // Parse and add component
    });
```

**[Full ComponentRegistry Documentation](./api/ComponentRegistry.md)**

---

## Components

### Built-in Components
Pre-implemented components for common game development needs.

**Available Components:**
- `PositionComponent` - 2D position
- `VelocityComponent` - Linear velocity
- `ScaleComponent` - Rendering scale
- `RotationComponent` - Rotation angle
- `TextureComponent` - Texture rendering
- `RenderCircle` - Debug circle rendering
- `RenderLine` - Debug line rendering
- `TTLComponent` - Time-to-live
- `CollisionComponent` - Collision detection

**Quick Example:**
```cpp
scene.addComponent(entity, ECS::PositionComponent{100, 200});
scene.addComponent(entity, ECS::VelocityComponent{5, 0});
```

**[Full Components Documentation](./api/Components.md)**

---

## Quick Start Guide

### Basic ECS Setup
```cpp
#include <Graphitech/ECS/ECS.hpp>

// 1. Create scene
ECS::Scene scene("MyGame");

// 2. Create entities
ECS::Entity player = scene.createEntity();
ECS::Entity enemy = scene.createEntity();

// 3. Add components
scene.addComponent(player, ECS::PositionComponent{100, 100});
scene.addComponent(player, ECS::VelocityComponent{0, 0});
scene.addComponent(enemy, ECS::PositionComponent{200, 200});

// 4. Add systems
scene.addSystem([](ECS::Scene& s, float dt) {
    // Movement system
    auto entities = s.getEntitiesWith<ECS::PositionComponent, ECS::VelocityComponent>();
    for (auto entity : entities) {
        auto* pos = s.getComponent<ECS::PositionComponent>(entity);
        auto* vel = s.getComponent<ECS::VelocityComponent>(entity);
        pos->x += vel->vx * dt;
        pos->y += vel->vy * dt;
    }
});

// 5. Main loop
while (running) {
    scene.update(0.016f); // 60 FPS
    scene.render();
}
```

### Map Loading
```cpp
// Load from file
ECS::GMapLoader loader;
loader.loadMap(scene, "Level1", "maps/level1");
scene.setCurrentMap("Level1");

// Or create programmatically
ECS::register_map_factory("Gameplay", [](ECS::Scene& s) {
    s.createMap("Gameplay");
    // Add gameplay entities...
});
ECS::tryCreateMapFromFactory(scene, "Gameplay");
scene.setCurrentMap("Gameplay");
```

### Custom Components
```cpp
// Define component
class HealthComponent : public ECS::AComponent {
public:
    int current = 100;
    int max = 100;
};

// Register for map loading
ECS::ComponentRegistry::get().registerParser("HealthComponent",
    [](ECS::Scene& s, ECS::Entity e, const nlohmann::json& data) {
        HealthComponent comp;
        comp.current = data.value("current", 100);
        comp.max = data.value("max", 100);
        s.addComponent(e, comp);
    });

// Use in code
scene.addComponent(player, HealthComponent{100, 100});
```

---

## Additional Resources

- [**API Documentation Index**](./README.md) - Overview of all documentation files
- [**RFC Documentation**](./ECS_RFC.md) - Architecture design and rationale
- [**Contributing Guide**](./CONTRIBUTING.md) - How to contribute to the project

---

*For detailed function signatures, parameters, and examples, please refer to the individual module documentation linked above.*