#include "allocatable.h"
#include "mem_manager.h"

namespace je { namespace mem {

    base_allocator& allocatable_trait_one_frame::get_allocator()
    {
        return mem_manager::get_inst().get_allocator_one_frame();
    }

    base_allocator& allocatable_trait_persistent::get_allocator()
    {
        return mem_manager::get_inst().get_allocator_persistent();
    }

    base_allocator& allocatable_trait_general_purpose::get_allocator()
    {
        return mem_manager::get_inst().get_allocator_general_purpose();
    }

    base_allocator& allocatable_trait_collections::get_allocator()
    {
        return mem_manager::get_inst().get_allocator_collections();
    }


#define JE_AllocatableDefineTrait(_trait_)                                                      \
    template<>                                                                                  \
    void* allocatable_base<_trait_>::operator new(size a_size) noexcept                         \
    {                                                                                           \
        return _trait_::get_allocator().allocate(a_size);                                       \
    }                                                                                           \
                                                                                                \
    template<>                                                                                  \
    void allocatable_base<_trait_>::operator delete(void* a_mem)                                \
    {                                                                                           \
        _trait_::get_allocator().free(a_mem);                                                   \
    }                                                                                           \
                                                                                                \
    template<>                                                                                  \
    const char* allocatable_base<_trait_>::get_allocator_name()                                 \
    {                                                                                           \
        return _trait_::get_allocator().get_name();                                             \
    }


    JE_AllocatableDefineTrait(allocatable_trait_one_frame);
    JE_AllocatableDefineTrait(allocatable_trait_persistent);
    JE_AllocatableDefineTrait(allocatable_trait_general_purpose);
    JE_AllocatableDefineTrait(allocatable_trait_collections);
}}