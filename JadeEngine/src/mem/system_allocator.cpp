#include "system_allocator.h"

namespace JE_NAMESPACE { namespace mem { 

    system_allocator::system_allocator()
        : base_allocator()
    {

    }

    system_allocator::~system_allocator()
    {

    }

    void* system_allocator::allocate_internal(size_t a_num_bytes, size_t a_alignment)
    {
#if JE_PLATFORM_WINDOWS
        return _aligned_malloc(a_num_bytes, a_alignment);
#elif JE_PLATFORM_LINUX || JE_PLATFORM_ANDROID
        return aligned_alloc(a_alignment, a_num_bytes);
#else
        #error "Implement allocate_internal."
        return nullptr;
#endif
    }

    bool system_allocator::free_internal(void* a_memory)
    {
#if JE_PLATFORM_WINDOWS
        _aligned_free(a_memory);
        return true;
#elif JE_PLATFORM_LINUX || JE_PLATFORM_ANDROID
        free(a_memory);
        return true;
#else
        #error "Implement free_internal."
        return false;
#endif
    }

}}