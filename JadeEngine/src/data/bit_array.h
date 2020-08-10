#pragma once

#include "array.h"

namespace je { namespace data {

    // Stores a set of flags in a sequence of bytes.
    template <size_t num_bits>
    class bit_array
    {
    public:

        static const size_t k_num_bits = num_bits;
        static const size_t k_num_bytes = (k_num_bits / 8) + (k_num_bits % 8 == 0 ? 0 : 1);

        bit_array()
        {
            std::memset(m_data, 0, k_num_bytes);
        }

        bit_array(bool a_fill_with_ones)
        {
            std::memset(m_data, a_fill_with_ones ? 0xFF : 0, k_num_bytes);
        }

        bit_array(const bit_array& other)
            : m_data(other.m_data)
        {
        }

        bit_array& operator=(const bit_array& other)
        {
            std::memcpy(m_data, other.m_data, k_num_bytes);
            return *this;
        }

        const uint8_t* get_data() const { return m_data; }

        bool get(size_t a_index) const
        {
            size_t byte_index(0), bit_index(0);
            get_indices(a_index, byte_index, bit_index);
            return static_cast<bool>(m_data[byte_index] & (1 << bit_index));
        }

        void set(size_t a_index, bool a_value)
        {
            size_t byte_index(0), bit_index(0);
            get_indices(a_index, byte_index, bit_index);
            if(a_value)
            {
                m_data[byte_index] |= (1 << bit_index);
            }
            else
            {
                m_data[byte_index] &= ~(1 << bit_index);
            }
        }

    private:

        void get_indices(size_t a_index, size_t& a_out_byte_index, size_t& a_out_bit_index) const
        {
            JE_assert(a_index < k_num_bits, "Out-of-bounds index.");
            a_out_bit_index = a_index % 8;
            a_out_byte_index = a_index / 8;
        }

        uint8_t m_data[k_num_bytes];
    };


    // Stores bitfield enum values.
    template<typename bitfield_enum_type, size_t num_bits = static_cast<size_t>(bitfield_enum_type::k_num_bits)>
    class bitfield_array : public bit_array<num_bits>
    {
    public:

        bool get(bitfield_enum_type a_index) const
        {
            return bit_array<num_bits>::get(get_index_from_enum(a_index));
        }

        void set(bitfield_enum_type a_index, bool a_value)
        {
            bit_array<num_bits>::set(get_index_from_enum(a_index), a_value);
        }

        bool get(int a_merged_index) const
        {
            for(size_t i = 0; i < bit_array<num_bits>::k_num_bits; ++i)
            {
                if(a_merged_index & (1 << i))
                {
                    if(bit_array<num_bits>::get(i))
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        void set(int a_merged_index, bool a_value)
        {
            for(size_t i = 0; i < bit_array<num_bits>::k_num_bits; ++i)
            {
                if(a_merged_index & (1 << i))
                {
                    bit_array<num_bits>::set(i, a_value);
                }
            }
        }

    private:

        // TODO: Optimize. Maybe calculate it in compile time?
        static constexpr size_t get_index_from_enum(bitfield_enum_type a_enum)
        {
            const size_t enum_val = static_cast<size_t>(a_enum);
            for(size_t i = 0; i < bit_array<num_bits>::k_num_bits; ++i)
            {
                if(enum_val == (1 << i))
                {
                    return i;
                }
            }

            JE_fail("Given enum value is not a bit flag: [%lld]", enum_val);
            return 0;
        }
    };
}}