#pragma once
#include "lava/ecs/types.hpp"
#include "lava/ecs/scene-archetype.hpp"
namespace lava::ecs
{
    template<typename... ComponentTypes>
    struct SceneViewArchetype
    {
        SceneViewArchetype(SceneArchetype& scene) : pScene(&scene)
        {
            if (sizeof...(ComponentTypes) == 0)
            {
                all = true;
            }
            else
            {
                ComponentId componentIds[] = {0, getComponentId<ComponentTypes>()...};
                for(int i = 1; i < (sizeof...(ComponentTypes)+1); i++)
                {
                    componentMask.set(componentIds[i]);
                }
            }
        }
        struct Iterator
        {
            Iterator(SceneArchetype* scene, size_t archetypeIndex, size_t entityIndex, ComponentMask mask, bool all) 
                : pScene(scene), archetypeIndex(archetypeIndex), entityIndex(entityIndex), mask(mask), all(all)
            {

            }
            EntityId operator*() const
            {
                return pScene->archetypes[archetypeIndex]->entities[entityIndex];
            }

            bool operator==(const Iterator& other) const
            {
                return archetypeIndex == other.archetypeIndex && entityIndex == other.entityIndex;
            }

            bool operator!=(const Iterator& other) const 
            {
                return archetypeIndex != other.archetypeIndex || entityIndex != other.entityIndex;
            } 

            bool isValidEntityIndex()
            {
                return isEntityValid(pScene->archetypes[archetypeIndex]->entities[entityIndex]) && (all || pScene->archetypes[archetypeIndex]->hasMask(mask));
            }

            bool isValidArchetypeIndex()
            {
                return pScene->archetypes[archetypeIndex]->hasMask(mask) && pScene->archetypes[archetypeIndex]->hasEntities();
            }

            Iterator& operator++()
            {
                do
                {
                    do
                    {
                        entityIndex++;
                        if(entityIndex < pScene->archetypes[archetypeIndex]->entities.size() && isValidEntityIndex())
                        {
                            return *this;
                        }
                    } while (entityIndex == pScene->archetypes[archetypeIndex]->entities.size());
                    
                    entityIndex = 0;
                    do
                    {
                        archetypeIndex++;
                    } while (archetypeIndex < pScene->archetypes.size() && !isValidArchetypeIndex());
                    if(archetypeIndex < pScene->archetypes.size() && isValidEntityIndex())
                    {
                        return *this;
                    }
                } while (archetypeIndex < pScene->archetypes.size() && entityIndex < pScene->archetypes[archetypeIndex]->entities.size());
                return *this;
            }

            size_t entityIndex;
            size_t archetypeIndex;
            SceneArchetype* pScene;
            ComponentMask mask;
            bool all{false};
        };

        const Iterator begin() const
        {
            size_t firstArchetypeIndex = 0;
            size_t firstEntityIndex = 0;
            bool wasArchetypeValid = true;
            do
            {
                firstEntityIndex = 0;
                while (firstArchetypeIndex < pScene->archetypes.size() && (!pScene->archetypes[firstArchetypeIndex]->hasMask(componentMask) || !pScene->archetypes[firstArchetypeIndex]->hasEntities()))
                {
                    firstArchetypeIndex++;
                }
                wasArchetypeValid = true;
                while (firstEntityIndex < pScene->archetypes[firstArchetypeIndex]->entities.size() && !isEntityValid(pScene->archetypes[firstArchetypeIndex]->entities[firstEntityIndex]))
                {
                    firstEntityIndex++;
                }
                if(firstEntityIndex == pScene->archetypes[firstArchetypeIndex]->entities.size())
                {
                    firstArchetypeIndex++;
                    wasArchetypeValid = false;
                    if(firstArchetypeIndex == pScene->archetypes.size())
                    {
                        return end();
                    }
                }

            } while(!wasArchetypeValid);

            return Iterator(pScene, firstArchetypeIndex, firstEntityIndex, componentMask, all);
        }

        const Iterator end() const
        {
            size_t lastArchetypeIndex = pScene->archetypes.size();
            return Iterator(pScene, lastArchetypeIndex, 0, componentMask, all);
        }


        SceneArchetype* pScene{nullptr};
        ComponentMask componentMask;
        bool all{false};
    };
}