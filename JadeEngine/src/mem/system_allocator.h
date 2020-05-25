#pragma once

#include "base_allocator.h"

#if JE_TRACK_ALLOCATIONS
#include <unordered_map>
#endif

namespace je { namespace mem { 

    class system_allocator : public base_allocator
    {
    public:
        
        system_allocator();
        ~system_allocator();

    protected:

        virtual void* allocate_internal(size_t num_bytes, alignment a_alignment) override final;
        virtual size_t free_internal(void* memory) override final;

#if JE_TRACK_ALLOCATIONS
        std::unordered_map<void*, size_t> m_allocation_map;
#endif
    };

}}