#pragma once

#if JE_DATA_STRUCTS_STD_BACKEND

#include <memory>

namespace je { namespace mem {

    class std_wrapper_allocator_helper
    {
    public:
        
        template<typename object_type>
        static object_type* allocate(size_t a_num_objects)
        {
            const size_t num_bytes = a_num_objects * sizeof(object_type);
            return static_cast<object_type*>(allocate_internal(num_bytes, alignof(object_type)));
        }

        template<typename object_type>
        static void free(object_type* a_pointer)
        {
            free_internal(static_cast<void*>(a_pointer));
        }

    private:

        static void* allocate_internal(size_t num_bytes, size_t alignment);
        static void free_internal(void* memory);
    };

    template <typename object_type>
    class std_wrapper : public std::allocator<object_type>
    {
    public:

        using value_type = object_type;
        using propagate_on_container_move_assignment = std::true_type;
        using is_always_equal = std::true_type;

        template<typename other_object_type>
        struct rebind
        {
            typedef std_wrapper<other_object_type> other;
        };

        std_wrapper() = default;
        std_wrapper(const std_wrapper& a_other) = default;
        ~std_wrapper() = default;

        template <typename other_object_type>
        std_wrapper(const std_wrapper<other_object_type>& a_other)
            : std::allocator<object_type>(a_other)
        {
        }

        value_type* allocate(std::size_t a_num_objects)
        {
#if JE_DATA_STRUCTS_STD_BACKEND_CUSTOM_ALLOCATOR
            return std_wrapper_allocator_helper::allocate<value_type>(a_num_objects);
#else
            return std::allocator<object_type>::allocate(a_num_objects);
#endif
        }

        void deallocate(value_type* a_pointer, std::size_t a_num_objects)
        {
#if JE_DATA_STRUCTS_STD_BACKEND_CUSTOM_ALLOCATOR
            std_wrapper_allocator_helper::free<value_type>(a_pointer);
#else
            std::allocator<object_type>::deallocate(a_pointer, a_num_objects);
#endif
        }
    };

}}

#endif