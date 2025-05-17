#pragma once
#include <lava/ecs/scene.hpp>
#include <lava/ecs/scene-view.hpp>
#include "../components/local-transform.hpp"
#include "../components/world-transform.hpp"
#include "../components/hierarchy.hpp"
using namespace pong::components;
namespace pong::systems
{
    /**
     * Calculates the world transform given the local transform and the hierarchy.
     * Ideally should traverse from leaf nodes to root and utilize caching.
     *  */ 
    void transformSystem(Scene& scene, const float deltaTime)
    {
        for(lava::ecs::EntityId id : SceneView<WorldTransform, LocalTransform, Hierarchy>(scene))
        {
            auto world = scene.getComponent<WorldTransform>(id);
            auto local = scene.getComponent<LocalTransform>(id);
            auto hierarchy = scene.getComponent<Hierarchy>(id);
        }
    }
}