#include <cstdio>
#include <lava/app.hpp>
#include <GLFW/glfw3.h>
#include "lava/resourceloader.hpp"
#include <iostream>

#include <lava/ecs/scene.hpp>
#include <lava/ecs/component.hpp>
#include <lava/ecs/scene-view.hpp>
#include "lava/ecs/scene-archetype.hpp"
#include "lava/ecs/scene-view-archetype.hpp"

int s_componentCounter = 0;
int main(int argc, char *argv[])
{
    fprintf(stdout, argv[0]);

    lava::ecs::Scene scene;
    lava::ecs::SceneArchetype sceneArchetype;
    
    auto id = sceneArchetype.createEntity();
    auto transform1 = sceneArchetype.addComponent<lava::ecs::Transform>(id);
    auto velocity1 = sceneArchetype.addComponent<lava::ecs::Velocity>(id);
    velocity1->velocity.x = 10;
   
    auto id2 = sceneArchetype.createEntity();
    sceneArchetype.addComponent<lava::ecs::Transform>(id2);

    /*
    auto id3 = sceneArchetype.createEntity();
    sceneArchetype.addComponent<lava::ecs::Transform>(id3);
    auto velocity2 = sceneArchetype.addComponent<lava::ecs::Velocity>(id3);

    auto id4 = sceneArchetype.createEntity();
    sceneArchetype.addComponent<lava::ecs::Transform>(id4);

    velocity1->velocity.x = 1;
    velocity2->velocity.y = 1;
    */
    std::cout << "PRINTING VALUES" << std::endl;
    for(lava::ecs::EntityId id : lava::ecs::SceneViewArchetype<lava::ecs::Transform>(sceneArchetype))
    {
        auto transform = sceneArchetype.getComponent<lava::ecs::Transform>(id);
        std::cout << "entity id: Index(" << lava::ecs::getEntityIndex(id) << ") Version(" << lava::ecs::getEntityVersion(id) << ")" << std::endl;
        std::cout << "- transform: pos(" << transform->position.x << "," << transform->position.y << "," << transform->position.z << ")" << std::endl;
    }

    std::cout << "CALCULATING VALUES" << std::endl;
    for(lava::ecs::EntityId id : lava::ecs::SceneViewArchetype<lava::ecs::Transform, lava::ecs::Velocity>(sceneArchetype))
    {
        auto transform = sceneArchetype.getComponent<lava::ecs::Transform>(id);
        auto velocity = sceneArchetype.getComponent<lava::ecs::Velocity>(id);

        transform->position = transform->position + velocity->velocity;
    }

    std::cout << "PRINTING VALUES" << std::endl;
    for(lava::ecs::EntityId id : lava::ecs::SceneViewArchetype<lava::ecs::Transform>(sceneArchetype))
    {
        auto transform = sceneArchetype.getComponent<lava::ecs::Transform>(id);
        std::cout << "entity id: Index(" << lava::ecs::getEntityIndex(id) << ") Version(" << lava::ecs::getEntityVersion(id) << ")" << std::endl;
        std::cout << "- transform: pos(" << transform->position.x << "," << transform->position.y << "," << transform->position.z << ")" << std::endl;
    }
    

    try
    {
        glfwInit();
        auto app = lava::App();
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