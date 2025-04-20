#include <cstdio>
#include <lava/app.hpp>
#include <GLFW/glfw3.h>
#include "lava/resourceloader.hpp"
#include <iostream>

#include <lava/ecs/scene.hpp>
#include <lava/ecs/component.hpp>
#include <lava/ecs/scene-view.hpp>
#include <lava/ecs/scene-archetype.hpp>
#include <lava/ecs/scene-view-archetype.hpp>
#include "systems/moving-system.hpp"
#include "systems/render-system.hpp"
int main(int argc, char *argv[])
{
    fprintf(stdout, argv[0]);


    try
    {
        glfwInit();
        auto world = std::make_unique<lava::ecs::World>();
        auto app = lava::App();
        world
            ->addSystem(pong::movingSystem)
            ->addSystem(pong::renderSystem);

        lava::ecs::SceneArchetype& scene = world->getActiveScene();

        auto id = scene.createEntity();
        auto transform1 = scene.addComponent<lava::ecs::Transform>(id);
        auto velocity1 = scene.addComponent<lava::ecs::Velocity>(id);
        velocity1->velocity.x = 10;
    
        auto id2 = scene.createEntity();
        scene.addComponent<lava::ecs::Transform>(id2);

        auto id3 = scene.createEntity();
        scene.addComponent<lava::ecs::Transform>(id3);
        auto velocity2 = scene.addComponent<lava::ecs::Velocity>(id3);
        
        app.setWorld(std::move(world));
        app.run();
    }
    catch (const std::exception &e)
    {
        fprintf(stderr, "Error: %s\n", e.what());
        std::cin.get();
    }
    glfwTerminate();
    return 0;
}