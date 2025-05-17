#pragma once
#include <lava/ecs/component.hpp>
#include "../components/mesh.hpp"
using lava::ecs::Scene;
using lava::ecs::SceneView;
using namespace pong::components;
namespace pong::systems
{
    struct RenderSystem
    {
        void tick(Scene &scene, const float deltaTime)
        {
            // in here or in a camera system
            // vkRenderer->setUBO(camera-getViewModel)
            for (auto id : SceneView<lava::ecs::Transform, Mesh>(scene))
            {
                auto transform = scene.getComponent<lava::ecs::Transform>(id);
                auto mesh = scene.getComponent<Mesh>(id);

                // get mesh and material from resource manager
                // get transform
                // push data to renderqueue

                // cmdBuffer = _vkRenderer->begin
                // _vkRenderer
                // _vkRenderer->render(cmdBuffer, mesh->model, mesh->material, transform);
                // vk_renderer->end(cmdBuffer)
            }
        }
    };
    void renderSystem(Scene &scene, const float deltaTime)
    {
    }
}