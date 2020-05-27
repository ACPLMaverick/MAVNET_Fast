#pragma once

#include "global.h"

#if JE_CONFIG_DEBUG
#define JE_DEBUG_ALLOCATIONS 1
#define JE_DEBUG_ALLOCATIONS_FILL_MEMORY_ON_ALLOC 1
#else
#define JE_DEBUG_ALLOCATIONS 0
#define JE_DEBUG_ALLOCATIONS_FILL_MEMORY_ON_ALLOC 0
#endif

namespace je { namespace mem { 

    enum class alignment : uint8_t
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

        size_t get_total_memory() const { return m_memory_num_bytes; }

#if JE_DEBUG_ALLOCATIONS
        size_t get_num_allocations() const { return m_num_allocations; }
        size_t get_used_memory() const { return m_used_num_bytes; }
        size_t get_memory_left() const { return get_total_memory() - get_used_memory(); }
#endif

    protected:

        class mem_ptr
        {
        public:
            mem_ptr()
                : m_memory(nullptr)
            {}
            mem_ptr(void* a_memory)
                : m_memory(a_memory)
            {}
            mem_ptr(uintptr_t a_num)
                : m_memory(reinterpret_cast<void*>(a_num))
            {}
            mem_ptr(void* a_memory, alignment a_alignment)
                : mem_ptr(a_memory)
            {
                align(a_alignment);
            }
            mem_ptr(uintptr_t a_num, alignment a_alignment)
                : mem_ptr(a_num)
            {
                align(a_alignment);
            }

            operator const void*() const { return m_memory; }
            operator void*&() { return m_memory; }
            operator uintptr_t() const { return reinterpret_cast<uintptr_t>(m_memory); }
            operator uintptr_t&() { return reinterpret_cast<uintptr_t&>(m_memory); }

            uintptr_t as_num() const { return reinterpret_cast<uintptr_t>(m_memory); }
            const void* get() const { return m_memory; }
            void* get() { return m_memory; }
            const uint8_t* as_data() const { return reinterpret_cast<const uint8_t*>(m_memory); }
            uint8_t* as_data() { return reinterpret_cast<uint8_t*&>(m_memory); }
            
            mem_ptr& operator=(const mem_ptr& other) { m_memory = other.m_memory; return *this; }
            mem_ptr& operator=(void* other) { m_memory = other; return *this; }
            mem_ptr& operator=(uint8_t* other) { m_memory = other; return *this; }
            mem_ptr& operator=(uintptr_t other) { m_memory = reinterpret_cast<void*>(other); return *this; }

            bool operator==(const mem_ptr& other) const { return m_memory == other.m_memory; }
            bool operator!=(const mem_ptr& other) const { return m_memory != other.m_memory; }

            void align(alignment a_alignment);
            void align(alignment a_alignment, size_t additional_num_bytes);
            bool is_aligned(alignment a_alignment);

        private:
            void* m_memory;
        };

        static const size_t k_kB = 1024ULL;
        static const size_t k_MB = 1024ULL * 1024ULL;
        static const size_t k_GB = 1024ULL * 1024ULL * 1024ULL;

        // Virtual interface.
        virtual mem_ptr allocate_internal(size_t num_bytes, alignment a_alignment, size_t& out_num_bytes_allocated) = 0;
        virtual bool free_internal(mem_ptr memory, size_t& out_num_bytes_freed) = 0;

        // Utilities.
        static alignment get_alignment(mem_ptr memory);

        base_allocator* m_allocator_from;
        void* m_memory;
        const size_t m_memory_num_bytes;
        
#if JE_DEBUG_ALLOCATIONS
    protected:
        size_t m_num_allocations;
        size_t m_used_num_bytes;
#endif
    };

}}