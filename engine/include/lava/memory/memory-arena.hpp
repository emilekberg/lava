namespace lava::memory {

    struct MemoryArena
    {
        MemoryArena(size_t reservedSize) : 
            maxSize(reservedSize), 
            pBuffer(new char[maxSize])
        {
            std::cout << "created memory arena" << std::endl;
        }

        ~MemoryArena()
        {
            std::cout << "destroyed memory arena" << std::endl;
            delete[] pBuffer;
        }

        void resize(size_t newMaxSize)
        {
            auto pOldBuffer = pBuffer;
            auto pNewBuffer = new char[newMaxSize];
            memcpy_s(pNewBuffer, maxSize, pOldBuffer, maxSize);
            pBuffer = pNewBuffer;
            maxSize = newMaxSize;
            delete pOldBuffer;
        }

        size_t getFreeSize()
        {
            return maxSize - written;
        }

        size_t getUsedSize()
        {
            return written;   
        }

        inline void* alloc(size_t size)
        {
            void* pAllocatedData = pBuffer + written;
            written += size;
            return pAllocatedData;
        }

        inline void* get(size_t offset)
        {
            return pBuffer + offset;
        }

        size_t written; 
        size_t maxSize;
        char* pBuffer{nullptr};
    };
}