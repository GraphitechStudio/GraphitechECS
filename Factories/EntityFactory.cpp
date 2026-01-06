#include <Factories/EntityFactory.hpp>
#include <iostream>

namespace ECS {

bool EntityFactory::registerFactory(const std::string& name, FactoryFn fn) {
    auto it = factories.find(name);
    if (it != factories.end()) {
        std::cerr << "[EntityFactory] Factory '" << name << "' already registered\n";
        return false;
    }
    factories[name] = std::move(fn);
    return true;
}

bool EntityFactory::unregisterFactory(const std::string& name) {
    return factories.erase(name) > 0;
}

bool EntityFactory::hasFactory(const std::string& name) const {
    return factories.find(name) != factories.end();
}

const EntityFactory::FactoryFn* EntityFactory::getFactory(const std::string& name) const {
    auto it = factories.find(name);
    if (it == factories.end()) {
        return nullptr;
    }
    return &it->second;
}

std::vector<std::string> EntityFactory::listFactories() const {
    std::vector<std::string> out;
    out.reserve(factories.size());
    for (auto &p : factories) out.push_back(p.first);
    return out;
}

EntityFactory& getEntityFactory(){
    static EntityFactory inst;
    return inst;
}

} // namespace ECS

