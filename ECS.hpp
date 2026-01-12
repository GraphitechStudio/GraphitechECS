#ifndef ECS_HPP
    #define ECS_HPP

// Header principal ECS - inclut tous les modules

// Modern ECS system (namespace ECS::)
#include "Core/Types.hpp"
#include "Component.hpp"
#include "Core/Scene.hpp"
#include "MapLoader.hpp"
#include "Factories/EntityFactory.hpp"

// Legacy ECS system (namespace ECS::)
#include <Graphitech/ECS/Core/IComponent.hpp>
#include <Graphitech/ECS/Core/IEntity.hpp>
#include <Graphitech/ECS/Core/AComponent.hpp>
#include <Graphitech/ECS/Core/AEntity.hpp>

// Factory systems
#include "Factories/MapFactory.hpp"

// Component implementations
#include "Components/PositionComponent.hpp"
#include "Components/RotationComponent.hpp"
#include "Components/ScaleComponent.hpp"
#include "Components/CollisionComponent.hpp"
#include "Components/VelocityComponent.hpp"
#include "Components/TTLComponent.hpp"
#include "Components/RenderPrimitives.hpp"

#endif
