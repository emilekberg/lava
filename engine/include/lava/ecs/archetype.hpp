#pragma once
#include <bitset>
#include <vector>
#include <queue>
#include <array>
#include <unordered_map>
#include "lava/ecs/types.hpp"
#include "lava/memory/memory-arena.hpp"
namespace lava::ecs
{
    struct Archetype
    {
        Archetype(ComponentMask mask)
            : componentMask(mask), 
            componentOffsets(0, 0), 
            sizePerEntity(getTotalSize(mask)),
            componentStorage(sizePerEntity * MAX_ENTITIES)
        {
            int numComponent = 0;
            componentOffsets.resize(componentMask.count(), 0);
            for(size_t i = 0; i < mask.size(); i++)
            {
                if (mask.test(i))
                {
                    ComponentId componentId = static_cast<ComponentId>(i);
                    componentOffsets[numComponent++] = componentId;
                    componentLUT[componentId] = numComponent;
                }
            }
        }

        bool hasEntities()
        {
            return freeEntityIndices.size() != entities.size();
        }

        constexpr size_t getTotalSize(ComponentMask mask)
        {
            size_t size = 0;
            for(size_t i = 0; i < mask.size(); i++)
            {
                if (mask.test(i))
                {
                    size += ComponentManager::getInstance()->getSize(static_cast<ComponentId>(i));
                    offsets.push_back(size);
                }
            }
            return size;
        }

        bool hasMask(ComponentMask mask)
        {
            bool res = mask == (mask & componentMask);
            return res;
        }
        bool isMask(ComponentMask mask)
        {
            return componentMask == mask;
        }

        void add(EntityId id)
        {
            if(entityLUT.contains(id))
            {
                return;
            }
            if(!freeEntityIndices.empty())
            {
                size_t index = freeEntityIndices.back();
                freeEntityIndices.pop();
                entityLUT[id] = index;
                entities[index] = id;
            }
            else
            {
                entities.push_back(id);
                entityLUT[id] = entities.size()-1;
            }
        }

        void remove(EntityId id)
        {
            if(!entityLUT.contains(id))
            {
                return;
            }
            freeEntityIndices.push(entityLUT[id]);
            entities[entityLUT[id]] = INVALID_ENTITY;
            entityLUT.erase(id);
        }

        void* get(EntityId id, ComponentId componentId)
        {
            size_t entityOffset = entityLUT[id]; // need constant time lookup
            size_t componentOffset = componentLUT[componentId]; // need constant time lookup
            size_t memoryOffset = componentOffset > 0 ? offsets[componentOffset-1] : 0;
            void* pData = componentStorage.get(entityOffset * sizePerEntity + memoryOffset);
            return pData;
        }
        template <typename T>
        T* get(EntityId id)
        {
            ComponentId componentId = getComponentId<T>();
            return static_cast<T*>(get(id, componentId));
        }
        std::unordered_map<EntityId, size_t> entityLUT;
        std::unordered_map<ComponentId, size_t> componentLUT;

        std::vector<size_t> offsets;
        size_t sizePerEntity{0};
        std::vector<EntityId> entities;
        memory::MemoryArena componentStorage;
        std::vector<ComponentId> componentOffsets;
        std::queue<size_t> freeEntityIndices;
        ComponentMask componentMask;
    };

}
