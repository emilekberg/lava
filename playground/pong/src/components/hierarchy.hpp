#pragma once
#include <lava/ecs/types.hpp>
#include <vector>
namespace pong::components
{
    struct Hierarchy
    {
        lava::ecs::EntityId parent;
        std::vector<lava::ecs::EntityId> children;       
    };
}