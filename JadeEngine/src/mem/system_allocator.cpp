#include "system_allocator.h"

#include <limits>

namespace je { namespace mem { 

    system_allocator::system_allocator(const char* a_name/* = nullptr*/, allocator_debug_flags a_debug_flags /*= base_allocator::k_default_debug_flags*/)
        : base_allocator(a_name, a_debug_flags)
    {
        const_cast<size_t&>(m_memory_num_bytes) = std::numeric_limits<size_t>::max();
    }

    system_allocator::~system_allocator()
    {
        conditionally_print_stack_trace();
    }

    system_allocator::mem_ptr system_allocator::allocate_internal(size_t a_num_bytes,
        alignment a_alignment, size_t& a_out_num_bytes_allocated)
    {
        void* mem = 
#if JE_PLATFORM_WINDOWS
            _aligned_malloc(a_num_bytes, static_cast<size_t>(a_alignment));
#elif JE_PLATFORM_LINUX || JE_PLATFORM_ANDROID
            aligned_alloc(a_alignment, a_num_bytes);
#else
#error "Implement allocate_internal."
            nullptr;
#endif

#if JE_DEBUG_ALLOCATIONS
        if(mem != nullptr)
        {
            m_allocation_map.emplace(mem, a_num_bytes);
        }
#endif

        a_out_num_bytes_allocated = a_num_bytes;
        return mem;
    }

    bool system_allocator::free_internal(mem_ptr a_memory, size_t& a_out_num_bytes_freed)
    {
#if JE_PLATFORM_WINDOWS
        _aligned_free(a_memory);
#elif JE_PLATFORM_LINUX || JE_PLATFORM_ANDROID
        free(a_memory);
#else
        #error "Implement free_internal."
#endif
        
#if JE_DEBUG_ALLOCATIONS
        auto it = m_allocation_map.find(a_memory);
        if(it != m_allocation_map.end())
        {
            a_out_num_bytes_freed = it->second;
            m_allocation_map.erase(it);
            return true;
        }
        else
        {
            return false;
        }
#else
        return true;
#endif
    }

}}