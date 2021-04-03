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

    public:

        // With default parameter creates an identity matrix. (Or identityish, if num_cols != num_rows).
        // Otherwise creates a matrix with a N value at main diagonal.
        mat(float n = 1.0f)
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

        vec<num_rows> operator[](size_t index) const
        {
            JE_assert(index < k_num_cols, "Invalid index.");
            return m_cols[index];
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

        // Transposition/inverse functions.

        // Determinant functions.

        // Special-case creation functions.
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