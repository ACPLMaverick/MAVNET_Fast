#pragma once

namespace je { namespace data {

    // Stores a set of flags in a sequence of bytes.
    template <size_t num_bits>
    class static_bit_array
    {
    public:

        static const size_t k_num_bits = num_bits;
        static const size_t k_num_bytes = (k_num_bits / 8) + (k_num_bits % 8 == 0 ? 0 : 1);

        static_bit_array()
        {
            std::memset(m_data, 0, k_num_bytes);
        }

        static_bit_array(bool a_fill_with_ones)
        {
            std::memset(m_data, a_fill_with_ones ? 0xFF : 0, k_num_bytes);
        }

        static_bit_array(const static_bit_array& other)
            : m_data(other.m_data)
        {
        }

        static_bit_array& operator=(const static_bit_array& other)
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

    protected:

        uint8_t* get_data() { return m_data; }

        void get_indices(size_t a_index, size_t& a_out_byte_index, size_t& a_out_bit_index) const
        {
            JE_assert(a_index < k_num_bits, "Out-of-bounds index.");
            a_out_bit_index = a_index % 8;
            a_out_byte_index = a_index / 8;
        }

    private:

        uint8_t m_data[k_num_bytes];
    };


    // Stores bitfield enum values.
    template<typename bitfield_enum_type, size_t num_bits = static_cast<size_t>(bitfield_enum_type::k_num_bits)>
    class static_bitfield_array : public static_bit_array<num_bits>
    {
    public:

        bool get(size_t a_merged_index) const
        {
            return get_data_as_num() & a_merged_index;
        }

        void set(size_t a_merged_index, bool a_value)
        {
            if(a_value)
            {
                get_data_as_num() |= a_merged_index;
            }
            else
            {
                get_data_as_num() &= ~a_merged_index;
            }
        }

        bool get(bitfield_enum_type a_index) const
        {
            return get(static_cast<size_t>(a_index));
        }

        void set(bitfield_enum_type a_index, bool a_value)
        {
            set(static_cast<size_t>(a_index), a_value);
        }

    private:

        const size_t& get_data_as_num() const
        {
            return *reinterpret_cast<const size_t*>(static_bit_array<num_bits>::get_data());
        }

        size_t& get_data_as_num()
        {
            return *reinterpret_cast<size_t*>(static_bit_array<num_bits>::get_data());
        }
    };
}}