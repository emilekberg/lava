#include <gtest/gtest.h>
#include <lava/ecs/scene-archetype.hpp>

int s_componentCounter = 0;
TEST(ECS_Scene, CreateEntity)
{
    lava::ecs::SceneArchetype scene;

    auto id = scene.createEntity();
    EXPECT_EQ(lava::ecs::getEntityIndex(id), 0);
    EXPECT_EQ(lava::ecs::getEntityVersion(id), 0);

    auto id2 = scene.createEntity();
    EXPECT_EQ(lava::ecs::getEntityIndex(id2), 1);
    EXPECT_EQ(lava::ecs::getEntityVersion(id2), 0);
}

TEST(ECS_Scene, CreateEntity_Regenerate)
{
    lava::ecs::SceneArchetype scene;

    auto id = scene.createEntity();
    EXPECT_EQ(lava::ecs::getEntityIndex(id), 0);
    EXPECT_EQ(lava::ecs::getEntityVersion(id), 0);

    scene.destroyEntity(id);

    auto id2 = scene.createEntity();
    EXPECT_EQ(lava::ecs::getEntityIndex(id2), 0);
    EXPECT_EQ(lava::ecs::getEntityVersion(id2), 1);
}

TEST(ECS_Scene, AddComponent)
{
    struct SomeComponent{ int value = 5; };
    lava::ecs::SceneArchetype scene;

    auto id = scene.createEntity();
    auto component = scene.addComponent<SomeComponent>(id);

    EXPECT_EQ(component->value, 5);
    EXPECT_EQ(scene.archetypes.size(), 1);
}

TEST(ECS_Scene, Add2Components)
{
    struct SomeComponent{ int value = 3; };
    struct AnotherComponent{int value = 10; };
    lava::ecs::SceneArchetype scene;

    auto id = scene.createEntity();
    auto someComponent = scene.addComponent<SomeComponent>(id);
    auto anotherComponent = scene.addComponent<AnotherComponent>(id);

    EXPECT_EQ(someComponent->value, 3);
    EXPECT_EQ(anotherComponent->value, 10);
    EXPECT_EQ(scene.archetypes.size(), 2);

}