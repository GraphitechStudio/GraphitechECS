# Contributing to GraphitechECS

This guide explains how to contribute to the development of the Entity Component System (ECS) library.

## Navigation

### Documentation Files
- [**Home**](./README.md) - Documentation overview
- [**API Reference**](./API.md) - Complete API overview
- [**RFC Document**](./ECS_RFC.md) - Architecture specification
- [**Contributing Guide**](./CONTRIBUTING.md) - This development guide

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

## Getting Started

### Prerequisites
- C++17 compatible compiler
- CMake 3.15 or later
- Basic understanding of ECS architecture

### Development Setup
1. Clone the repository
2. Create a build directory: `mkdir build && cd build`
3. Configure with CMake: `cmake ..`
4. Build: `make`

## Code Style

### Naming Conventions
- Classes: PascalCase (e.g., `PositionComponent`)
- Functions: camelCase (e.g., `createEntity`)
- Variables: snake_case (e.g., `entity_id`)
- Constants: UPPER_SNAKE_CASE (e.g., `MAX_COMPONENTS`)

### File Organization
- Headers in `include/Graphitech/ECS/`
- Implementations in corresponding `.cpp` files
- Tests in `tests/` directory

## Adding Components

### 1. Create Component Class
Create a new header file in `Components/`:

```cpp
// filepath: Components/MyComponent.hpp
#ifndef MYCOMPONENT_HPP
#define MYCOMPONENT_HPP

#include <Graphitech/ECS/Core/AComponent.hpp>

namespace ECS {
    class MyComponent : public AComponent {
    public:
        int value = 0;
        std::string name = "Default";

        MyComponent() = default;
        MyComponent(int v, const std::string& n) : value(v), name(n) {}
    };
}

#endif // MYCOMPONENT_HPP
```

### 2. Add Implementation (if needed)
```cpp
// filepath: Components/MyComponent.cpp
#include "MyComponent.hpp"

// Implementation if required
```

### 3. Register with Build System
Add to `CMakeLists.txt`:
```cmake
file(GLOB ECS_SOURCES
    # ... existing sources ...
    ${CMAKE_CURRENT_SOURCE_DIR}/Components/MyComponent.cpp
)
```

### 4. Add to Main Header
Update `ECS.hpp`:
```cpp
#include "Components/MyComponent.hpp"
```

### 5. Register Parser (for map loading)
In `Registry/ComponentRegistration.hpp`, add:
```cpp
ECS::ComponentRegistry::get().registerParser("MyComponent",
    [](ECS::Scene& s, ECS::Entity e, const nlohmann::json& data) {
        ECS::MyComponent comp;
        if (data.contains("value")) comp.value = data["value"];
        if (data.contains("name")) comp.name = data["name"];
        s.addComponent(e, comp);
    });
```

## Adding Systems

### 1. Create System Function
Systems are typically free functions or lambdas:

```cpp
// filepath: Systems/MySystem.cpp
#include "MySystem.hpp"
#include <Graphitech/ECS/Components/MyComponent.hpp>

namespace ECS {
    void mySystem(Scene& scene, float deltaTime) {
        auto entities = scene.query<MyComponent>();
        for (auto& entity : entities) {
            auto* comp = entity.getComponent<MyComponent>();
            // System logic here
            comp->value += 1;
        }
    }
}
```

### 2. Add System Header
```cpp
// filepath: Systems/MySystem.hpp
#ifndef MYSYSTEM_HPP
#define MYSYSTEM_HPP

#include <Graphitech/ECS/Core/Scene.hpp>

namespace ECS {
    void mySystem(Scene& scene, float deltaTime);
}

#endif // MYSYSTEM_HPP
```

### 3. Integrate with Scene
In your game code:
```cpp
scene.addSystem(ECS::mySystem);
```

## Adding Factories

### Entity Factory
```cpp
// Register in your initialization code
ECS::getEntityFactory().registerFactory("MyEntity",
    [](ECS::Scene& scene, const std::string& name, ECS::Entity entity) {
        scene.addComponent(entity, ECS::MyComponent{42, name});
    });
```

### Map Factory
```cpp
ECS::register_map_factory("MyMap",
    [](ECS::Scene& scene) {
        // Create map entities
        ECS::Entity ent = scene.createEntity();
        scene.addComponent(ent, ECS::PositionComponent{0, 0});
    });
```

## Testing

### Unit Tests
Add tests in `tests/` directory using your preferred testing framework.

Example test structure:
```cpp
// filepath: tests/TestMyComponent.cpp
#include <gtest/gtest.h>
#include <Graphitech/ECS/Components/MyComponent.hpp>

TEST(MyComponentTest, DefaultValues) {
    ECS::MyComponent comp;
    EXPECT_EQ(comp.value, 0);
    EXPECT_EQ(comp.name, "Default");
}
```

## Documentation

### Function Documentation
Use Doxygen-style comments for all public functions:

```cpp
/**
 * @brief Brief description of the function
 *
 * Detailed description with examples.
 *
 * @param param1 Description of parameter
 * @param param2 Description of parameter
 * @return Description of return value
 *
 * @example
 * // Example usage
 * int result = myFunction(42, "test");
 */
int myFunction(int param1, const std::string& param2);
```

### Examples
Provide practical examples in documentation:

```cpp
/**
 * @brief Creates a player entity with default components
 *
 * @param scene The scene to create the entity in
 * @param name The player's name
 * @return The created entity
 *
 * @example
 * ECS::Scene scene;
 * ECS::Entity player = createPlayer(scene, "Hero");
 * // Player now has Position, Velocity, and Health components
 */
ECS::Entity createPlayer(ECS::Scene& scene, const std::string& name);
```

## Pull Request Process

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Make your changes following the guidelines above
4. Add tests for new functionality
5. Update documentation
6. Run existing tests: `make test`
7. Commit with clear messages: `git commit -m "Add MyComponent"`
8. Push to your fork: `git push origin feature/my-feature`
9. Create a Pull Request with detailed description

## Code Review Checklist

- [ ] Code follows style guidelines
- [ ] All public functions documented
- [ ] Tests added for new functionality
- [ ] No breaking changes without discussion
- [ ] Performance considerations addressed
- [ ] Examples provided in documentation

## Reporting Issues

Use GitHub Issues for:
- Bug reports (with minimal reproduction case)
- Feature requests (with use case description)
- Documentation improvements
- Performance issues

## Community

Join discussions in GitHub Discussions or Discord (if available).

Thank you for contributing to GraphitechECS!