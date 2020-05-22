#pragma once

#include "global.h"

#define JE_TRACK_ALLOCATIONS JE_CONFIG_DEBUG

namespace je { namespace mem { 

    class base_allocator
    {
    public:

        static const size_t k_default_alignment = 16;

        base_allocator();
        virtual ~base_allocator();

        void* allocate(size_t num_bytes, size_t alignment = k_default_alignment);
        void free(void* memory);

#if JE_TRACK_ALLOCATIONS
        size_t get_num_allocations() const { return m_num_allocations; }
        size_t get_used_memory() const { return m_used_memory; }
#endif

    protected:

        virtual void* allocate_internal(size_t num_bytes, size_t alignment) = 0;
        virtual size_t free_internal(void* memory) = 0;
        
#if JE_TRACK_ALLOCATIONS
    private:
        size_t m_num_allocations;
        size_t m_used_memory;
#endif
    };

}}