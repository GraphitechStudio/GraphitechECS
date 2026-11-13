// language: cpp
/*
** EPITECH PROJECT, 2025
** GraphitechEngine
** File description:
** MapFactory.hpp
*/

#ifndef MAPFACTORY_HPP
#define MAPFACTORY_HPP
#include <filesystem>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <dlfcn.h>

// Forward declare MapEvent (defined in ECS/Types.hpp)
namespace ECS {
    class Scene;
    struct MapEvent;

    /**
     * @brief Function type for map creation factories
     * @param scene Reference to the ECS scene where the map will be created
     */
    using MapFactory = std::function<void(Scene &)>;

    /**
     * @brief Get the global map factory registry
     * @return Reference to the unordered_map containing registered map factories
     * @note This function returns a static instance, so all calls return the same registry
     */
    inline std::unordered_map<std::string, MapFactory> &getMapFactory()
    {
        static std::unordered_map<std::string, MapFactory> mapFactory;
        return mapFactory;
    }

    /**
     * @brief Attempt to create a map using either registered factory or dynamic library loading
     * @param s Reference to the ECS scene where the map will be created
     * @param name Name of the map to create
     * @return true if the map was successfully created, false otherwise
     *
     * This function first tries to find a registered factory for the given map name.
     * If no factory is found, it attempts to load a dynamic library (.so file) containing
     * the map implementation. The function searches for the library in several standard
     * locations relative to the current working directory.
     *
     * The dynamic library must export a function named "register_map_<name>" or "register_map"
     * that takes a Scene& parameter and registers the map in the scene.
     *
     * @note On non-Unix systems, only registered factories are supported
     * @warning The loaded libraries are kept in memory until program termination
     */
    inline bool tryCreateMapFromFactory(Scene &s, const std::string &name)
    {
        auto &m = getMapFactory();
        auto it = m.find(name);
        if (it != m.end()) {
            it->second(s);
            return true;
        }

#if defined(__unix__) || defined(__APPLE__)
        using namespace std::filesystem;

        std::vector<path> candidates;
        candidates.emplace_back("src/maps/" + name + "/libmap_" + name + ".so");
        candidates.emplace_back("maps/" + name + "/libmap_" + name + ".so");
        candidates.emplace_back("build/src/maps/" + name + "/libmap_" + name + ".so");
        candidates.emplace_back("./build/src/maps/" + name + "/libmap_" + name + ".so");

        std::error_code ec;
        const path cwd = current_path(ec);
        if (!ec) {
            candidates.emplace_back(cwd / "src" / "maps" / name / ("libmap_" + name + ".so"));
            candidates.emplace_back(cwd / "build" / "src" / "maps" / name / ("libmap_" + name + ".so"));
        }

        void *handle = nullptr;
        path usedPath;
        for (auto &soPath: candidates) {
            if (exists(soPath, ec)) {
                uintmax_t sz = file_size(soPath, ec);
                if (!ec)
                    std::cerr << "[ECS] candidate exists: " << soPath.string() << " (size=" << sz << ")\n";
                else
                    std::cerr << "[ECS] candidate exists: " << soPath.string() << " (size=? - error)\n";
            } else { std::cerr << "[ECS] candidate not found: " << soPath.string() << "\n"; }

            // clear any previous dlerror
            dlerror();
            handle = dlopen(soPath.c_str(), RTLD_NOW | RTLD_LOCAL);
            if (handle) {
                usedPath = soPath;
                break;
            } else {
                const char *err = dlerror();
                std::cerr << "[ECS] dlopen(" << soPath.string() << ") -> " << (err ? err : "(no error)") << "\n";
            }
        }

        if (!handle) {
            std::cerr << "[ECS] dlopen failed for candidates: ";
            for (auto &c: candidates)
                std::cerr << c.string() << " ";
            std::cerr << " - last error: " << (dlerror() ? dlerror() : "(no error)") << "\n";
            return false;
        }

        using RegFn = void (*)(Scene &);
        std::string sym = std::string("register_map_") + name;

        dlerror();
        RegFn reg = reinterpret_cast<RegFn>(dlsym(handle, sym.c_str()));
        if (!reg)
            reg = reinterpret_cast<RegFn>(dlsym(handle, "register_map"));
        if (!reg) {
            std::cerr << "[ECS] dlsym failed to find '" << sym << "' or 'register_map' in " << usedPath.string() <<
                    "\n";
            dlclose(handle);
            return false;
        }

        reg(s);

        getMapFactory()[name] = [reg](Scene &sc) { reg(sc); };

        static std::vector<void *> s_handles;
        s_handles.push_back(handle);

        return true;
#else
        return false;
#endif
    }
}

#endif //MAPFACTORY_HPP
