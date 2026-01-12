/**
 * @file GMapLoader.cpp
 * @brief Implementation of JSON and binary map loader
 */

#include "GMapLoader.hpp"
#include <Graphitech/ECS/Components/PositionComponent.hpp>
#include <Graphitech/ECS/Components/ScaleComponent.hpp>
#include <Graphitech/ECS/Components/RotationComponent.hpp>
#include <Graphitech/ECS/Components/VelocityComponent.hpp>
#include <Graphitech/ECS/Components/TextureComponent.hpp>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

namespace ECS {

GMapLoader::GMapLoader() {
    // Register default component parsers

    // PositionComponent (2D only: x, y)
    registerParser("PositionComponent", [](Scene& s, Entity e, const json& data) {
        PositionComponent pos;
        if (data.contains("x")) pos.x = data["x"].get<float>();
        if (data.contains("y")) pos.y = data["y"].get<float>();
        s.addComponent(e, pos);
    });

    // ScaleComponent (2D only: x, y)
    registerParser("ScaleComponent", [](Scene& s, Entity e, const json& data) {
        ScaleComponent scale;
        if (data.contains("x")) scale.x = data["x"].get<float>();
        if (data.contains("y")) scale.y = data["y"].get<float>();
        s.addComponent(e, scale);
    });

    // RotationComponent (angle only in radians)
    registerParser("RotationComponent", [](Scene& s, Entity e, const json& data) {
        RotationComponent rot;
        if (data.contains("angle")) rot.angle = data["angle"].get<float>();
        s.addComponent(e, rot);
    });

    // VelocityComponent (2D only: vx, vy)
    registerParser("VelocityComponent", [](Scene& s, Entity e, const json& data) {
        VelocityComponent vel;
        if (data.contains("vx")) vel.vx = data["vx"].get<float>();
        if (data.contains("vy")) vel.vy = data["vy"].get<float>();
        s.addComponent(e, vel);
    });

    // TextureComponent (path, enabled)
    registerParser("TextureComponent", [](Scene& s, Entity e, const json& data) {
        TextureComponent tex;
        if (data.contains("path")) tex.texturePath = data["path"].get<std::string>();
        if (data.contains("enabled")) tex.enabled = data["enabled"].get<bool>();
        s.addComponent(e, tex);
    });

    // Note: UI parsers are intentionally not registered here to keep ECS
    // independent from Engine/UI. UI components should be parsed/registered
    // by the Engine or game-specific modules.
}

void GMapLoader::registerParser(const std::string& componentType, ComponentParser parser) {
    parsers_[componentType] = parser;
}

bool GMapLoader::fileExists(const std::string& path) const {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool GMapLoader::loadMap(Scene& scene, const std::string& mapName, const std::string& filepath) {
    // Only load .gmapb (binary - required after compilation)
    std::string binaryPath = filepath + ".gmapb";

    if (fileExists(binaryPath)) {
        std::cout << "[GMapLoader] Loading binary: " << binaryPath << std::endl;
        return loadBinaryMap(scene, mapName, binaryPath);
    }

    // Not found
    std::cerr << "[GMapLoader] Map not found: " << binaryPath << std::endl;
    return false;
}

bool GMapLoader::loadBinaryMap(Scene& scene, const std::string& mapName, const std::string& filepath) {
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "[GMapLoader] Failed to open binary: " << filepath << std::endl;
            return false;
        }

        // Read entire file into memory
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer(fileSize);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        file.close();

        BinaryReader reader(buffer);

        // ============ READ HEADER ============
        uint32_t magic = reader.read<uint32_t>();
        if (magic != GMAP_MAGIC) {
            std::cerr << "[GMapLoader] Invalid magic number: 0x" << std::hex << magic << std::dec << std::endl;
            return false;
        }

        uint16_t version_major = reader.read<uint16_t>();
        uint16_t version_minor = reader.read<uint16_t>();
        uint32_t total_size = reader.read<uint32_t>();
        uint32_t prefabs_offset = reader.read<uint32_t>();
        uint32_t entities_offset = reader.read<uint32_t>();

        std::cout << "[GMapLoader] Valid GMAP binary v" << version_major << "." << version_minor
              << " (size: " << total_size << " bytes)" << std::endl;

        // ============ READ PREFABS ============
        reader.seek(prefabs_offset);
        uint32_t prefab_count = reader.read<uint32_t>();

        std::unordered_map<std::string, json> loaded_prefabs;

        for (uint32_t i = 0; i < prefab_count; ++i) {
            uint32_t prefab_name_hash = reader.read<uint32_t>();
            std::string prefab_name = reader.readString();
            uint32_t comp_count = reader.read<uint32_t>();

            json components_json = json::array();
            for (uint32_t j = 0; j < comp_count; ++j) {
                uint32_t type_hash = reader.read<uint32_t>();
                std::string comp_type = reader.readString();
                std::string data_str = reader.readString();
                
                try {
                    json data = json::parse(data_str);
                    json comp_entry;
                    comp_entry["type"] = comp_type;
                    comp_entry["data"] = data;
                    components_json.push_back(comp_entry);
                } catch (const std::exception& e) {
                    std::cerr << "[GMapLoader] Error parsing component data: " << e.what() << std::endl;
                }
            }

            // Store prefab for later application (same shape as JSON loader expects)
            json prefab_json;
            prefab_json["components"] = components_json;
            loaded_prefabs[prefab_name] = prefab_json;
        }

        // expose to applyPrefabToEntity for runtime usage
        prefabs_ = loaded_prefabs;

        // ============ READ ENTITIES ============
        reader.seek(entities_offset);
        uint32_t entity_count = reader.read<uint32_t>();

        for (uint32_t i = 0; i < entity_count; ++i) {
            uint32_t entity_name_hash = reader.read<uint32_t>();
            uint32_t prefab_hash = reader.read<uint32_t>();
            std::string entity_name = reader.readString();
            std::string prefab_name = reader.readString();
            uint32_t comp_count = reader.read<uint32_t>();

            Entity entity = scene.createEntity();

            // Apply prefab if present
            if (!prefab_name.empty()) {
                auto pfit = loaded_prefabs.find(prefab_name);
                if (pfit != loaded_prefabs.end()) {
                    applyComponents(scene, entity, pfit->second["components"]);
                } else {
                    std::cerr << "[GMapLoader] Prefab not found: " << prefab_name << std::endl;
                }
            }

            // Parse entity components
            for (uint32_t j = 0; j < comp_count; ++j) {
                uint32_t type_hash = reader.read<uint32_t>();
                std::string comp_type = reader.readString();
                std::string data_str = reader.readString();
                
                try {
                    json comp_data = json::parse(data_str);
                    
                    // Apply component using the registered parser
                    auto it = parsers_.find(comp_type);
                    if (it != parsers_.end()) {
                        it->second(scene, entity, comp_data);
                    } else {
                        std::cerr << "[GMapLoader] No parser registered for component type: " << comp_type << std::endl;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "[GMapLoader] Error parsing entity component data: " << e.what() << std::endl;
                }
            }
        }

        std::cout << "[GMapLoader] Binary map loaded successfully!" << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "[GMapLoader] Error loading binary map: " << e.what() << std::endl;
        return false;
    }
}

bool GMapLoader::loadJsonMap(Scene& scene, const std::string& mapName, const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "[GMapLoader] Failed to open: " << filepath << std::endl;
            return false;
        }

        json data = json::parse(file);
        file.close();

        // Verify version
        if (!data.contains("version")) {
            std::cerr << "[GMapLoader] Missing version field in: " << filepath << std::endl;
            return false;
        }

        std::cout << "[GMapLoader] Loading map: " << mapName
                  << " (version " << data["version"] << ")" << std::endl;

        // Parse and cache prefabs
        prefabs_ = parsePrefabs(data);

        // Create entities
        if (data.contains("entities")) {
            for (const auto& entityData : data["entities"]) {
                createEntityFromJson(scene, entityData, prefabs_);
            }
        }

        std::cout << "[GMapLoader] Successfully loaded: " << filepath << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "[GMapLoader] Error loading " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

std::unordered_map<std::string, json> GMapLoader::parsePrefabs(const json& data) {
    std::unordered_map<std::string, json> prefabs;

    if (data.contains("prefabs")) {
        for (auto it = data["prefabs"].begin(); it != data["prefabs"].end(); ++it) {
            prefabs[it.key()] = it.value();
        }
    }

    return prefabs;
}

Entity GMapLoader::createEntityFromJson(Scene& scene, const json& entityData,
                                        const std::unordered_map<std::string, json>& prefabs) {
    // Create entity
    Entity entity = scene.createEntity();

    // Get entity name if provided
    std::string name = entityData.value("name", "");

    // Apply prefab if specified
    if (entityData.contains("prefab")) {
        std::string prefabName = entityData["prefab"].get<std::string>();
        auto it = prefabs.find(prefabName);
        if (it != prefabs.end()) {
            const json& prefabData = it->second;
            if (prefabData.contains("components")) {
                applyComponents(scene, entity, prefabData["components"]);
            }
        } else {
            std::cerr << "[GMapLoader] Prefab not found: " << prefabName << std::endl;
        }
    }

    // Apply entity-specific components (overrides prefab)
    if (entityData.contains("components")) {
        applyComponents(scene, entity, entityData["components"]);
    }

    if (!name.empty()) {
        std::cout << "[GMapLoader] Created entity: " << name << " (ID: " << entity << ")" << std::endl;
    }

    return entity;
}

void GMapLoader::applyComponents(Scene& scene, Entity entity, const json& components) {
    for (const auto& comp : components) {
        if (!comp.contains("type")) {
            std::cerr << "[GMapLoader] Component missing 'type' field" << std::endl;
            continue;
        }

        std::string type = comp["type"].get<std::string>();
        auto it = parsers_.find(type);

        if (it != parsers_.end()) {
            json compData = comp.value("data", json::object());
            it->second(scene, entity, compData);
        } else {
            std::cerr << "[GMapLoader] No parser registered for component type: " << type << std::endl;
        }
    }
}

bool GMapLoader::applyPrefabToEntity(Scene& scene, Entity entity, const std::string& prefabName) {
    auto it = prefabs_.find(prefabName);
    if (it == prefabs_.end()) {
        std::cerr << "[GMapLoader] Prefab not found: " << prefabName << std::endl;
        return false;
    }

    const json& prefabData = it->second;
    if (prefabData.contains("components")) {
        applyComponents(scene, entity, prefabData["components"]);
        std::cout << "[GMapLoader] Applied prefab '" << prefabName << "' to entity "
                  << entity << std::endl;
        return true;
    }

    return false;
}

} // namespace ECS

