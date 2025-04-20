#pragma once
#include <bitset>
#include <array>
// https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html


extern int s_componentCounter;
namespace lava::ecs
{

    constexpr int MAX_COMPONENTS = 32;
    constexpr int MAX_ENTITIES = 64;
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
        
        constexpr inline ComponentId getNextComponentId()
        {
            return componentIdCounter++;
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
        ComponentId componentIdCounter;
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


    // can this be pure constexpr? would be nice with static assert on the id...
    static ComponentId s_componentIdCounter = 0;
    template <typename T>
    constexpr inline ComponentId getComponentId()
    {
        static ComponentId s_componentId = s_componentIdCounter++;
        ComponentManager::getInstance()->defineSize<T>(s_componentId);
        // s_componentId is not compiletime... so this assert won't work ;(
        // static_assert(s_componentId < MAX_COMPONENTS, "number of declared components is greater than MAX_COMPONENTS");
        return s_componentId; 
    }

    template <typename T>
    constexpr inline size_t defineComponentSize(size_t size = 0)
    {
        static size_t realSize = 0;
        if(size > 0) {
            realSize = size;
        }
        return realSize;
    }

    // can i do this compiletime?
    constexpr inline size_t getComponentSize(ComponentId id)
    {
        return ComponentManager::getInstance()->getSize(id);
    }

    #define INVALID_ENTITY createEntityId(EntityIndex(-1), 0);
}