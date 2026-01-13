# EntityFactory Class Reference

The `EntityFactory` class provides a registry system for entity creation functions. Factory functions can be registered with unique names and later called to create entities with specific component configurations.

## Navigation

### Documentation Files
- [**Home**](../README.md) - Documentation overview
- [**API Reference**](../API.md) - Complete API overview
- [**RFC Document**](../ECS_RFC.md) - Architecture specification
- [**Contributing Guide**](../CONTRIBUTING.md) - Development guidelines

### Core Classes
- [**Scene**](../api/Scene.md) - Scene container class
- [**EntityFactory**](../api/EntityFactory.md) - This EntityFactory documentation

### Loading & Serialization
- [**GMapLoader**](../api/GMapLoader.md) - Map file loader
- [**MapLoader**](../api/MapLoader.md) - Dynamic library loader

### Registry Systems
- [**ComponentRegistry**](../api/ComponentRegistry.md) - Component parser registry

### Components
- [**Built-in Components**](../api/Components.md) - Pre-implemented components

---

## Table of Contents
- [Factory Registration](#factory-registration)
- [Factory Retrieval](#factory-retrieval)
- [Factory Management](#factory-management)

---

## Factory Registration

### `bool registerFactory(const std::string& name, FactoryFn factory)`
Registers a factory function with a given name.

**Parameters:**
- `name`: The unique name identifier for this factory
- `factory`: The factory function to register

**Returns:** true if registration succeeded, false if the name already exists

**Example:**
```cpp
ECS::getEntityFactory().registerFactory("Player",
    [](ECS::Scene& s, const std::string& name, ECS::Entity e) {
        s.addComponent(e, ECS::PositionComponent{0, 0});
        s.addComponent(e, ECS::VelocityComponent{0, 0});
    });
```

---

## Factory Retrieval

### `const FactoryFn* getFactory(const std::string& name) const`
Retrieves a factory function by name.

**Parameters:**
- `name`: The factory name to look up

**Returns:** Pointer to the factory function, or nullptr if not found

**Example:**
```cpp
auto* factory = ECS::getEntityFactory().getFactory("Player");
if (factory) {
    // Use factory
}
```

### `bool hasFactory(const std::string& name) const`
Checks if a factory with the given name exists.

**Parameters:**
- `name`: The factory name to check

**Returns:** true if the factory exists, false otherwise

**Example:**
```cpp
if (ECS::getEntityFactory().hasFactory("Player")) {
    // Factory exists
}
```

---

## Factory Management

### `bool unregisterFactory(const std::string& name)`
Unregisters a factory function.

**Parameters:**
- `name`: The name of the factory to remove

**Returns:** true if a factory was removed, false if the name wasn't found

**Example:**
```cpp
ECS::getEntityFactory().unregisterFactory("Player");
```

### `std::vector<std::string> listFactories() const`
Lists all registered factory names.

**Returns:** Vector containing the names of all registered factories

**Example:**
```cpp
auto factories = ECS::getEntityFactory().listFactories();
for (const auto& name : factories) {
    std::cout << "Factory: " << name << std::endl;
}
```

---

## Global Access

### `EntityFactory& getEntityFactory()`
Gets the global EntityFactory instance.

**Returns:** Reference to the global EntityFactory instance

**Example:**
```cpp
auto& factory = ECS::getEntityFactory();
```

---

## Type Definitions

### `using FactoryFn = std::function<void(Scene&, const std::string&, Entity)>`
Type alias for factory functions.

Factory functions receive:
- `Scene&`: Reference to the scene
- `const std::string&`: Entity name
- `Entity`: The entity to configure

**Example:**
```cpp
using ECS::EntityFactory::FactoryFn;

FactoryFn playerFactory = [](ECS::Scene& scene, const std::string& name, ECS::Entity entity) {
    // Configure player entity
    scene.addComponent(entity, ECS::PositionComponent{100, 100});
    scene.addComponent(entity, ECS::VelocityComponent{0, 0});
    // Add more components...
};
```

[⬆️ Back to API Index](../API.md)