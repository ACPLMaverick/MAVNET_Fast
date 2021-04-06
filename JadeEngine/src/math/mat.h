#pragma once

#include "global.h"
#include "vec.h"

namespace je { namespace math {

    // A column-major matrix. Uses GLSL convention as a standard.
    template<size_t num_cols, size_t num_rows>
    class mat
    {
    public:
        static const constexpr size_t k_num_cols = num_cols;
        static const constexpr size_t k_num_rows = num_rows;
    
    protected:

        // For internal use - create matrix without initializing its components.
        mat()
        {
        }

    public:

        mat(float n)
        {
            #pragma unroll
            for(size_t i = 0; i < k_num_cols && i < k_num_rows; ++i)
            {
                m_cols[i][i] = n;
            }
        }

        // Accessors.
        vec<num_rows>& operator[](size_t index)
        {
            JE_assert(index < k_num_cols, "Invalid index.");
            return m_cols[index];
        }

        const vec<num_rows>& operator[](size_t index) const
        {
            JE_assert(index < k_num_cols, "Invalid index.");
            return m_cols[index];
        }

        // Comparison operators.
        bool operator==(const mat& other) const
        {
            #pragma unroll
            for(size_t i = 0; i < k_num_cols; ++i)
            {
                if(m_cols[i] != other.m_cols[i])
                {
                    return false;
                }
            }

            return true;
        }

        bool operator!=(const mat& other) const
        {
            return (*this == other) == false;
        }

        // Arithmetric functions.
        mat& operator+=(const mat& other)
        {
            #pragma unroll
            for(size_t i = 0; i < k_num_cols; ++i)
            {
                m_cols[i] += other.m_cols[i];
            }
            return *this;
        }

        mat operator+(const mat& other) const
        {
            mat newMat(*this);
            newMat += other;
            return newMat;
        }

        mat& operator-=(const mat& other)
        {
            #pragma unroll
            for(size_t i = 0; i < k_num_cols; ++i)
            {
                m_cols[i] -= other.m_cols[i];
            }
            return *this;
        }

        mat operator-(const mat& other) const
        {
            mat newMat(*this);
            newMat -= other;
            return newMat;
        }

        mat& operator*=(float value)
        {
            #pragma unroll
            for(size_t i = 0; i < k_num_cols; ++i)
            {
                m_cols[i] *= value;
            }
            return *this;
        }

        mat& operator*(float value) const
        {
            mat newMat(*this);
            newMat *= value;
            return newMat;
        }

        vec<num_rows> operator*(const vec<num_rows>& value) const
        {
            vec<num_rows> result(0.0f);

            #pragma unroll
            for(size_t i = 0; i < num_rows; ++i)
            {
                for(size_t j = 0; j < num_cols; ++j)
                {
                    result[i] += (*this)[j][i] * value[j];
                }
            }

            return result;
        }

        template<size_t vec_num_components>
        vec<vec_num_components> operator*(const vec<vec_num_components>& value) const
        {
            vec<num_rows> converted_value(value);
            converted_value = *this * converted_value;
            return vec<vec_num_components>(converted_value);
        }

        template<size_t num_common, size_t num_rows_a, size_t num_cols_b>
        inline static mat<num_cols_b, num_rows_a> mul(const mat<num_common, num_rows_a>& a, const mat<num_cols_b, num_common>& b)
        {
            mat<num_cols_b, num_rows_a> result(0.0f);

            #pragma unroll
            for(size_t i = 0; i < num_cols_b; ++i)
            {
                #pragma unroll
                for(size_t j = 0; j < num_rows_a; ++j)
                {
                    #pragma unroll
                    for(size_t k = 0; k < num_common; ++k)
                    {
                        result[i][j] += a[k][j] * b[i][k];
                    }
                }
            }

            return result;
        }

        mat& operator*=(const mat& other)
        {
            *this = mul(*this, other);
            return *this;
        }

        template<size_t other_num_cols>
        mat<other_num_cols, num_rows> operator*(const mat<other_num_cols, num_cols>& other) const
        {
            return mul(*this, other);
        }

        // Transposition/inverse functions.

        // Special-case creation functions.
        inline static mat identity()
        {
            return mat(1.0f);
        }

        static mat transform
        (
            const vec3& position,
            const vec3& rotation = vec3(0.0f),
            const vec3& scale = vec3(1.0f)
        )
        {
            JE_todo();
            return mat();
        }

        static mat view_target(const vec3& view_position, const vec3& view_target_position)
        {
            JE_todo();
            return mat();
        }

        // Direction should be a normalized vector.
        static mat view_direction(const vec3& view_position, const vec3& view_direction)
        {
            JE_todo();
            JE_assert(sc::is_almost_equal(view_direction.get_length(), 1.0f), "view_direction is not a normalized vector.");
            return mat();
        }

        static mat projection_perspective
        (
            float fovY,
            float screen_aspect_ratio,
            float near_plane,
            float far_plane
        )
        {
            JE_todo();
            return mat();
        }

        static mat projection_ortho
        (
            float screen_width,
            float screen_height,
            float near_plane,
            float far_plane
        )
        {
            JE_todo();
            return mat();
        }

    protected:

        data::static_array<vec<k_num_rows>, k_num_cols> m_cols;
    };

    using mat4 = mat<4, 4>;
    using mat3x4 = mat<3, 4>;

}}