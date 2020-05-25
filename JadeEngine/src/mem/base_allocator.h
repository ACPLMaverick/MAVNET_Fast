#pragma once

#include "global.h"

#define JE_TRACK_ALLOCATIONS JE_CONFIG_DEBUG

namespace je { namespace mem { 

    enum class alignment : size_t
    {
        k_0 = 0,
        k_4 = 4,
        k_8 = 8,
        k_16 = 16,
        k_32 = 32,
        k_64 = 64
    };

    class base_allocator
    {
    public:

        static const alignment k_default_alignment = alignment::k_16;

        base_allocator();
        base_allocator(
            base_allocator& allocator_from,
            size_t num_bytes,
            alignment alignment = k_default_alignment);
        virtual ~base_allocator();

        void* allocate(size_t num_bytes, alignment a_alignment = k_default_alignment);
        void free(void* memory);

#if JE_TRACK_ALLOCATIONS
        size_t get_num_allocations() const { return m_num_allocations; }
        size_t get_used_memory() const { return m_used_num_bytes; }
#endif

    protected:

        virtual void* allocate_internal(size_t num_bytes, alignment a_alignment) = 0;
        virtual size_t free_internal(void* memory) = 0;

        static inline uintptr_t memory_to_uint(void* memory)
            { return reinterpret_cast<uintptr_t>(memory); }
        static inline void* uint_to_memory(uintptr_t number)
            { return reinterpret_cast<void*>(number); }
        inline uintptr_t get_memory_uint() const 
            { return memory_to_uint(m_memory); }

        static size_t alignment_to_num(alignment a_alignment)
            { return static_cast<size_t>(a_alignment); }
        static size_t alignment_to_uint(alignment a_alignment)
            { return static_cast<uintptr_t>(a_alignment); }

        static void* align_memory(void* memory, alignment a_alignment);
        static bool is_memory_aligned(void* memory, alignment a_alignment);

        base_allocator* m_allocator_from;
        void* m_memory;
        size_t m_memory_num_bytes;
        
#if JE_TRACK_ALLOCATIONS
    private:
        size_t m_num_allocations;
        size_t m_used_num_bytes;
#endif
    };

}}