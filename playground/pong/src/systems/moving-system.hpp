#pragma once
#include <lava/ecs/scene-view-archetype.hpp>
#include <lava/ecs/component.hpp>
using lava::ecs::SceneViewArchetype;
using lava::ecs::SceneArchetype;
namespace pong
{
    void movingSystem(SceneArchetype& scene)
    {
        for(lava::ecs::EntityId id : SceneViewArchetype<lava::ecs::Transform, lava::ecs::Velocity>(scene))
        {
            auto transform = scene.getComponent<lava::ecs::Transform>(id);
            auto velocity = scene.getComponent<lava::ecs::Velocity>(id);
    
            transform->position = transform->position + velocity->velocity;
        }
    }
}