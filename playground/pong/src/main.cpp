#include <cstdio>
#include <lava/app.hpp>
#include <GLFW/glfw3.h>
#include "lava/resourceloader.hpp"
#include <iostream>

#include <lava/ecs/scene.hpp>
#include <lava/ecs/component.hpp>
#include <lava/ecs/scene-view.hpp>

int s_componentCounter = 0;
int main(int argc, char *argv[])
{
    fprintf(stdout, argv[0]);

    lava::ecs::Scene scene;

    lava::ecs::EntityId triangle = scene.createEntity();
    scene.addComponent<lava::ecs::Transform>(triangle);
    scene.addComponent<lava::ecs::Shape>(triangle);

    scene.destroyEntity(triangle);
    lava::ecs::EntityId rectangle = scene.createEntity();
    scene.addComponent<lava::ecs::Transform>(rectangle);
    scene.addComponent<lava::ecs::Shape>(rectangle); 
    
    // demo system
    for(lava::ecs::EntityId entity : lava::ecs::SceneView<lava::ecs::Transform, lava::ecs::Shape>(scene))
    {
        auto worldTransform = scene.getComponent<lava::ecs::Transform>(entity);
        auto shape = scene.getComponent<lava::ecs::Shape>(entity);

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