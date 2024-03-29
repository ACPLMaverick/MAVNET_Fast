#pragma once

#include "global.h"

#if JE_DEBUG_ALLOCATIONS
#include <vector>
#define JE_DEBUG_ALLOCATIONS_FILL_MEMORY_ON_ALLOC 1
#include "util/stack_tracer.h"
#define JE_DEBUG_ALLOCATIONS_USE_STACK_TRACER 1 && (JE_USE_STACK_TRACER)
#else
#define JE_DEBUG_ALLOCATIONS_FILL_MEMORY_ON_ALLOC 0
#define JE_DEBUG_ALLOCATIONS_USE_STACK_TRACER 0
#endif

namespace je { namespace mem { 

    enum class alignment : u8
    {
        k_1     = 1,
        k_4     = 4,
        k_8     = 8,
        k_16    = 16,
        k_32    = 32,
        k_64    = 64
    };

    JE_bitfield allocator_debug_flags
    {
        k_none                  = 0,
        k_count_allocations     = (1 << 0),
        k_fill_memory_on_alloc  = (1 << 1),
        k_stack_tracer          = (1 << 2),
        k_boundary_guard        = (1 << 3),
        k_all                   = 0xFF
    };

    static const size k_kB = 1024ULL;
    static const size k_MB = 1024ULL * 1024ULL;
    static const size k_GB = 1024ULL * 1024ULL * 1024ULL;


    class base_allocator
    {
    public:

        static const alignment k_default_alignment = alignment::k_16;
        static const allocator_debug_flags k_default_debug_flags = allocator_debug_flags::k_all;

        base_allocator(const char* name = nullptr, allocator_debug_flags debug_flags = base_allocator::k_default_debug_flags);
        base_allocator(
            base_allocator& allocator_from,
            size num_bytes,
            alignment a_alignment = k_default_alignment,
            const char* name = nullptr,
            allocator_debug_flags debug_flags = base_allocator::k_default_debug_flags);
        virtual ~base_allocator();

        JE_disallow_copy(base_allocator);

        void* allocate(size num_bytes, alignment a_alignment = k_default_alignment);
        void free(void* memory);

        size get_total_memory() const { return m_memory_num_bytes; }

#if JE_DEBUG_ALLOCATIONS
        const std::vector<base_allocator*>& get_child_allocators() const { return m_child_allocators; }
        const char* get_name() const { return m_name != nullptr ? m_name : "N/A"; }
        size get_num_allocations() const { return m_num_allocations; }
        size get_used_memory() const { return m_used_num_bytes; }
        size get_memory_left() const { return get_total_memory() - get_used_memory(); }
#else
        const char* get_name() const { return "N/A"; }
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
            mem_ptr(uptr a_num)
                : m_memory(reinterpret_cast<void*>(a_num))
            {}
            mem_ptr(void* a_memory, alignment a_alignment)
                : mem_ptr(a_memory)
            {
                align(a_alignment);
            }
            mem_ptr(uptr a_num, alignment a_alignment)
                : mem_ptr(a_num)
            {
                align(a_alignment);
            }

            operator const void*() const { return m_memory; }
            operator void*&() { return m_memory; }
            operator uptr() const { return reinterpret_cast<uptr>(m_memory); }
            operator uptr&() { return reinterpret_cast<uptr&>(m_memory); }

            uptr as_num() const { return reinterpret_cast<uptr>(m_memory); }
            const void* get() const { return m_memory; }
            void* get() { return m_memory; }

            bool is_null() const { return m_memory == nullptr; }
            
            mem_ptr& operator=(const mem_ptr& other) { m_memory = other.m_memory; return *this; }
            mem_ptr& operator=(void* other) { m_memory = other; return *this; }
            mem_ptr& operator=(uptr other) { m_memory = reinterpret_cast<void*>(other); return *this; }

            bool operator==(const mem_ptr& other) const { return m_memory == other.m_memory; }
            bool operator!=(const mem_ptr& other) const { return m_memory != other.m_memory; }
            bool operator>=(const mem_ptr& other) const { return m_memory >= other.m_memory; }
            bool operator<=(const mem_ptr& other) const { return m_memory <= other.m_memory; }
            bool operator>(const mem_ptr& other) const { return m_memory > other.m_memory; }
            bool operator<(const mem_ptr& other) const { return m_memory < other.m_memory; }

            void align(alignment a_alignment);
            size align_adjust(alignment a_alignment, size additional_num_bytes);
            bool is_aligned(alignment a_alignment);

            template <typename cast_type> cast_type* cast() const
            {
                return reinterpret_cast<cast_type*>(m_memory);
            }

            template <typename block_type> block_type* get_struct_ptr_before() const
            {
                return reinterpret_cast<block_type*>(*this - sizeof(block_type));
            }

        private:
            void* m_memory;
        };

        // Virtual interface.
        virtual mem_ptr allocate_internal(size num_bytes, alignment a_alignment, size& out_num_bytes_allocated) = 0;
        virtual bool free_internal(mem_ptr memory, size& out_num_bytes_freed) = 0;

        // Utilities.
        inline bool debug_process_before_allocate(size num_bytes, alignment a_alignment);
        inline void debug_process_after_allocate(size num_bytes, alignment a_alignment, void* memory, size bytes_allocated);

        inline bool debug_process_before_free(void* memory);
        inline void debug_process_after_free(bool free_succeeded, size num_bytes_freed);

        void conditionally_print_stack_trace();
        static alignment get_alignment(mem_ptr memory);
        static bool is_alignment_correct(alignment a_alignment);

        base_allocator* m_allocator_from;
        void* m_memory;
        const size m_memory_num_bytes;
        
    protected:
#if JE_DEBUG_ALLOCATIONS
        std::vector<base_allocator*> m_child_allocators;
        size m_num_allocations;
        size m_used_num_bytes;
        const char* m_name;
        allocator_debug_flags m_debug_flags;
#endif
#if JE_DEBUG_ALLOCATIONS_USE_STACK_TRACER
        util::stack_tracer m_stack_tracer;
#endif
    };

}}