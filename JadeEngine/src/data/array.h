#pragma once

#include "global.h"

namespace je { namespace data {

    template <typename object_type, size_t num_objects>
    class array
    {
    public:

        static const size_t k_num_objects = num_objects;

        array()
        {
        }

        array(const object_type& a_value_to_copy_over)
        {
            for(size_t i = 0; i < k_num_objects; ++i)
            {
                m_array[i] = a_value_to_copy_over;
            }
        }

        array(const array& a_other)
            : m_array(a_other.m_array)
        {
        }

        array(const array&& a_other)
            : m_array(a_other.m_array)
        {
        }

        template <size_t other_num_objects>
        array(const array<object_type, other_num_objects>& a_other)
        {
            static_assert(num_objects >= other_num_objects, "Trying to copy bigger array into smaller one.");
            copy(a_other.get_data(), other_num_objects);
        }

        template <size_t other_num_objects>
        array(const array<object_type, other_num_objects>& a_other, const object_type& a_value_to_copy_over)
            : array(a_other)
        {
            for(size_t i = other_num_objects; i < k_num_objects; ++i)
            {
                m_array[i] = a_value_to_copy_over;
            }
        }

        ~array()
        {
        }

        const object_type* get_data() const { return m_array; }

        array& operator=(const array& a_other)
        {
            copy(a_other.m_array, k_num_objects);
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

        inline void copy(const object_type* a_other_array, size_t a_num_objects_to_copy)
        {
            if(std::is_pod<object_type>())
            {
                std::memcpy(m_array, a_other_array, a_num_objects_to_copy * sizeof(object_type));
            }
            else
            {
                for(size_t i = 0; i < a_num_objects_to_copy; ++i)
                {
                    m_array[i] = a_other_array[i];
                }
            }
        }

        object_type m_array[k_num_objects];
    };

}}