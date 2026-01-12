#ifndef MAP_LOADER_HPP
    #define MAP_LOADER_HPP

// ECS does not include Engine; dynamic loading facilities must be provided
// by external modules if needed.
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#if defined(__unix__)
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#endif

namespace ECS {

class Scene;  // Forward declaration

/**
 * @brief Type alias for map factory functions.
 *
 * @details A map factory is a function that takes a Scene reference and
 * initializes it with the map's entities, components, and logic.
 */
using MapFactory = std::function<void(Scene&)>;

/**
 * @brief Gets the global registry of map factories.
 *
 * @details Returns a static map that stores registered map factories by name.
 * This allows maps to be registered at startup and retrieved later.
 *
 * @return Reference to the global map factory registry.
 */
inline std::unordered_map<std::string, MapFactory>& get_map_factories() {
    static std::unordered_map<std::string, MapFactory> m;
    return m;
}

/**
 * @brief Registers a map factory function with a name.
 *
 * @details Stores a map factory function in the global registry, allowing it
 * to be retrieved and called by name later.
 *
 * @param name The unique identifier for this map.
 * @param f The factory function that initializes the map.
 */
inline void register_map_factory(const std::string& name, MapFactory f) {
    get_map_factories()[name] = std::move(f);
}

/**
 * @brief Attempts to load a map by name.
 *
 * @details First checks if the map is already registered in the factory registry.
 * If not found, attempts to dynamically load a shared library (.so) containing
 * the map implementation. The function searches multiple potential paths for the
 * library file.
 *
 * On Unix systems, the following paths are checked (in order):
 * - src/maps/{name}/libmap_{name}.so
 * - maps/{name}/libmap_{name}.so
 * - build/src/maps/{name}/libmap_{name}.so
 * - ./build/src/maps/{name}/libmap_{name}.so
 * - {cwd}/src/maps/{name}/libmap_{name}.so
 * - {cwd}/build/src/maps/{name}/libmap_{name}.so
 *
 * The loaded library must export either a `register_map_{name}` or `register_map`
 * function that accepts a Scene reference.
 *
 * @param s The Scene to initialize with the loaded map.
 * @param name The name of the map to load.
 * @return true if the map was successfully loaded, false otherwise.
 *
 * @note On non-Unix systems, only pre-registered maps can be loaded.
 * @note Loaded libraries are kept in memory for the application lifetime.
 */
inline bool try_load_map_library(Scene& s, const std::string& name) {
    auto& m = get_map_factories();
    auto it = m.find(name);
    if (it != m.end()) {
        it->second(s);
        return true;
    }

#if defined(__unix__) && 0
    std::vector<std::string> candidates;
    // New paths: Games/<game>/Client compiled to build/maps/<game>/
    candidates.push_back(std::string("build/maps/") + name + "/libmap_" + name + ".so");
    candidates.push_back(std::string("./build/maps/") + name + "/libmap_" + name + ".so");
    
    // Legacy paths for backward compatibility
    candidates.push_back(std::string("src/maps/") + name + "/libmap_" + name + ".so");
    candidates.push_back(std::string("maps/") + name + "/libmap_" + name + ".so");
    candidates.push_back(std::string("build/src/maps/") + name + "/libmap_" + name + ".so");
    candidates.push_back(std::string("./build/src/maps/") + name + "/libmap_" + name + ".so");

    char cwd_buf[PATH_MAX];
    if (getcwd(cwd_buf, sizeof(cwd_buf))) {
        std::string cwd(cwd_buf);
        candidates.push_back(cwd + "/build/maps/" + name + "/libmap_" + name + ".so");
        candidates.push_back(cwd + "/src/maps/" + name + "/libmap_" + name + ".so");
        candidates.push_back(cwd + "/build/src/maps/" + name + "/libmap_" + name + ".so");
    }

    using RegFn = void (*)(Scene&);
    static std::vector<std::unique_ptr<void>> s_loaders;
    
    for (auto& soPath : candidates) {
        struct stat st;
        if (stat(soPath.c_str(), &st) == 0) {
            std::cerr << "[MapLoader] candidate exists: " << soPath << " (size=" << st.st_size << ")\n";
        }
        
        auto loader = std::make_unique<DLLoader<RegFn>>(soPath.c_str());
        if (!*loader) {
            continue;
        }
        
        std::string sym = std::string("register_map_") + name;
        RegFn reg = loader->getSymbol(sym);
        if (!reg) {
            reg = loader->getSymbol("register_map");
        }
        
        if (!reg) {
            std::cerr << "[MapLoader] getSymbol failed to find '" << sym << "' or 'register_map' in " << soPath << "\n";
            continue;
        }

        reg(s);
        get_map_factories()[name] = [reg](Scene& sc) { reg(sc); };

        s_loaders.push_back(std::move(loader));

        return true;
    }
    
    std::cerr << "[MapLoader] Failed to load map '" << name << "'\n";
    return false;
#else
    // Dynamic loading disabled inside ECS: only pre-registered maps are supported
    return false;
#endif
}

} // namespace ECS
#endif