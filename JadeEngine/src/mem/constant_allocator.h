#pragma once

#include "base_allocator.h"

namespace je { namespace mem { 

    class constant_allocator : public base_allocator
    {
    public:

        constant_allocator(
            base_allocator& allocator_from,
            size_t num_bytes,
            alignment a_alignment = k_default_alignment);
        ~constant_allocator();

    protected:

        virtual void* allocate_internal(size_t num_bytes, alignment a_alignment) override final;
        virtual size_t free_internal(void* memory) override final;
    };

}}