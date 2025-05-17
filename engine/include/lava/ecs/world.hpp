#pragma once
#include <unordered_set>
#include <vector>
#include <functional>
#include <chrono>
#include "./scene.hpp"

namespace lava::ecs
{
    using chrono_ms = std::chrono::duration<float, std::milli>;
    using EcsSystemCallback = std::function<void(Scene&, const float)>;
    struct World
    {
        World* addSystem(const EcsSystemCallback& system)
        {
            systems.push_back(system);
            return this;
        }
        
        inline Scene& getActiveScene()
        {
            return _scene;
        }

        void update()
        {
            auto now = timer.now();
            float deltaTime = std::chrono::duration_cast<chrono_ms>(now-lastFrame).count();
            lastFrame = now;
            for(auto system : systems)
            {
                system(getActiveScene(), deltaTime);
            }
        }

        std::vector<EcsSystemCallback> systems;
        Scene _scene;
        std::chrono::high_resolution_clock timer;
        std::chrono::steady_clock::time_point lastFrame;
    };
}