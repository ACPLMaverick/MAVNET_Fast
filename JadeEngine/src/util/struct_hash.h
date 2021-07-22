#pragma once

#include "global.h"

namespace je { namespace util {

    template<typename struct_type>
    class struct_hash
    {
    public:

        struct_hash(const struct_type& a_object)
            : m_hash(0)
        {
            static const size k_array_size = sizeof(struct_type) / sizeof(u64) + static_cast<size>(sizeof(struct_type) % sizeof(u64) != 0);
            static const size k_num_redundant_bytes = (k_array_size * sizeof(u64)) - sizeof(struct_type);
            const u64* array = reinterpret_cast<const u64*>(&a_object);
            u64 num_to_iterate = k_array_size;
            if(k_num_redundant_bytes > 0)
            {
                num_to_iterate = k_array_size - 1;
                const u64 last_value = array[k_array_size - 1];
                const u64 bitmask = (1 << k_num_redundant_bytes * 8) - 1;
                m_hash ^= (last_value & bitmask);
            }

            for(size i = 0; i < num_to_iterate; ++i)
            {
                m_hash ^= array[i];
            }
        }

        u64 get_value() const
        {
            return m_hash;
        }

        bool operator==(const struct_hash& other) const
        {
            return m_hash == other.m_hash;
        }

        bool operator!=(const struct_hash& other) const
        {
            return m_hash != other.m_hash;
        }

        bool operator>(const struct_hash& other) const
        {
            return m_hash > other.m_hash;
        }

        bool operator<(const struct_hash& other) const
        {
            return m_hash < other.m_hash;
        }

        bool operator>=(const struct_hash& other) const
        {
            return m_hash >= other.m_hash;
        }

        bool operator<=(const struct_hash& other) const
        {
            return m_hash <= other.m_hash;
        }

    private:

        u64 m_hash;
    };
}}