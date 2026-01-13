# GraphitechECS API Documentation

Welcome to the GraphitechECS API documentation. This documentation is organized by module for easier navigation.

## Navigation

### Documentation Files
- [**Home**](./README.md) - This overview page
- [**API Reference**](./API.md) - Complete API overview
- [**RFC Document**](./ECS_RFC.md) - Architecture specification
- [**Contributing Guide**](./CONTRIBUTING.md) - Development guidelines

### Core Classes
- [**Scene**](./api/Scene.md) - Main ECS container
- [**EntityFactory**](./api/EntityFactory.md) - Entity creation factory

### Loading & Serialization
- [**GMapLoader**](./api/GMapLoader.md) - Map file loader
- [**MapLoader**](./api/MapLoader.md) - Dynamic library loader

### Registry Systems
- [**ComponentRegistry**](./api/ComponentRegistry.md) - Component parser registry

### Components
- [**Built-in Components**](./api/Components.md) - Pre-implemented components

---

## Documentation Structure

Each module has its own dedicated documentation file containing:
- Complete function reference with signatures
- Parameter descriptions and return values
- Usage examples and code snippets
- Best practices and performance notes

## API Reference by Module

### Core Classes

#### [**Scene**](./api/Scene.md)
The main ECS container managing entities, components, and maps.
- Entity management (create, destroy, validate)
- Component management (add, get, remove, query)
- System management (global and map-specific)
- Map management (create, switch, callbacks)
- Update/render loop integration

#### [**EntityFactory**](./api/EntityFactory.md)
Factory system for creating entities with predefined configurations.
- Factory registration and management
- Entity creation from templates
- Global factory registry access

### Loading & Serialization

#### [**GMapLoader**](./api/GMapLoader.md)
JSON and binary map file loader.
- .gmap (JSON) and .gmapb (binary) format support
- Component parser registration
- Prefab system for entity templates
- Automatic format detection

#### [**MapLoader**](./api/MapLoader.md)
Dynamic map loading from shared libraries.
- Runtime library loading
- Map factory registration
- Cross-platform library search paths

### Registry Systems

#### [**ComponentRegistry**](./api/ComponentRegistry.md)
Global registry for component parsers.
- Parser registration for custom components
- Singleton access pattern
- Integration with GMapLoader

### Components

#### [**Built-in Components**](./api/Components.md)
Pre-implemented components for common use cases.
- Position, Velocity, Scale, Rotation
- Render primitives (Circle, Line)
- Game logic (TTL, Collision)
- Custom component creation guide

## Quick Start

### Basic Scene Setup
```cpp
#include <Graphitech/ECS/ECS.hpp>

// Create scene
ECS::Scene scene("MyGame");

// Create entity
ECS::Entity player = scene.createEntity();

// Add components
scene.addComponent(player, ECS::PositionComponent{100, 100});
scene.addComponent(player, ECS::VelocityComponent{0, 0});

// Add update system
scene.addSystem([](ECS::Scene& s, float dt) {
    // Game logic here
});

// Main loop
while (running) {
    scene.update(0.016f); // 60 FPS
    scene.render();
}
```

### Loading Maps
```cpp
#include <Graphitech/ECS/ECS.hpp>

// Load from JSON file
ECS::GMapLoader loader;
bool success = loader.loadMap(scene, "Level1", "maps/level1");

// Or create programmatically
ECS::register_map_factory("MainMenu", [](ECS::Scene& s) {
    s.createMap("MainMenu");
    // Add menu entities...
});

ECS::tryCreateMapFromFactory(scene, "MainMenu");
scene.setCurrentMap("MainMenu");
```

### Custom Components
```cpp
// Define component
class HealthComponent : public ECS::AComponent {
public:
    int current = 100;
    int maximum = 100;
};

// Register parser
ECS::ComponentRegistry::get().registerParser("HealthComponent",
    [](ECS::Scene& s, ECS::Entity e, const nlohmann::json& data) {
        HealthComponent comp;
        comp.current = data.value("current", 100);
        comp.maximum = data.value("maximum", 100);
        s.addComponent(e, comp);
    });

// Use in code
scene.addComponent(player, HealthComponent{100, 100});
```

## Additional Resources

- [**RFC Documentation**](../ECS_RFC.md) - Architecture design and rationale
- [**Contributing Guide**](../CONTRIBUTING.md) - How to contribute to the project
- [**Complete API Reference**](../API.md) - All functions in one document

## Navigation

- [Back to Main Documentation](../README.md)
- [RFC Document](../ECS_RFC.md)
- [Contributing](../CONTRIBUTING.md)

---

*This documentation is automatically generated and kept in sync with the codebase.*