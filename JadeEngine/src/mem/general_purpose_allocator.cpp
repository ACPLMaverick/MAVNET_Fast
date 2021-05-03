#include "general_purpose_allocator.h"

namespace je { namespace mem {

    general_purpose_allocator::general_purpose_allocator(
        base_allocator& a_allocator_from,
        size a_num_bytes,
        alignment a_alignment /*= k_default_alignment*/,
        const char* a_name/* = nullptr*/,
        allocator_debug_flags a_debug_flags /*= base_allocator::k_default_debug_flags*/)
        : base_allocator(a_allocator_from, a_num_bytes, a_alignment, a_name, a_debug_flags)
        , m_free_blocks(reinterpret_cast<free_block*>(m_memory))
    {
        JE_assert(m_memory_num_bytes >= sizeof(free_block), "Too little memory for general purpose allocator.");
        m_free_blocks->m_num_bytes = m_memory_num_bytes;
        m_free_blocks->m_next = nullptr;
    }

    general_purpose_allocator::~general_purpose_allocator()
    {
        conditionally_print_stack_trace();
        JE_assert(m_free_blocks == m_memory
            && m_free_blocks->m_num_bytes == m_memory_num_bytes
            && m_free_blocks->m_next == nullptr, "Destroyed allocator is corrupted.");
    }

    general_purpose_allocator::mem_ptr general_purpose_allocator::allocate_internal(
        size a_num_bytes, alignment a_alignment, size& a_out_num_bytes_allocated)
    {
        free_block* prev_free_block(nullptr);
        free_block* curr_free_block(m_free_blocks);

        while(curr_free_block != nullptr)
        {
            mem_ptr aligned_memory(curr_free_block);
            u8 adjustment = aligned_memory.align_adjust(a_alignment, sizeof(allocation_header));
            size total_num_bytes = a_num_bytes + adjustment;

            // If allocation doesn't fit this free_block, try the next one.
            if(curr_free_block->m_num_bytes < total_num_bytes)
            {
                prev_free_block = curr_free_block;
                curr_free_block = curr_free_block->m_next;
                continue;
            }

            static_assert(sizeof(allocation_header) >= sizeof(free_block));
            
            // If allocations in the remaining memory will be impossible...
            if(curr_free_block->m_num_bytes - total_num_bytes <= sizeof(allocation_header))
            {
                JE_assert(static_cast<i64>(curr_free_block->m_num_bytes) - static_cast<i64>(total_num_bytes) >= 0,
                    "We would allocate less memory than needed.");
                // ... increase allocation size instead of creating a new free_block.
                total_num_bytes = curr_free_block->m_num_bytes;

                // Mark curr_free_block as not free.
                if(prev_free_block != nullptr)
                {
                    prev_free_block->m_next = curr_free_block->m_next;
                }
                else
                {
                    m_free_blocks = curr_free_block->m_next;
                }
            }
            else
            {
                // Else create a new free_block containing remaining memory.

                free_block* next_free_block = reinterpret_cast<free_block*>(mem_ptr(curr_free_block) + total_num_bytes);
                next_free_block->m_num_bytes = curr_free_block->m_num_bytes - total_num_bytes;
                next_free_block->m_next = curr_free_block->m_next;

                // Mark curr_free_block as not free.
                if(prev_free_block != nullptr)
                {
                    prev_free_block->m_next = next_free_block;
                }
                else
                {
                    m_free_blocks = next_free_block;
                }
            }

            allocation_header* header = aligned_memory.get_struct_ptr_before<allocation_header>();
            header->m_num_bytes = total_num_bytes;
            header->m_adjustment_num_bytes = adjustment;

            a_out_num_bytes_allocated = total_num_bytes;
            return aligned_memory;
        }

#if JE_DEBUG_ALLOCATIONS
        JE_fail("Could not find a large enough free block.");
#endif

        return nullptr;
    }

    bool general_purpose_allocator::free_internal(mem_ptr a_memory, size& a_out_num_bytes_freed)
    {
        allocation_header* header(a_memory.get_struct_ptr_before<allocation_header>());

        mem_ptr block_start(a_memory - header->m_adjustment_num_bytes);
        const size block_num_bytes = header->m_num_bytes;
        mem_ptr block_end(block_start + block_num_bytes);

        free_block* prev_free_block(nullptr);
        free_block* next_free_block(m_free_blocks);

        // Find closest free blocks before and after current memory ptr.
        // TODO Do i have to traverse through the list from the beginning to find it?
        while(next_free_block != nullptr)
        {
            if(mem_ptr(next_free_block) >= block_end)
            {
                break;
            }

            prev_free_block = next_free_block;
            next_free_block = next_free_block->m_next;
        }

        if(prev_free_block == nullptr)
        {
            // No free memory before this block. Create a new free block at the beginning.
            prev_free_block =  block_start.cast<free_block>();
            prev_free_block->m_num_bytes = block_num_bytes;
            prev_free_block->m_next = m_free_blocks;
            m_free_blocks = prev_free_block;
        }
        else if(mem_ptr(prev_free_block) + prev_free_block->m_num_bytes == block_start)
        {
            // No need to create any new free block - freed one and prev_free_block are adjacent. Just merge them.
            prev_free_block->m_num_bytes += block_num_bytes;
        }
        else
        {
            // Create a new free block right here and plug it between prev_ and next_free_blocks.
            free_block* temp = block_start.cast<free_block>();
            temp->m_num_bytes = block_num_bytes;
            temp->m_next = prev_free_block->m_next;
            prev_free_block->m_next = temp;
            prev_free_block = temp;
        }

        if(next_free_block != nullptr && next_free_block == block_end.cast<free_block>())
        {
            // Next free block is adjacent with the one we have freed. Merge them.
            prev_free_block->m_num_bytes += next_free_block->m_num_bytes;
            prev_free_block->m_next = next_free_block->m_next;
        }

        a_out_num_bytes_freed = block_num_bytes;
        return true;
    }

}}