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
            componentStorage(sizePerEntity * MAX_ENTITIES)
        {
            int numComponent = 0;
            size_t totalSize = 0;
            componentOffsets.resize(componentMask.count(), 0);
            for(size_t i = 0; i < mask.size(); i++)
            {
                if (mask.test(i))
                {
                    ComponentId componentId = static_cast<ComponentId>(i);
                    componentOffsets[numComponent++] = componentId;
                    componentLUT[componentId] = numComponent;
                    totalSize += ComponentManager::getInstance()->getSize(static_cast<ComponentId>(i));
                    offsets.push_back(totalSize);
                }
            }
            sizePerEntity = totalSize;
        }

        bool hasEntities()
        {
            return freeEntityIndices.size() != entityIds.size();
        }

        bool hasMask(ComponentMask mask)
        {
            return mask == (mask & componentMask);
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
                entityIds[index] = id;
            }
            else
            {
                entityIds.push_back(id);
                entityLUT[id] = entityIds.size()-1;
            }
        }

        void remove(EntityId id)
        {
            if(!entityLUT.contains(id))
            {
                return;
            }
            freeEntityIndices.push(entityLUT[id]);
            entityIds[entityLUT[id]] = INVALID_ENTITY;
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
        std::vector<EntityId> entityIds;
        memory::MemoryArena componentStorage;
        std::vector<ComponentId> componentOffsets;
        std::queue<size_t> freeEntityIndices;
        ComponentMask componentMask;
    };

}
