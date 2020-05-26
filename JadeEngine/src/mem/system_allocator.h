#pragma once

#include "base_allocator.h"

#if JE_DEBUG_ALLOCATIONS
#include <unordered_map>
#endif

namespace je { namespace mem { 

    class system_allocator : public base_allocator
    {
    public:
        
        system_allocator();
        virtual ~system_allocator();

    protected:

        virtual mem_ptr allocate_internal(size_t num_bytes, alignment a_alignment, size_t& out_num_bytes_allocated) override final;
        virtual bool free_internal(mem_ptr memory, size_t& out_num_bytes_freed) override final;

#if JE_DEBUG_ALLOCATIONS
        std::unordered_map<void*, size_t> m_allocation_map;
#endif
    };

}}