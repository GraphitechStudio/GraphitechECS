/**
 * @file GMapLoader.hpp
 * @brief JSON and Binary .gmap file loader for scene initialization
 *
 * Supports both .gmap (JSON) and .gmapb (binary) formats.
 * Prioritizes .gmapb for performance, falls back to .gmap if needed.
 */

#ifndef GMAP_LOADER_HPP
#define GMAP_LOADER_HPP

#include <Graphitech/ECS/Core/Scene.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <functional>

namespace ECS {

using json = nlohmann::json;

/**
 * @brief Component parser function type.
 */
using ComponentParser = std::function<void(Scene&, Entity, const json&)>;

/**
 * @brief Loads .gmap (JSON) and .gmapb (binary) files.
 *
 * @details
 * Priority order:
 * 1. .gmapb (binary - fast)
 * 2. .gmap (JSON - fallback)
 *
 * Binary format: GMAP v1.0 with magic number 0x50414D47
 */
class GMapLoader {
private:
    std::unordered_map<std::string, ComponentParser> parsers_;
    std::unordered_map<std::string, json> prefabs_;  // Cache prefabs for factory use

    // Binary format constants
    static constexpr uint32_t GMAP_MAGIC = 0x50414D47;  // "GMAP"
    static constexpr uint16_t GMAP_VERSION_MAJOR = 1;
    static constexpr uint16_t GMAP_VERSION_MINOR = 0;

public:
    /**
     * @brief Constructs a GMapLoader and registers default parsers.
     */
    GMapLoader();

    /**
     * @brief Registers a component parser.
     *
     * @param componentType The component type name (e.g., "PositionComponent")
     * @param parser The parser function
     */
    void registerParser(const std::string& componentType, ComponentParser parser);

    /**
     * @brief Loads a .gmap or .gmapb file and populates the scene.
     *
     * @param scene The scene to load into
     * @param mapName The map name in the scene
     * @param filepath Path to the .gmap/.gmapb file (without extension)
     * @return true if loaded successfully, false otherwise
     *
     * @details
     * Automatically detects format:
     * 1. Tries .gmapb (binary)
     * 2. Falls back to .gmap (JSON)
     */
    bool loadMap(Scene& scene, const std::string& mapName, const std::string& filepath);

    /**
     * @brief Applies a prefab to an existing entity.
     *
     * @param scene The scene containing the entity
     * @param entity The entity to apply the prefab to
     * @param prefabName The name of the prefab to apply
     * @return true if prefab was found and applied, false otherwise
     */
    bool applyPrefabToEntity(Scene& scene, Entity entity, const std::string& prefabName);

private:
    /**
     * @brief Binary reader helper for .gmapb parsing
     */
    struct BinaryReader {
        const std::vector<uint8_t>& data;
        size_t pos = 0;

        BinaryReader(const std::vector<uint8_t>& d) : data(d) {}

        template<typename T>
        T read() {
            T value = 0;
            for (size_t i = 0; i < sizeof(T); ++i) {
                if (pos >= data.size()) throw std::runtime_error("Binary read out of bounds");
                value |= (static_cast<T>(data[pos++]) << (i * 8));
            }
            return value;
        }

        std::string readString() {
            uint32_t len = read<uint32_t>();
            if (pos + len > data.size()) throw std::runtime_error("String read out of bounds");
            std::string str(data.begin() + pos, data.begin() + pos + len);
            pos += len;
            return str;
        }

        void skip(size_t bytes) {
            pos += bytes;
        }

        void seek(size_t newPos) {
            pos = newPos;
        }
    };

    /**
     * @brief Checks if a file exists
     */
    bool fileExists(const std::string& path) const;

    /**
     * @brief Loads a binary .gmapb file
     */
    bool loadBinaryMap(Scene& scene, const std::string& mapName, const std::string& filepath);

    /**
     * @brief Loads a JSON .gmap file
     */
    bool loadJsonMap(Scene& scene, const std::string& mapName, const std::string& filepath);

    /**
     * @brief Parses prefabs from the JSON.
     */
    std::unordered_map<std::string, json> parsePrefabs(const json& data);

    /**
     * @brief Creates an entity from a prefab or entity definition.
     */
    Entity createEntityFromJson(Scene& scene, const json& entityData,
                                 const std::unordered_map<std::string, json>& prefabs);

    /**
     * @brief Applies components to an entity.
     */
    void applyComponents(Scene& scene, Entity entity, const json& components);
};

} // namespace ECS

#endif // GMAP_LOADER_HPP

