#pragma once

#include "global.h"

#define JE_TRACK_ALLOCATIONS JE_CONFIG_DEBUG

namespace JE_NAMESPACE { namespace mem { 

    class base_allocator
    {
    public:

        static const size_t k_default_alignment = 16;

        base_allocator();
        virtual ~base_allocator();

        void* allocate(size_t num_bytes, size_t alignment = k_default_alignment);
        void free(void* memory);

    protected:

        virtual void* allocate_internal(size_t num_bytes, size_t alignment) = 0;
        virtual bool free_internal(void* memory) = 0;
        
#if JE_TRACK_ALLOCATIONS
    private:
        size_t m_num_allocations;
#endif
    };

}}