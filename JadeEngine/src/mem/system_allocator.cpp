#include "system_allocator.h"

namespace je { namespace mem { 

    system_allocator::system_allocator()
        : base_allocator()
    {

    }

    system_allocator::~system_allocator()
    {

    }

    void* system_allocator::allocate_internal(size_t a_num_bytes, size_t a_alignment)
    {
        void* mem = 
#if JE_PLATFORM_WINDOWS
            _aligned_malloc(a_num_bytes, a_alignment);
#elif JE_PLATFORM_LINUX || JE_PLATFORM_ANDROID
            aligned_alloc(a_alignment, a_num_bytes);
#else
#error "Implement allocate_internal."
            nullptr;
#endif

#if JE_TRACK_ALLOCATIONS
        if(mem != nullptr)
        {
            m_allocation_map.emplace(mem, a_num_bytes);
        }
#endif

        return mem;
    }

    size_t system_allocator::free_internal(void* a_memory)
    {
#if JE_PLATFORM_WINDOWS
        _aligned_free(a_memory);
#elif JE_PLATFORM_LINUX || JE_PLATFORM_ANDROID
        free(a_memory);
#else
        #error "Implement free_internal."
#endif
        
#if JE_TRACK_ALLOCATIONS
        auto it = m_allocation_map.find(a_memory);
        if(it != m_allocation_map.end())
        {
            const size_t num_bytes = it->second;
            m_allocation_map.erase(it);
            return num_bytes;
        }
        else
        {
            return 0;
        }
#else
        return 0;
#endif
    }

}}