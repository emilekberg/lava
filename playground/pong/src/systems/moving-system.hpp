#pragma once
#include <lava/ecs/scene-view.hpp>
#include <lava/ecs/component.hpp>
using lava::ecs::SceneView;
using lava::ecs::Scene;
namespace pong::systems
{
    void movingSystem(Scene& scene, const float deltaTime)
    {
        for(lava::ecs::EntityId id : SceneView<lava::ecs::Transform, lava::ecs::Velocity>(scene))
        {
            auto transform = scene.getComponent<lava::ecs::Transform>(id);
            auto velocity = scene.getComponent<lava::ecs::Velocity>(id);
    
            transform->position = transform->position + velocity->velocity;
        }
    }
}