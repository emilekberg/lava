#pragma once
#include <lava/ecs/scene-view.hpp>
#include "../components/input.hpp"
#include "../components/player.hpp"

using lava::ecs::SceneView;
using lava::ecs::SceneArchetype;
using namespace pong::components;
namespace pong::systems
{
    void inputSystem(Scene& scene)
    {
        for(lava::ecs::EntityId id : SceneView<Input>(scene))
        {
            auto input = scene.getComponent<Input>(id);
        }
    }
}