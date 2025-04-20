#pragma once
#include <unordered_set>
#include <vector>
#include <functional>
#include "./scene-archetype.hpp"

namespace lava::ecs
{
    struct World
    {
        World* addSystem(const std::function<void(SceneArchetype&)>& system)
        {
            systems.push_back(system);
            return this;
        }
        
        inline SceneArchetype& getActiveScene()
        {
            return _scene;
        }

        void update()
        {
            for(auto system : systems)
            {
                system(getActiveScene());
            }
        }

        std::vector<std::function<void(SceneArchetype&)>> systems;
        SceneArchetype _scene;
    };
}