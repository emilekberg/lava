#pragma once
#include <lava/ecs/scene-view-archetype.hpp>
#include <lava/ecs/component.hpp>
#include "../components/mesh.hpp"
using lava::ecs::SceneViewArchetype;
using lava::ecs::SceneArchetype;
namespace pong
{
    void renderSystem(SceneArchetype& scene, const float deltaTime)
    {
        for(lava::ecs::EntityId id : SceneViewArchetype<lava::ecs::Transform, pong::Mesh>(scene))
        {
            auto transform = scene.getComponent<lava::ecs::Transform>(id);
            auto mesh = scene.getComponent<pong::Mesh>(id);

        }
    }
}