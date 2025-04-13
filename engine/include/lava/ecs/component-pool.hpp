#pragma once
#include "lava/ecs/types.hpp"
namespace lava::ecs
{
    struct ComponentPool
    {
        ComponentPool(size_t elementSize)
        {
            _elementSize = elementSize;
            pData = new char[elementSize * MAX_ENTITIES];
        }
        ~ComponentPool()
        {
            delete[] pData;
        }


        inline void* get(EntityIndex index)
        {
            return pData + index * _elementSize;
        }

        char* pData{nullptr};
        size_t _elementSize{0};
    };
}