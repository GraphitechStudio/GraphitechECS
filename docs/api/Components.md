# Built-in Components Reference

This document describes the built-in components provided by the GraphitechECS library. These components can be used directly without additional implementation.

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
- [**MapLoader**](../api/MapLoader.md) - Dynamic library loader

### Registry Systems
- [**ComponentRegistry**](../api/ComponentRegistry.md) - Component parser registry

### Components
- [**Built-in Components**](../api/Components.md) - This Components documentation

---

## Table of Contents
- [PositionComponent](#positioncomponent)
- [VelocityComponent](#velocitycomponent)
- [ScaleComponent](#scalecomponent)
- [RotationComponent](#rotationcomponent)
- [Render Components](#render-components)
- [TTLComponent](#ttlcomponent)
- [CollisionComponent](#collisioncomponent)

---

## PositionComponent

Represents 2D position in the game world.

**Header:** `Components/PositionComponent.hpp`

**Members:**
- `float x`: X-coordinate
- `float y`: Y-coordinate

**Example:**
```cpp
ECS::PositionComponent pos;
pos.x = 100.0f;
pos.y = 200.0f;
scene.addComponent(entity, pos);
```

**JSON Format:**
```json
{
  "PositionComponent": {
    "x": 100.0,
    "y": 200.0
  }
}
```

---

## VelocityComponent

Represents linear velocity for movement.

**Header:** `Components/VelocityComponent.hpp`

**Members:**
- `float vx`: Velocity in X direction (units per second)
- `float vy`: Velocity in Y direction (units per second)

**Example:**
```cpp
ECS::VelocityComponent vel;
vel.vx = 5.0f;  // 5 units/second right
vel.vy = -2.0f; // 2 units/second up
scene.addComponent(entity, vel);
```

**JSON Format:**
```json
{
  "VelocityComponent": {
    "vx": 5.0,
    "vy": -2.0
  }
}
```

---

## ScaleComponent

Represents scaling factors for rendering.

**Header:** `Components/ScaleComponent.hpp`

**Members:**
- `float x`: Scale factor in X direction
- `float y`: Scale factor in Y direction

**Example:**
```cpp
ECS::ScaleComponent scale;
scale.x = 2.0f; // Double width
scale.y = 1.5f; // 1.5x height
scene.addComponent(entity, scale);
```

**JSON Format:**
```json
{
  "ScaleComponent": {
    "x": 2.0,
    "y": 1.5
  }
}
```

---

## RotationComponent

Represents rotation angle.

**Header:** `Components/RotationComponent.hpp`

**Members:**
- `float angle`: Rotation angle in radians

**Example:**
```cpp
ECS::RotationComponent rot;
rot.angle = 3.14159f / 2.0f; // 90 degrees
scene.addComponent(entity, rot);
```

**JSON Format:**
```json
{
  "RotationComponent": {
    "angle": 1.5708
  }
}
```

---

## Render Components

### RenderCircle

Debug rendering component for circles.

**Header:** `Components/RenderPrimitives.hpp`

**Members:**
- `float radius`: Circle radius
- `float r, g, b`: RGB color components (0.0-1.0)

**Example:**
```cpp
ECS::RenderCircle circle;
circle.radius = 16.0f;
circle.r = 1.0f; circle.g = 0.0f; circle.b = 0.0f; // Red
scene.addComponent(entity, circle);
```

**JSON Format:**
```json
{
  "RenderCircle": {
    "radius": 16.0,
    "r": 1.0,
    "g": 0.0,
    "b": 0.0
  }
}
```

### RenderLine

Debug rendering component for lines.

**Header:** `Components/RenderPrimitives.hpp`

**Members:**
- `float x1, y1`: Start point coordinates
- `float x2, y2`: End point coordinates

**Example:**
```cpp
ECS::RenderLine line;
line.x1 = 0.0f; line.y1 = 0.0f;
line.x2 = 100.0f; line.y2 = 100.0f;
scene.addComponent(entity, line);
```

**JSON Format:**
```json
{
  "RenderLine": {
    "x1": 0.0,
    "y1": 0.0,
    "x2": 100.0,
    "y2": 100.0
  }
}
```

---

## TTLComponent

Time-to-live component for temporary entities.

**Header:** `Components/TTLComponent.hpp`

**Members:**
- `float timeLeft`: Remaining time in seconds

**Example:**
```cpp
ECS::TTLComponent ttl;
ttl.timeLeft = 5.0f; // 5 seconds lifetime
scene.addComponent(entity, ttl);
```

**JSON Format:**
```json
{
  "TTLComponent": {
    "timeLeft": 5.0
  }
}
```

---

## CollisionComponent

Collision detection component.

**Header:** `Components/CollisionComponent.hpp`

**Members:**
- `float width`: Collision box width
- `float height`: Collision box height

**Example:**
```cpp
ECS::CollisionComponent collision;
collision.width = 32.0f;
collision.height = 32.0f;
scene.addComponent(entity, collision);
```

**JSON Format:**
```json
{
  "CollisionComponent": {
    "width": 32.0,
    "height": 32.0
  }
}
```

---

## Custom Components

To create custom components, inherit from `ECS::AComponent`:

```cpp
#include <Graphitech/ECS/Core/AComponent.hpp>

class HealthComponent : public ECS::AComponent {
public:
    int currentHealth = 100;
    int maxHealth = 100;

    HealthComponent() = default;
    HealthComponent(int current, int max)
        : currentHealth(current), maxHealth(max) {}
};
```

### Registering Custom Component Parsers

```cpp
ECS::ComponentRegistry::get().registerParser("HealthComponent",
    [](ECS::Scene& s, ECS::Entity e, const nlohmann::json& data) {
        ECS::HealthComponent comp;
        if (data.contains("current")) comp.currentHealth = data["current"];
        if (data.contains("max")) comp.maxHealth = data["max"];
        s.addComponent(e, comp);
    });
```

### Using Custom Components

```cpp
// Add to entity
scene.addComponent(player, HealthComponent{100, 100});

// Query entities with custom component
auto entities = scene.getEntitiesWith<HealthComponent>();

// Access component
for (auto entity : entities) {
    auto* health = scene.getComponent<HealthComponent>(entity);
    if (health) {
        health->currentHealth -= damage;
    }
}
```

---

## Component Requirements

All components must:
1. Inherit from `ECS::AComponent`
2. Be default constructible
3. Have public members or appropriate getters/setters
4. Be registered with the ComponentRegistry for map loading

**Example complete custom component:**
```cpp
// HealthComponent.hpp
#ifndef HEALTHCOMPONENT_HPP
#define HEALTHCOMPONENT_HPP

#include <Graphitech/ECS/Core/AComponent.hpp>

namespace ECS {
    class HealthComponent : public AComponent {
    public:
        int currentHealth = 100;
        int maxHealth = 100;

        HealthComponent() = default;
        HealthComponent(int current, int max)
            : currentHealth(current), maxHealth(max) {}

        // Optional: Add methods
        bool isAlive() const { return currentHealth > 0; }
        void takeDamage(int damage) {
            currentHealth = std::max(0, currentHealth - damage);
        }

        // Static parser registration
        static void registerParser() {
            ComponentRegistry::get().registerParser("HealthComponent",
                [](Scene& s, Entity e, const nlohmann::json& data) {
                    HealthComponent comp;
                    if (data.contains("current"))
                        comp.currentHealth = data["current"].get<int>();
                    if (data.contains("max"))
                        comp.maxHealth = data["max"].get<int>();
                    s.addComponent(e, comp);
                });
        }
    };
}

#endif // HEALTHCOMPONENT_HPP
```

[⬆️ Back to API Index](../API.md)