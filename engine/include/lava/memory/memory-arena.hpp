namespace lava::memory {

    struct MemoryArena
    {
        MemoryArena(size_t reservedSize) : 
            totalSize(reservedSize), 
            pData(new char[totalSize])
        {
        }

        ~MemoryArena()
        {
            delete[] pData;
        }

        inline void* alloc(size_t size)
        {
            void* pAllocatedData = pData + written;
            written += size;
            return pAllocatedData;
        }
        inline void* get(size_t offset)
        {
            return pData + offset;
        }

        size_t written; 
        size_t totalSize;
        char* pData{nullptr};
    };
}