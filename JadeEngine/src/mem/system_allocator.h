#pragma once

#include "base_allocator.h"

namespace JE_NAMESPACE { namespace mem { 

    class system_allocator : public base_allocator
    {
    public:
        
        system_allocator();
        ~system_allocator();

    protected:

        virtual void* allocate_internal(size_t num_bytes, size_t alignment) override final;
        virtual bool free_internal(void* memory) override final;

    };

}}