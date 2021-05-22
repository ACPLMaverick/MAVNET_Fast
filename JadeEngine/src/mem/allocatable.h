#pragma once

#include "global.h"

namespace je { namespace mem {

    class base_allocator;

    // These classes should correspond to allocators we have defined in mem_manager.
    class allocatable_trait_one_frame
    {
    public:

        static base_allocator& get_allocator();

    };

    class allocatable_trait_persistent
    {
    public:

        static base_allocator& get_allocator();

    };

    class allocatable_trait_general_purpose
    {
    public:

        static base_allocator& get_allocator();

    };

    class allocatable_trait_collections
    {
    public:

        static base_allocator& get_allocator();

    };

    template<class trait_type>
    class allocatable_base
    {
    public:

        inline void* operator new(size a_size) noexcept
        {
            void* mem = malloc_internal(a_size);
            if(mem != nullptr)
            {
                return mem;
            }
            else
            {
                JE_fail("Failed to allocate [", a_size, "] bytes from [", get_allocator_name(), "].");
                return nullptr;
            }
        }

        inline void operator delete(void* mem)
        {
            free_internal(mem);
        }

    private:

        static void* malloc_internal(size a_size) noexcept
        {
            JE_fail("Malloc function not defined for this allocatable.");
            return nullptr;
        }

        static void free_internal(void* mem)
        {
            JE_fail("Free function not defined for this allocatable.");
        }

        static const char* get_allocator_name()
        {
            JE_fail("Get Allocator Name function not defined for this allocatable.");
            return "N/A";
        }

    };

    using allocatable = allocatable_base<allocatable_trait_general_purpose>;
    using allocatable_one_frame = allocatable_base<allocatable_trait_one_frame>;
    using allocatable_persistent = allocatable_base<allocatable_trait_persistent>;
    using allocatable_collection = allocatable_base<allocatable_trait_collections>;

}}