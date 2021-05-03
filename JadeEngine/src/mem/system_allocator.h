#pragma once

#include "base_allocator.h"

#if JE_DEBUG_ALLOCATIONS
#include <unordered_map>
#endif

namespace je { namespace mem { 

    class system_allocator : public base_allocator
    {
    public:
        
        system_allocator(const char* name = nullptr, allocator_debug_flags debug_flags = base_allocator::k_default_debug_flags);
        virtual ~system_allocator();

    protected:

        virtual mem_ptr allocate_internal(size num_bytes, alignment a_alignment, size& out_num_bytes_allocated) override final;
        virtual bool free_internal(mem_ptr memory, size& out_num_bytes_freed) override final;

#if JE_DEBUG_ALLOCATIONS
        std::unordered_map<void*, size> m_allocation_map;
#endif
    };

}}