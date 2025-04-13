#pragma once
#include <bitset>
#include <vector>
#include <queue>
#include <array>
#include "lava/ecs/types.hpp"
#include "lava/memory/memory-arena.hpp"
namespace lava::ecs
{
    struct Archetype
    {
        Archetype(ComponentMask mask)
            : componentMask(mask), 
            componentOffsets(componentMask.count(), 0), 
            sizePerEntity(getTotalSize(mask)), 
            componentStorage(sizePerEntity * MAX_ENTITIES)
        {
            int numComponent = 0;
            for(size_t i = 0; i < mask.size(); i++)
            {
                if (mask.test(i))
                {
                    componentOffsets[numComponent++] = i;
                }
            }
        }

        constexpr size_t getTotalSize(ComponentMask mask)
        {
            size_t size = 0;
            for(size_t i = 0; i < mask.size(); i++)
            {
                if (mask.test(i))
                {
                    size += ComponentManager::getInstance()->getSize(i);
                }
            }
            return size;
        }

        bool isMask(ComponentMask mask)
        {
            return mask == (mask & componentMask);
        }

        void add(EntityId id, ComponentId componentId, void* data)
        {
            
        }

        void* get(EntityId id, ComponentId componentId)
        {

        }
        template <typename T>
        T* get(EntityId id)
        {
            ComponentId componentId = getComponentId<T>();
            size_t entityOffset = 0; // need constant time lookup
            size_t componentOffset = 0; // need constant time lookup
            void* pData = componentStorage.get(entityOffset * sizePerEntity + componentOffset);
            return static_cast<T*>(pData);
        }

        size_t sizePerEntity{0};
        memory::MemoryArena componentStorage;
        std::vector<ComponentId> componentOffsets;
        std::vector<EntityId> entities;
        std::queue<int> freeIndices;
        ComponentMask componentMask;
    };

}
