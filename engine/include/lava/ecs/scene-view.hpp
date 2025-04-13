#pragma once
#include "lava/ecs/types.hpp"
#include "lava/ecs/scene.hpp"
namespace lava::ecs
{
    template<typename... ComponentTypes>
    struct SceneView
    {
        SceneView(Scene& scene) : pScene(&scene)
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
            Iterator(Scene* scene, EntityIndex index, ComponentMask mask, bool all) 
                : pScene(scene), index(index), mask(mask), all(all)
            {

            }
            EntityId operator*() const
            {
                return pScene->entities[index].id;
            }

            bool operator==(const Iterator& other) const
            {
                return index == other.index || index == pScene->entities.size();
            }

            bool operator!=(const Iterator& other) const 
            {
                return index != other.index && index != pScene->entities.size();
            } 

            bool isValidIndex()
            {
                return isEntityValid(pScene->entities[index].id) && (all || mask == (mask & pScene->entities[index].mask));
            }

            Iterator& operator++()
            {
                do
                {
                    index++;
                } while (index < pScene->entities.size()  && !isValidIndex());
                return *this;
            }

            EntityIndex index;
            Scene* pScene;
            ComponentMask mask;
            bool all{false};
        };

        const Iterator begin() const
        {
            int firstIndex = 0;
            while (firstIndex < pScene->entities.size() && (componentMask != (componentMask & pScene->entities[firstIndex].mask) || !isEntityValid(pScene->entities[firstIndex].id)))
            {
                firstIndex++;
            }
            return Iterator(pScene, firstIndex, componentMask, all);
        }

        const Iterator end() const
        {
            return Iterator(pScene, EntityIndex(pScene->entities.size()), componentMask, all);
        }


        Scene* pScene{nullptr};
        ComponentMask componentMask;
        bool all{false};
    };
}