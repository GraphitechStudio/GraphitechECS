#ifndef ECS_HPP
    #define ECS_HPP

// Header principal ECS - inclut tous les modules

// Modern ECS system (namespace ECS::)
#include <Graphitech/ECS/Core/Types.hpp>
#include <Graphitech/ECS/Component.hpp>
#include <Graphitech/ECS/Core/Scene.hpp>
#include <Graphitech/ECS/MapLoader.hpp>
#include <Graphitech/ECS/Factories/EntityFactory.hpp>

// Legacy ECS system (namespace ECS::)
#include <Graphitech/ECS/Core/IComponent.hpp>
#include <Graphitech/ECS/Core/IEntity.hpp>
#include <Graphitech/ECS/Core/AComponent.hpp>
#include <Graphitech/ECS/Core/AEntity.hpp>

// Factory systems
#include <Graphitech/ECS/Factories/MapFactory.hpp>

// Component implementations
#include <Graphitech/ECS/Components/PositionComponent.hpp>
#include <Graphitech/ECS/Components/RotationComponent.hpp>
#include <Graphitech/ECS/Components/ScaleComponent.hpp>
#include <Graphitech/ECS/Components/CollisionComponent.hpp>
#include <Graphitech/ECS/Components/VelocityComponent.hpp>
#include <Graphitech/ECS/Components/TTLComponent.hpp>
#include <Graphitech/ECS/Components/RenderPrimitives.hpp>

#endif
