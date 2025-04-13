#pragma once
#include <bitset>
#include <array>
// https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html


extern int s_componentCounter;
namespace lava::ecs
{

    const int MAX_COMPONENTS = 1024;
    const int MAX_ENTITIES = 1024;
    using EntityIndex = unsigned int;
    using EntityVersion = unsigned int;
    using EntityId = unsigned long long;
    using ComponentId = int;
    using ComponentMask = std::bitset<MAX_COMPONENTS>;


    struct ComponentManager
    {
        static ComponentManager* getInstance() {
            static ComponentManager* instance = new ComponentManager(); 
            return instance;
        }

        template <typename T>
        constexpr inline void defineSize(ComponentId id)
        {
            sizes[id] = sizeof(T);
        }

        constexpr inline size_t getSize(ComponentId id) const
        {
            return sizes[id];
        }
        std::array<size_t, MAX_COMPONENTS> sizes;
    };
    
    inline EntityId createEntityId(EntityIndex index, EntityVersion version)
    {
        return ((EntityId)index << 32) | ((EntityId)version);
    }

    inline EntityIndex getEntityIndex(EntityId id)
    {
        return id >> 32;
    }

    inline EntityVersion getEntityVersion(EntityId id)
    {
        return (EntityVersion)id;
    }

    inline bool isEntityValid(EntityId id)
    {
        return (id >> 32) != EntityIndex(-1);
    }

    template <typename T>
    constexpr inline ComponentId getComponentId()
    {
        static int s_componentId = s_componentCounter++;
        ComponentManager::getInstance()->defineSize<T>(s_componentId);
        return s_componentId; 
    }

    #define INVALID_ENTITY createEntityId(EntityIndex(-1), 0);





}