#pragma once
#include "lava/ecs/entity.hpp"
#include <memory>
#include <array>
#include <vector>
#include <queue>
#include "lava/ecs/types.hpp"
#include "lava/ecs/component-pool.hpp"

namespace lava::ecs
{
    struct Scene
    {
        EntityId createEntity()
        {
            if(!freeEntityIndices.empty())
            {
                EntityIndex newIndex = freeEntityIndices.back();
                freeEntityIndices.pop();
                EntityId newId = createEntityId(newIndex, getEntityVersion(entities[newIndex].id));
                entities[newIndex].id = newId;
                return entities[newIndex].id;
            }
            auto id = createEntityId(static_cast<unsigned int>(entities.size()), 0);
            entities.push_back({id, ComponentMask()});
            return entities.back().id;
        }
        
        template <typename T>
        T* addComponent(EntityId id)
        {
            ComponentId componentId = getComponentId<T>();
            if (componentPools.size() <= componentId)
            {
                componentPools.resize(componentId + 1, nullptr);
            }
            if (componentPools[componentId] == nullptr)
            {
                componentPools[componentId] = new ComponentPool(sizeof(T));
            }
            auto entityIndex = getEntityIndex(id);
            T* pComponent = new (componentPools[componentId]->get(entityIndex)) T();

            entities[entityIndex].mask.set(componentId);
            return pComponent;
        }

        template <typename T>
        T* getComponent(EntityId id)
        {
            ComponentId componentId = getComponentId<T>();
            if(!entities[getEntityIndex(id)].mask.test(componentId))
                return nullptr;
            
            T* pComponent = static_cast<T*>(componentPools[componentId]->get(getEntityIndex(id)));
            return pComponent;

        }

        template <typename T>
        void removeComponent(EntityId id)
        {
            // ensures you're not accessing a removed entity
            if(entities[getEntityIndex(id)].id != id)
            {
                return;
            }
            int componentId = getComponentId<T>();
            entities[getEntityIndex(id)].mask.reset(componentId);
        }

        void destroyEntity(EntityId id)
        {
            EntityId newId = createEntityId(EntityIndex(-1), getEntityVersion(id) + 1 );
            entities[getEntityIndex(id)].id = newId;
            entities[getEntityIndex(id)].mask.reset();
            freeEntityIndices.push(getEntityIndex(id));
        }

        struct EntityDescriptor
        {
            EntityId id;
            ComponentMask mask;
        };
        std::vector<EntityDescriptor> entities;
        std::queue<EntityIndex> freeEntityIndices;
        std::vector<ComponentPool*> componentPools; 
        size_t componentCounter;
    };

}