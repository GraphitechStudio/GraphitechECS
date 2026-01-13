# Request for Comments: Entity Component System (ECS) Architecture

## Navigation

### Documentation Files
- [**Home**](./README.md) - Documentation overview
- [**API Reference**](./API.md) - Complete API overview
- [**RFC Document**](./ECS_RFC.md) - This architecture specification
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

## Status

**Draft** - This RFC describes the proposed Entity Component System architecture for the GraphitechECS library.

## Abstract

This RFC outlines the design and implementation of an Entity Component System (ECS) for game development. The ECS provides a flexible, performant architecture for managing game entities, their components, and the systems that operate on them. It supports dynamic entity creation, component-based composition, and efficient querying.

## 1. Introduction

### 1.1 Purpose

The ECS architecture separates data (components) from logic (systems), allowing for better code organization, reusability, and performance optimization. This design enables developers to create complex game worlds with minimal coupling between different aspects of game objects.

### 1.2 Scope

This RFC covers:
- Core ECS concepts (Entities, Components, Systems)
- Scene management
- Component registration and parsing
- Map loading and factories
- Performance considerations

## 2. Architecture Overview

### 2.1 Core Concepts

#### Entities
Entities are lightweight identifiers that represent game objects. They don't contain data directly but serve as containers for components.

```cpp
// Creating an entity
ECS::Entity entity = scene.createEntity();

// Checking if entity is valid
if (entity) {
    // Entity exists
}
```

#### Components
Components are data structures attached to entities. Each component type represents a specific aspect of an entity (position, velocity, rendering, etc.).

```cpp
// Defining a custom component
class HealthComponent : public ECS::AComponent {
public:
    int health = 100;
    int maxHealth = 100;
};

// Adding a component to an entity
scene.addComponent(entity, ECS::PositionComponent{10.0f, 20.0f});
```

#### Systems
Systems are functions that operate on entities with specific component combinations. They implement game logic.

```cpp
// Movement system example
void movementSystem(ECS::Scene& scene, float deltaTime) {
    auto entities = scene.query<ECS::PositionComponent, ECS::VelocityComponent>();
    for (auto& entity : entities) {
        auto* pos = entity.getComponent<ECS::PositionComponent>();
        auto* vel = entity.getComponent<ECS::VelocityComponent>();
        pos->x += vel->vx * deltaTime;
        pos->y += vel->vy * deltaTime;
    }
}

// Adding system to scene
scene.addSystem(movementSystem);
```

### 2.2 Scene Management

The Scene class manages all entities, components, and systems.

```cpp
// Creating a scene
ECS::Scene scene("MyGameScene");

// Creating entities
ECS::Entity player = scene.createEntity();
ECS::Entity enemy = scene.createEntity();

// Adding components
scene.addComponent(player, ECS::PositionComponent{0.0f, 0.0f});
scene.addComponent(player, ECS::VelocityComponent{1.0f, 0.0f});

// Updating the scene
scene.update(0.016f); // 60 FPS
scene.render();
```

## 3. Component System

### 3.1 Built-in Components

#### PositionComponent
Represents 2D position.

```cpp
ECS::PositionComponent pos;
pos.x = 10.0f;
pos.y = 20.0f;
scene.addComponent(entity, pos);
```

#### VelocityComponent
Represents linear velocity in units per second.

```cpp
ECS::VelocityComponent vel;
vel.vx = 5.0f; // 5 units/second right
vel.vy = -2.0f; // 2 units/second up
scene.addComponent(entity, vel);
```

#### ScaleComponent
Represents scaling factors.

```cpp
ECS::ScaleComponent scale;
scale.x = 2.0f; // Double width
scale.y = 1.5f; // 1.5x height
scene.addComponent(entity, scale);
```

#### RotationComponent
Represents rotation in radians.

```cpp
ECS::RotationComponent rot;
rot.angle = 3.14159f / 2.0f; // 90 degrees
scene.addComponent(entity, rot);
```

#### Render Primitives
Simple rendering components for debug visualization.

```cpp
// Circle
ECS::RenderCircle circle;
circle.radius = 16.0f;
circle.r = 1.0f; circle.g = 0.0f; circle.b = 0.0f; // Red
scene.addComponent(entity, circle);

// Line
ECS::RenderLine line;
line.x1 = 0.0f; line.y1 = 0.0f;
line.x2 = 100.0f; line.y2 = 100.0f;
scene.addComponent(entity, line);
```

### 3.2 Custom Components

To create custom components, inherit from `ECS::AComponent`:

```cpp
class CustomComponent : public ECS::AComponent {
public:
    int customValue = 42;
    std::string customString = "Hello";
};
```

### 3.3 Component Registration

For map loading, register component parsers:

```cpp
ECS::ComponentRegistry::get().registerParser("CustomComponent",
    [](ECS::Scene& s, ECS::Entity e, const nlohmann::json& data) {
        ECS::CustomComponent comp;
        if (data.contains("value")) comp.customValue = data["value"];
        if (data.contains("text")) comp.customString = data["text"];
        s.addComponent(e, comp);
    });
```

## 4. Map Loading System

### 4.1 GMAP Format

Maps are defined in JSON (.gmap) or binary (.gmapb) format.

#### JSON Example
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

#### Loading Maps
```cpp
ECS::GMapLoader loader;
bool success = loader.loadMap(scene, "Level1", "maps/level1");
if (success) {
    std::cout << "Map loaded successfully!" << std::endl;
}
```

### 4.2 Binary Compilation

Compile JSON maps to binary for faster loading:

```bash
gmap-compiler input.gmap -o output.gmapb -v
```

## 5. Factory Systems

### 5.1 Entity Factories

Register entity creation functions:

```cpp
ECS::getEntityFactory().registerFactory("Player",
    [](ECS::Scene& scene, const std::string& name, ECS::Entity entity) {
        scene.addComponent(entity, ECS::PositionComponent{0, 0});
        scene.addComponent(entity, ECS::VelocityComponent{0, 0});
        // Add more components...
    });

// Create entity from factory
ECS::Entity player = scene.createEntityFromFactory("Player", "Hero");
```

### 5.2 Map Factories

Create maps programmatically:

```cpp
ECS::register_map_factory("MainMenu",
    [](ECS::Scene& scene) {
        // Create menu entities
        ECS::Entity title = scene.createEntity();
        scene.addComponent(title, ECS::PositionComponent{400, 100});
        // Add UI components...
    });

// Load map
ECS::tryCreateMapFromFactory(scene, "MainMenu");
```

## 6. Querying and Iteration

### 6.1 Component Queries

Query entities with specific components:

```cpp
// Get all entities with Position and Velocity
auto movingEntities = scene.query<ECS::PositionComponent, ECS::VelocityComponent>();

for (auto& entity : movingEntities) {
    auto* pos = entity.getComponent<ECS::PositionComponent>();
    auto* vel = entity.getComponent<ECS::VelocityComponent>();
    // Update position...
}
```

### 6.2 Map Management

```cpp
// Create and switch maps
scene.createMap("Gameplay");
scene.switchToMap("Gameplay");

// Add systems to specific maps
scene.addMapSystem("Gameplay", physicsSystem);

// List available maps
auto maps = scene.listMaps();
```

## 7. Performance Considerations

### 7.1 Memory Layout
- Components are stored in contiguous arrays by type
- Entity IDs are simple integers for fast lookup
- Bitsets track component presence efficiently

### 7.2 Iteration
- Systems iterate over component arrays directly
- No virtual function calls during iteration
- Cache-friendly memory access patterns

### 7.3 Binary Maps
- 10x faster loading than JSON
- 75% smaller file sizes
- Optimized for runtime performance

## 8. Implementation Details

### 8.1 Entity Management
Entities are created via `Scene::createEntity()` and automatically added to the current map.

### 8.2 Component Storage
Components are stored in type-indexed maps within the Scene.

### 8.3 System Execution
Systems are called during `Scene::update()` with a delta time parameter.

## 9. Future Extensions

- Spatial partitioning for collision detection
- Event system for inter-entity communication
- Serialization support for save/load functionality
- Multi-threading for parallel system execution

## 10. References

- [Entity Component System on Wikipedia](https://en.wikipedia.org/wiki/Entity_component_system)
- [ECS Architecture Benefits](https://gameprogrammingpatterns.com/component.html)