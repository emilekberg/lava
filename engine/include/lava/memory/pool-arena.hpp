#pragma once
#include "memory-arena.hpp"
namespace lava::memory
{
    struct MemoryPool
    {
        MemoryPool(size_t elementSize, size_t maxElements) : 
            elementSize(elementSize),
            arena(elementSize * maxElements)
        {
        }
        inline void* get(size_t index)
        {
            return arena.get(elementSize);
        }
        size_t elementSize{0};
        MemoryArena arena;
    };
}