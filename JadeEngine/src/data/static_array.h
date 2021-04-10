#pragma once

#include "global.h"

namespace je { namespace data {

    template <typename object_type, size_t num_objects>
    class static_array
    {
    public:

        static const size_t k_num_objects = num_objects;

        static_array()
        {
        }

        static_array(const object_type arr[])
        {
            copy<num_objects>(arr);
        }

        static_array(std::initializer_list<object_type> list)
        {
            JE_assert(list.size() == k_num_objects);
            copy<num_objects>(list.begin());
        }

        static_array(const object_type& a_value_to_copy_over)
        {
            for(size_t i = 0; i < k_num_objects; ++i)
            {
                m_array[i] = a_value_to_copy_over;
            }
        }

        static_array(const static_array& a_other)
        {
            copy<num_objects>(a_other.get_data());
        }

        template <size_t other_num_objects>
        static_array(const static_array<object_type, other_num_objects>& a_other)
        {
            static_assert(num_objects >= other_num_objects, "Trying to copy bigger array into smaller one.");
            copy<other_num_objects>(a_other.get_data());
        }

        template <size_t other_num_objects>
        static_array(const static_array<object_type, other_num_objects>& a_other, const object_type& a_value_to_copy_over)
            : static_array(a_other)
        {
            for(size_t i = other_num_objects; i < k_num_objects; ++i)
            {
                m_array[i] = a_value_to_copy_over;
            }
        }

        ~static_array()
        {
        }

        object_type* get_data() { return m_array; }
        const object_type* get_data() const { return m_array; }

        static_array& operator=(const static_array& a_other)
        {
            copy<num_objects>(a_other.m_array);
            return *this;
        }

        object_type& operator[](size_t a_index)
        {
            JE_assert(a_index < k_num_objects, "Out-of-bounds index.");
            return m_array[a_index];
        }

        const object_type& operator[](size_t a_index) const
        {
            JE_assert(a_index < k_num_objects, "Out-of-bounds index.");
            return m_array[a_index];
        }

    private:

        template<size_t num_objects_to_copy>
        inline void copy(const object_type* a_other_array)
        {
            if(std::is_pod<object_type>())
            {
                std::memcpy(m_array, a_other_array, num_objects_to_copy * sizeof(object_type));
            }
            else
            {
                #pragma unroll
                for(size_t i = 0; i < num_objects_to_copy; ++i)
                {
                    m_array[i] = a_other_array[i];
                }
            }
        }

        object_type m_array[k_num_objects];
    };

}}