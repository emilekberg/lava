#pragma once
#include <vector>
#include "types.hpp"
#include "archetype.hpp"
namespace lava::ecs
{
    struct SceneArchetype
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
            std::cout << "created entity" << "index(" << getEntityIndex(id) << ") version(" << getEntityVersion(id) << ")." << std::endl;
            return entities.back().id;
        }
        
        template <typename T>
        T* addComponent(EntityId id)
        {
            auto entityIndex = getEntityIndex(id);
            entities[entityIndex].mask.set(getComponentId<T>());
            
            size_t oldArchetypeId;
            size_t newArchetypeId;
            bool foundArchetype = false;
            for(size_t i = 0; i < archetypes.size(); i++)
            {
                if (archetypes[i]->isMask(entities[entityIndex].mask))
                {
                    newArchetypeId = i;
                    foundArchetype = true;
                    break;   
                }
            }
            if(!foundArchetype)
            {
                archetypes.push_back(std::make_unique<Archetype>(entities[entityIndex].mask));
                newArchetypeId = archetypes.size()-1;
            }
            archetypes[newArchetypeId]->add(id);
            
            if(entities[entityIndex].mask.count() > 1)
            {
                oldArchetypeId = entitiesArchetypeLUT[entityIndex]; 
                for (ComponentId componentId : (archetypes[oldArchetypeId])->componentOffsets)
                {
                    // copy component data to new archetype
                    auto pSrcData = archetypes[oldArchetypeId]->get(id, componentId);
                    auto pDstData = archetypes[newArchetypeId]->get(id, componentId);
                    auto size = ComponentManager::getInstance()->getSize(componentId);
                    memcpy_s(pDstData, size, pSrcData, size);
                }
                archetypes[oldArchetypeId]->remove(id);
            }
            entitiesArchetypeLUT[entityIndex] = newArchetypeId;

            T* pComponent = new (archetypes[newArchetypeId]->get<T>(id)) T(); 
            return pComponent;
        }

        template <typename T>
        T* getComponent(EntityId id)
        {
            return archetypes[entitiesArchetypeLUT[getEntityIndex(id)]]->get<T>(id);
        }

        template <typename T>
        void removeComponent(EntityId id)
        {

        }

        void destroyEntity(EntityId id)
        {

        }

        struct EntityDescriptor
        {
            EntityId id;
            ComponentMask mask;
        };
        std::vector<std::unique_ptr<Archetype>> archetypes;
        std::array<size_t, MAX_ENTITIES> entitiesArchetypeLUT;
        std::vector<EntityDescriptor> entities;
        std::queue<EntityIndex> freeEntityIndices;

    };
}