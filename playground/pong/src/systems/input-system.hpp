#pragma once
#include <lava/ecs/scene-view-archetype.hpp>
#include "../components/input.hpp"
using lava::ecs::SceneViewArchetype;
using lava::ecs::SceneArchetype;
namespace pong
{
    void inputSystem(SceneArchetype& scene)
    {
        for(lava::ecs::EntityId id : SceneViewArchetype<pong::Input>(scene))
        {
            auto input = scene.getComponent<pong::Input>(id);
        }
    }
}