#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <queue>
#include "lava/memory/memory-arena.hpp"
namespace lava
{
    using AssetTypeId = size_t;

    struct ResourceContainer
    {
        ResourceContainer(size_t elementSize) : 
            _elementSize(elementSize),
            _memoryArena(_elementSize * 10),
            _count(0)
        {

        }

        size_t getNextIndex()
        {
            if (_freeIndices.size() > 0)
            {
                size_t index = _freeIndices.back();
                _freeIndices.pop();
                return index;
            }
            _memoryArena.alloc(_elementSize);
            size_t index = _count++;;
            return index;
        }


        template <typename T>    
        void add(const std::string& name, T* pData)
        {
           if (_assetOffsets.contains(name))
                return;
            size_t index = getNextIndex();
            _assetOffsets[name] = index;
            auto pStoredData = _memoryArena.get(index * _elementSize);

            memcpy_s(pStoredData, _elementSize, pData, _elementSize);
        }

        template <typename T>
        T* get(const std::string& name)
        {
            if (!_assetOffsets.contains(name))
                return nullptr;
            
            size_t offset = _assetOffsets[name];
            void* pData = _memoryArena.get(name);
            return static_cast<T*>(pData);
        }

        size_t _count;
        size_t _elementSize;
        memory::MemoryArena _memoryArena;
        std::unordered_map<std::string, size_t> _assetOffsets;
        std::queue<size_t> _freeIndices;
    };


    class ResourceManager
    {
    public:
        template <typename T>    
        static void add(const std::string& name, T* data)
        {
            size_t assetTypeId = 0;
            if (!_resourceContainers.contains(assetTypeId)) 
            {
                _resourceContainers.emplace();
            }
            _resourceContainers[assetTypeId].add(name, data);
        }
        
        template <typename T>
        static T* get(const std::string& name)
        {
            // get type from T, similar to components does. 
            size_t assetTypeId = 0;
            if (!_resourceContainers.contains(assetTypeId)) 
                return nullptr;

            return _resourceContainers[assetTypeId].get<T>(name);
        }

    private:
        static std::unordered_map<AssetTypeId, ResourceContainer> _resourceContainers;
    };



}