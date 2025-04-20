#pragma once
#include <bitset>
#include <array>
// https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html


extern int s_componentCounter;
namespace lava::ecs
{

    constexpr size_t MAX_COMPONENTS = 32;
    constexpr size_t MAX_ENTITIES = 64;
    using EntityIndex = unsigned int;
    using EntityVersion = unsigned int;
    using EntityId = unsigned long long;
    using ComponentId = size_t;
    using ComponentMask = std::bitset<MAX_COMPONENTS>;

    class ComponentManager
    {
    public:
        template <typename T>
        static ComponentId getNextComponentId()
        {
            static const ComponentId id = componentIdCounter++;
            // static_assert(id < MAX_COMPONENTS, "Initialized too many components");
            
            sizes[id] = sizeof(T);

            return id;
        } 

        static size_t getSize(ComponentId id) 
        {
            return sizes[id];
        }
    private:
        inline static std::array<size_t, MAX_COMPONENTS> sizes;
        inline static ComponentId componentIdCounter = 0;
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
        return getEntityIndex(id) != EntityIndex(-1);
    }


    // can this be pure constexpr? would be nice with static assert on the id...
    static ComponentId s_componentIdCounter = 0;
    template <typename T>
    static inline ComponentId getComponentId()
    {
        return ComponentManager::getNextComponentId<T>();
        /*
        static ComponentId s_componentId = s_componentIdCounter++;
        ComponentManager::getInstance()->defineSize<T>(s_componentId);
        // s_componentId is not compiletime... so this assert won't work ;(
        // static_assert(s_componentId < MAX_COMPONENTS, "number of declared components is greater than MAX_COMPONENTS");
        return s_componentId; 
        */
    }

    // can i do this compiletime?
    static inline size_t getComponentSize(ComponentId id)
    {
        return ComponentManager::getSize(id);
    }

    #define INVALID_ENTITY createEntityId(EntityIndex(-1), 0);
}