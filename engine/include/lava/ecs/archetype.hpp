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
            componentStorage(sizePerEntity * 10)
        {
            int numComponent = 0;
            size_t totalSize = 0;
            componentOffsets.resize(componentMask.count(), 0);
            componentMemoryOffsets.push_back(0);
            for(size_t i = 0; i < mask.size(); i++)
            {
                if (mask.test(i))
                {
                    ComponentId componentId = static_cast<ComponentId>(i);
                    componentOffsets[numComponent++] = componentId;
                    componentIndexLUT[componentId] = numComponent;
                    totalSize += ComponentManager::getInstance()->getSize(static_cast<ComponentId>(i));
                    componentMemoryOffsets.push_back(totalSize);
                }
            }
            sizePerEntity = totalSize;
            componentStorage.resize(sizePerEntity * 10);
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
            if(entityIdLUT.contains(id))
            {
                return;
            }
            if(!freeEntityIndices.empty())
            {
                size_t index = freeEntityIndices.back();
                freeEntityIndices.pop();
                entityIdLUT[id] = index;
                entityIds[index] = id;
            }
            else
            {
                entityIds.push_back(id);
                entityIdLUT[id] = entityIds.size()-1;
                if(entityIds.size() * sizePerEntity > componentStorage.maxSize) {
                    componentStorage.resize(sizePerEntity * entityIds.size() * 2);
                }
            }
        }

        void remove(EntityId id)
        {
            if(!entityIdLUT.contains(id))
            {
                return;
            }
            freeEntityIndices.push(entityIdLUT[id]);
            entityIds[entityIdLUT[id]] = INVALID_ENTITY;
            entityIdLUT.erase(id);
        }

        void* get(EntityId id, ComponentId componentId)
        {
            size_t entityIndex = entityIdLUT[id]; // need constant time lookup
            size_t componentIndex = componentIndexLUT[componentId]; // need constant time lookup
            size_t componentMemoryOffset = componentMemoryOffsets[componentIndex];
            void* pData = componentStorage.get(entityIndex * sizePerEntity + componentMemoryOffset);
            return pData;
        }
        template <typename T>
        T* get(EntityId id)
        {
            ComponentId componentId = getComponentId<T>();
            return static_cast<T*>(get(id, componentId));
        }
        std::unordered_map<EntityId, size_t> entityIdLUT;
        std::unordered_map<ComponentId, size_t> componentIndexLUT;

        std::vector<size_t> componentMemoryOffsets;
        size_t sizePerEntity{0};
        std::vector<EntityId> entityIds;
        memory::MemoryArena componentStorage;
        std::vector<ComponentId> componentOffsets;
        std::queue<size_t> freeEntityIndices;
        ComponentMask componentMask;
    };

}
