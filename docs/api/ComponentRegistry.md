# ComponentRegistry Class Reference

The `ComponentRegistry` is a global singleton that manages component parsers for the GMapLoader. It allows games to register custom component parsers without modifying the loader code.

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
- [**ComponentRegistry**](../api/ComponentRegistry.md) - This ComponentRegistry documentation

### Components
- [**Built-in Components**](../api/Components.md) - Pre-implemented components

---

## Table of Contents
- [Singleton Access](#singleton-access)
- [Parser Registration](#parser-registration)
- [Parser Retrieval](#parser-retrieval)
- [Parser Management](#parser-management)

---

## Singleton Access

### `static ComponentRegistry& get()`
Gets the singleton instance of the ComponentRegistry.

**Returns:** Reference to the global ComponentRegistry instance

**Example:**
```cpp
ECS::ComponentRegistry::get().registerParser("MyComponent", parser);
```

---

## Parser Registration

### `void registerParser(const std::string& componentType, ComponentParser parser)`
Registers a component parser for a specific component type.

**Parameters:**
- `componentType`: The component type name (e.g., "PositionComponent")
- `parser`: The parser function

**Example:**
```cpp
ECS::ComponentRegistry::get().registerParser("PositionComponent",
    [](ECS::Scene& s, ECS::Entity e, const nlohmann::json& data) {
        ECS::PositionComponent comp;
        comp.x = data["x"];
        comp.y = data["y"];
        s.addComponent(e, comp);
    });
```

---

## Parser Retrieval

### `ComponentParser* getParser(const std::string& componentType)`
Gets a registered parser by component type.

**Parameters:**
- `componentType`: The component type name

**Returns:** Pointer to the parser function, or nullptr if not found

**Example:**
```cpp
auto* parser = ECS::ComponentRegistry::get().getParser("PositionComponent");
if (parser) {
    // Use parser
}
```

### `bool hasParser(const std::string& componentType) const`
Checks if a parser is registered for the given component type.

**Parameters:**
- `componentType`: The component type name

**Returns:** true if registered, false otherwise

**Example:**
```cpp
if (ECS::ComponentRegistry::get().hasParser("MyComponent")) {
    // Parser exists
}
```

---

## Parser Management

### `std::vector<std::string> listParsers() const`
Lists all registered parser names.

**Returns:** Vector of component type names

**Example:**
```cpp
auto parsers = ECS::ComponentRegistry::get().listParsers();
for (const auto& name : parsers) {
    std::cout << "Parser: " << name << std::endl;
}
```

---

## Type Definitions

### `using ComponentParser = std::function<void(Scene&, Entity, const json&)>`
Type alias for component parser functions.

Parser functions receive:
- `Scene&`: Reference to the scene
- `Entity`: The entity to add the component to
- `const json&`: JSON data for the component

**Example:**
```cpp
using ECS::ComponentParser;

ComponentParser positionParser = [](ECS::Scene& scene, ECS::Entity entity, const nlohmann::json& data) {
    ECS::PositionComponent comp;
    comp.x = data["x"].get<float>();
    comp.y = data["y"].get<float>();
    scene.addComponent(entity, comp);
};
```

---

## Usage Patterns

### Static Registration in Component Header
```cpp
// MyComponent.hpp
class MyComponent : public ECS::AComponent {
public:
    int value;

    // Static registration
    static void registerParser() {
        ECS::ComponentRegistry::get().registerParser("MyComponent",
            [](ECS::Scene& s, ECS::Entity e, const nlohmann::json& data) {
                MyComponent comp;
                if (data.contains("value")) {
                    comp.value = data["value"].get<int>();
                }
                s.addComponent(e, comp);
            });
    }
};
```

### Registration in Main Application
```cpp
// main.cpp
int main() {
    // Register custom component parsers
    ECS::ComponentRegistry::get().registerParser("HealthComponent", healthParser);
    ECS::ComponentRegistry::get().registerParser("WeaponComponent", weaponParser);

    // Now GMapLoader can parse these components
    ECS::GMapLoader loader;
    loader.loadMap(scene, "Level1", "maps/level1");

    return 0;
}
```

---

## Built-in Parsers

The ComponentRegistry automatically registers parsers for built-in components:
- `PositionComponent`
- `VelocityComponent`
- `ScaleComponent`
- `RotationComponent`
- `RenderCircle`
- `RenderLine`
- `TTLComponent`
- `CollisionComponent`

---

## Error Handling

- **Duplicate registration**: Attempting to register a parser for an already registered component type will overwrite the previous parser
- **Missing parsers**: If a component type is encountered in a map file without a registered parser, loading will fail with an error message
- **Invalid JSON**: Parser functions should handle malformed JSON data gracefully

**Example error:**
```
Component parser not found for 'UnknownComponent'
```

[⬆️ Back to API Index](../API.md)