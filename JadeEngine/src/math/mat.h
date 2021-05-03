#pragma once

#include "global.h"
#include "vec.h"

namespace je
{
    namespace math
    {
        // A column-major matrix. Uses GLSL convention as a standard.
        template <size num_cols, size num_rows>
        class mat
        {
        public:
            static const constexpr size k_num_cols = num_cols;
            static const constexpr size k_num_rows = num_rows;

        public:

            template<typename ... args_type>
            mat(args_type ... list)
                : m_cols({ list... })
            {
            }

            // This ctor creates an uninitialized object. Use with care!
            mat()
            {
            }
            
            mat(f32 n)
            {
                #pragma unroll
                for (size i = 0; i < k_num_cols; ++i)
                {
                    #pragma unroll
                    for(size j = 0; j < k_num_rows; ++j)
                    {
                        m_cols[i][j] = static_cast<f32>(i == j) * n;
                    }
                }
            }

            template<size other_num_cols, size other_num_rows>
            mat(const mat<other_num_cols, other_num_rows>& other)
            {
                constexpr const size cols_to_zero = num_cols > other_num_cols ? num_cols - other_num_cols : 0;
                constexpr const size rows_to_zero = num_rows > other_num_rows ? num_rows - other_num_rows : 0;

                #pragma unroll
                for(size i = 0; i < k_num_cols && i < other_num_cols; ++i)
                {
                    #pragma unroll
                    for(size j = 0; j < k_num_rows && j < other_num_rows; ++j)
                    {
                        m_cols[i][j] = other[i][j];
                    }
                }

                #pragma unroll
                for(size i = (k_num_cols - cols_to_zero); i < k_num_cols; ++i)
                {
                    #pragma unroll
                    for(size j = 0; j < k_num_rows; ++j)
                    {
                        m_cols[i][j] = i == j ? 1.0f : 0.0f;
                    }
                }

                if(rows_to_zero > 0)
                {
                    #pragma unroll
                    for(size i = 0; i < k_num_cols; ++i)
                    {
                        #pragma unroll
                        for(size j = (k_num_rows + rows_to_zero); j < k_num_rows; ++j)
                        {
                            m_cols[i][j] = i == j ? 1.0f : 0.0f;
                        }
                    }
                }
            } 

            // Accessors.
            vec<num_rows>& operator[](size index)
            {
                JE_assert(index < k_num_cols, "Invalid index.");
                return m_cols[index];
            }

            const vec<num_rows>& operator[](size index) const
            {
                JE_assert(index < k_num_cols, "Invalid index.");
                return m_cols[index];
            }

            // Comparison operators.
            bool operator==(const mat &other) const
            {
                #pragma unroll
                for (size i = 0; i < k_num_cols; ++i)
                {
                    if (m_cols[i] != other.m_cols[i])
                    {
                        return false;
                    }
                }

                return true;
            }

            bool operator!=(const mat &other) const
            {
                return (*this == other) == false;
            }

            static bool is_almost_equal(const mat &a, const mat &b)
            {
                #pragma unroll
                for (size i = 0; i < k_num_cols; ++i)
                {
                    if (sc::is_almost_equal(a.m_cols[i], b.m_cols[i]) == false)
                    {
                        return false;
                    }
                }

                return true;
            }

            // Arithmetric functions.
            mat& operator+=(const mat &other)
            {
                #pragma unroll
                for (size i = 0; i < k_num_cols; ++i)
                {
                    m_cols[i] += other.m_cols[i];
                }
                return *this;
            }

            mat operator+(const mat &other) const
            {
                mat newMat(*this);
                newMat += other;
                return newMat;
            }

            mat& operator-=(const mat &other)
            {
                #pragma unroll
                for (size i = 0; i < k_num_cols; ++i)
                {
                    m_cols[i] -= other.m_cols[i];
                }
                return *this;
            }

            mat operator-(const mat &other) const
            {
                mat newMat(*this);
                newMat -= other;
                return newMat;
            }

            mat& operator*=(f32 value)
            {
                #pragma unroll
                for (size i = 0; i < k_num_cols; ++i)
                {
                    m_cols[i] *= value;
                }
                return *this;
            }

            mat operator*(f32 value) const
            {
                mat newMat(*this);
                newMat *= value;
                return newMat;
            }

            vec<num_rows> operator*(const vec<num_rows> &value) const
            {
                vec<num_rows> result(0.0f);

                #pragma unroll
                for (size i = 0; i < num_rows; ++i)
                {
                    #pragma unroll
                    for (size j = 0; j < num_cols; ++j)
                    {
                        result[i] += (*this)[j][i] * value[j];
                    }
                }

                return result;
            }

            template <size vec_num_components>
            vec<vec_num_components> operator*(const vec<vec_num_components> &value) const
            {
                vec<num_rows> converted_value(value);
                converted_value = *this * converted_value;
                return vec<vec_num_components>(converted_value);
            }

        protected:

            template <size num_common, size num_rows_a, size num_cols_b>
            inline static mat<num_cols_b, num_rows_a> mul(const mat<num_common, num_rows_a> &a, const mat<num_cols_b, num_common> &b)
            {
                mat<num_cols_b, num_rows_a> result(0.0f);

                #pragma unroll
                for (size i = 0; i < num_cols_b; ++i)
                {
                    #pragma unroll
                    for (size j = 0; j < num_rows_a; ++j)
                    {
                        #pragma unroll
                        for (size k = 0; k < num_common; ++k)
                        {
                            result[i][j] += a[k][j] * b[i][k];
                        }
                    }
                }

                return result;
            }

        public:

            mat &operator*=(const mat &other)
            {
                *this = mul(*this, other);
                return *this;
            }

            template <size other_num_cols>
            mat<other_num_cols, num_rows> operator*(const mat<other_num_cols, num_cols> &other) const
            {
                return mul(*this, other);
            }

            // Transposition/inverse functions.

            mat<num_rows, num_cols> transposed() const
            {
                mat<num_rows, num_cols> new_mat;

                #pragma unroll
                for (size i = 0; i < k_num_cols; ++i)
                {
                    #pragma unroll
                    for (size j = 0; j < k_num_rows; ++j)
                    {
                        new_mat[j][i] = m_cols[i][j];
                    }
                }

                return new_mat;
            }

            mat inversed() const; // Implemented per-case.

            void transpose()
            {
                *this = transposed();
            }

            void inverse()
            {
                *this = inversed();
            }

            // Transform functions
            void translate(const vec3& translation)
            {
                static const size last_col = num_cols - 1;
                m_cols[last_col][0] += translation[0];
                m_cols[last_col][1] += translation[1];
                m_cols[last_col][2] += translation[2];
            }

        protected:

            inline static void rot(mat& matrix, f32 angle, const vec3& axis)
            {
                // From GLM.

                const f32 a = angle;
                const sc::sincos sin_and_cos = sc::sin_cos(a);
                const f32 c = sin_and_cos.cos;
                const f32 s = sin_and_cos.sin;

                JE_assert(sc::is_almost_equal(axis.get_length_squared(), 1.0f), "Rotation axis must be a normalized vector.");
                vec3 temp(axis * (1.0f - c));

                mat mat_rotate;
                mat_rotate[0][0] = c + temp[0] * axis[0];
                mat_rotate[0][1] = temp[0] * axis[1] + s * axis[2];
                mat_rotate[0][2] = temp[0] * axis[2] - s * axis[1];

                mat_rotate[1][0] = temp[1] * axis[0] - s * axis[2];
                mat_rotate[1][1] = c + temp[1] * axis[1];
                mat_rotate[1][2] = temp[1] * axis[2] + s * axis[0];

                mat_rotate[2][0] = temp[2] * axis[0] + s * axis[1];
                mat_rotate[2][1] = temp[2] * axis[1] - s * axis[0];
                mat_rotate[2][2] = c + temp[2] * axis[2];

                matrix[0] = matrix[0] * mat_rotate[0][0] + matrix[1] * mat_rotate[0][1] + matrix[2] * mat_rotate[0][2];
                matrix[1] = matrix[0] * mat_rotate[1][0] + matrix[1] * mat_rotate[1][1] + matrix[2] * mat_rotate[1][2];
                matrix[2] = matrix[0] * mat_rotate[2][0] + matrix[1] * mat_rotate[2][1] + matrix[2] * mat_rotate[2][2];
            }

            inline static void rot_euler(mat& matrix, const vec3& euler)
            {
                // Definitions from https://en.wikipedia.org/wiki/Euler_angles
                // Ordering: YXZ.

                // BTW Unity has it the other way around: ZXY
                // https://forum.unity.com/threads/rotation-order.13469/

                const sc::sincos sico_x = sc::sin_cos(euler.x);
                const sc::sincos sico_y = sc::sin_cos(euler.y);
                const sc::sincos sico_z = sc::sin_cos(euler.z);

                const f32 s1 = sico_x.sin;
                const f32 c1 = sico_x.cos;
                const f32 s2 = sico_y.sin;
                const f32 c2 = sico_y.cos;
                const f32 s3 = sico_z.sin;
                const f32 c3 = sico_z.cos;

                matrix[0][0] *= c1 * c3 + s1 * s2 * s3;
                matrix[0][1] = c2 * s3;
                matrix[0][2] = c1 * s2 * s3 - c3 * s1;
                matrix[1][0] = c3 * s1 * s2 - c1 * s3;
                matrix[1][1] *= c2 * c3;
                matrix[1][2] = c1 * c3 * s2 + s1 * s3;
                matrix[2][0] = c2 * s1;
                matrix[2][1] = -s2;
                matrix[2][2] *= c1 * c2;
            }

        public:

            void rotate(f32 angle, const vec3& rotation_axis)
            {
                rot(*this, angle, rotation_axis);
            }

            void rotate(const vec3& rotation_euler)
            {
                rot_euler(*this, rotation_euler);
            }

            void rescale(const vec3& scale)
            {
                for(size i = 0; i < scale.k_num_components && i < num_rows && i < num_cols; ++i)
                {
                    m_cols[i][i] *= scale[i];
                }
            }

            // Special-case creation functions.
            inline static mat identity()
            {
                return mat(1.0f);
            }

            static mat translation(const vec3& a_translation)
            {
                mat result(identity());
                static const size last_col = num_cols - 1;
                result[last_col][0] = a_translation[0];
                result[last_col][1] = a_translation[1];
                result[last_col][2] = a_translation[2];
                return result;
            }

            static mat rotation(f32 angle, const vec3& rotation_axis)
            {
                mat result(identity());
                rot(result, angle, rotation_axis);
                return result;
            }

            static mat rotation(const vec3& a_rotation_euler)
            {
                mat result(identity());
                rot_euler(result, a_rotation_euler);
                return result;
            }

            static mat scale(const vec3& a_scale)
            {
                mat result(identity());
                result[0][0] = a_scale[0];
                result[1][1] = a_scale[1];
                result[2][2] = a_scale[2];
                return result;
            }

            static mat transform(
                const vec3& a_position,
                const vec3& a_rotation = vec3(0.0f),
                const vec3& a_scale = vec3(1.0f))
            {
                mat result(scale(a_scale));
                result.rotate(a_rotation);
                result.translate(a_position);
                return result;
            }

            // Direction should be a normalized vector.
            static mat view_direction(
                const vec3& view_position,
                const vec3& view_direction,
                const vec3& up = vec3::up());

            static mat view_target(
                const vec3& view_position,
                const vec3& view_target_position,
                const vec3& up = vec3::up());
            
            static mat projection_perspective(
                f32 fov_y,
                f32 screen_width,
                f32 screen_height,
                f32 near_plane,
                f32 far_plane);

            static mat projection_ortho(
                f32 screen_width,
                f32 screen_height,
                f32 near_plane,
                f32 far_plane);

        protected:
            data::static_array<vec<k_num_rows>, k_num_cols> m_cols;
        };

        using mat4 = mat<4, 4>;
        using mat4x3 = mat<4, 3>;
        using mat3 = mat<3, 3>;

        template<>
        inline mat3 mat3::inversed() const
        {
            // From glm.

            const f32 det = (+m_cols[0][0] * (m_cols[1][1] * m_cols[2][2] - m_cols[2][1] * m_cols[1][2]) - m_cols[1][0] * (m_cols[0][1] * m_cols[2][2] - m_cols[2][1] * m_cols[0][2]) + m_cols[2][0] * (m_cols[0][1] * m_cols[1][2] - m_cols[1][1] * m_cols[0][2]));
            JE_assert(sc::is_almost_zero(det) == false, "Inverse matrix does not exist.");
            const f32 det_inv = 1.0f / det;

            mat3 inverse;

            inverse[0][0] = +(m_cols[1][1] * m_cols[2][2] - m_cols[2][1] * m_cols[1][2]) * det_inv;
            inverse[1][0] = -(m_cols[1][0] * m_cols[2][2] - m_cols[2][0] * m_cols[1][2]) * det_inv;
            inverse[2][0] = +(m_cols[1][0] * m_cols[2][1] - m_cols[2][0] * m_cols[1][1]) * det_inv;
            inverse[0][1] = -(m_cols[0][1] * m_cols[2][2] - m_cols[2][1] * m_cols[0][2]) * det_inv;
            inverse[1][1] = +(m_cols[0][0] * m_cols[2][2] - m_cols[2][0] * m_cols[0][2]) * det_inv;
            inverse[2][1] = -(m_cols[0][0] * m_cols[2][1] - m_cols[2][0] * m_cols[0][1]) * det_inv;
            inverse[0][2] = +(m_cols[0][1] * m_cols[1][2] - m_cols[1][1] * m_cols[0][2]) * det_inv;
            inverse[1][2] = -(m_cols[0][0] * m_cols[1][2] - m_cols[1][0] * m_cols[0][2]) * det_inv;
            inverse[2][2] = +(m_cols[0][0] * m_cols[1][1] - m_cols[1][0] * m_cols[0][1]) * det_inv;

            return inverse;
        }

        template<>
        inline mat4 mat4::inversed() const
        {
            // From glm.

            f32 coef_00 = m_cols[2][2] * m_cols[3][3] - m_cols[3][2] * m_cols[2][3];
            f32 coef_02 = m_cols[1][2] * m_cols[3][3] - m_cols[3][2] * m_cols[1][3];
            f32 coef_03 = m_cols[1][2] * m_cols[2][3] - m_cols[2][2] * m_cols[1][3];

            f32 coef_04 = m_cols[2][1] * m_cols[3][3] - m_cols[3][1] * m_cols[2][3];
            f32 coef_06 = m_cols[1][1] * m_cols[3][3] - m_cols[3][1] * m_cols[1][3];
            f32 coef_07 = m_cols[1][1] * m_cols[2][3] - m_cols[2][1] * m_cols[1][3];

            f32 coef_08 = m_cols[2][1] * m_cols[3][2] - m_cols[3][1] * m_cols[2][2];
            f32 coef_10 = m_cols[1][1] * m_cols[3][2] - m_cols[3][1] * m_cols[1][2];
            f32 coef_11 = m_cols[1][1] * m_cols[2][2] - m_cols[2][1] * m_cols[1][2];

            f32 coef_12 = m_cols[2][0] * m_cols[3][3] - m_cols[3][0] * m_cols[2][3];
            f32 coef_14 = m_cols[1][0] * m_cols[3][3] - m_cols[3][0] * m_cols[1][3];
            f32 coef_15 = m_cols[1][0] * m_cols[2][3] - m_cols[2][0] * m_cols[1][3];

            f32 coef_16 = m_cols[2][0] * m_cols[3][2] - m_cols[3][0] * m_cols[2][2];
            f32 coef_18 = m_cols[1][0] * m_cols[3][2] - m_cols[3][0] * m_cols[1][2];
            f32 coef_19 = m_cols[1][0] * m_cols[2][2] - m_cols[2][0] * m_cols[1][2];

            f32 coef_20 = m_cols[2][0] * m_cols[3][1] - m_cols[3][0] * m_cols[2][1];
            f32 coef_22 = m_cols[1][0] * m_cols[3][1] - m_cols[3][0] * m_cols[1][1];
            f32 coef_23 = m_cols[1][0] * m_cols[2][1] - m_cols[2][0] * m_cols[1][1];

            vec4 fac_0(coef_00, coef_00, coef_02, coef_03);
            vec4 fac_1(coef_04, coef_04, coef_06, coef_07);
            vec4 fac_2(coef_08, coef_08, coef_10, coef_11);
            vec4 fac_3(coef_12, coef_12, coef_14, coef_15);
            vec4 fac_4(coef_16, coef_16, coef_18, coef_19);
            vec4 fac_5(coef_20, coef_20, coef_22, coef_23);

            vec4 vec_0(m_cols[1][0], m_cols[0][0], m_cols[0][0], m_cols[0][0]);
            vec4 vec_1(m_cols[1][1], m_cols[0][1], m_cols[0][1], m_cols[0][1]);
            vec4 vec_2(m_cols[1][2], m_cols[0][2], m_cols[0][2], m_cols[0][2]);
            vec4 vec_3(m_cols[1][3], m_cols[0][3], m_cols[0][3], m_cols[0][3]);

            vec4 inv_0(vec_1 * fac_0 - vec_2 * fac_1 + vec_3 * fac_2);
            vec4 inv_1(vec_0 * fac_0 - vec_2 * fac_3 + vec_3 * fac_4);
            vec4 inv_2(vec_0 * fac_1 - vec_1 * fac_3 + vec_3 * fac_5);
            vec4 inv_3(vec_0 * fac_2 - vec_1 * fac_4 + vec_2 * fac_5);

            vec4 sign_a(+1, -1, +1, -1);
            vec4 sign_b(-1, +1, -1, +1);
            mat4 inverse(inv_0 * sign_a, inv_1 * sign_b, inv_2 * sign_a, inv_3 * sign_b);

            vec4 row_0(inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0]);

            vec4 dot_0(m_cols[0] * row_0);
            f32 dot_1 = (dot_0.x + dot_0.y) + (dot_0.z + dot_0.w);
            JE_assert(sc::is_almost_zero(dot_1) == false, "Inverse matrix does not exist.");

            f32 det_inv = 1.0f / dot_1;

            return inverse * det_inv;
        }

        template<>
        inline mat4x3 mat4x3::inversed() const
        {
            mat4 m4(*this);
            m4.inverse();
            return mat4x3(m4);
        }

        // Direction should be a normalized vector.
        template<>
        inline mat4 mat4::view_direction(
            const vec3& view_position,
            const vec3& view_direction,
            const vec3& up)
        {
            // Builds a left-handed view matrix - forward is positive Z.

            JE_assert(sc::is_almost_equal(view_direction.get_length_squared(), 1.0f), "view_direction is not a normalized vector.");
            JE_assert(sc::is_almost_equal(up.get_length_squared(), 1.0f), "up is not a normalized vector.");

            const vec3 view_right(vec3(vec3::cross(up, view_direction)).get_normalized());
            const vec3 view_up(vec3::cross(view_direction, view_right));

            mat result(mat::identity());
            result[0][0] = view_right.x;
            result[1][0] = view_right.y;
            result[2][0] = view_right.z;
            result[0][1] = view_up.x;
            result[1][1] = view_up.y;
            result[2][1] = view_up.z;
            result[0][2] = view_direction.x;
            result[1][2] = view_direction.y;
            result[2][2] = view_direction.z;
            result[0][3] = -vec3::dot(view_right, view_position);
            result[1][3] = -vec3::dot(view_up, view_position);
            result[2][3] = -vec3::dot(view_direction, view_position.z);
            return result;
        }

        template<>
        inline mat4 mat4::view_target(
            const vec3& view_position,
            const vec3& view_target_position,
            const vec3& up)
        {
            JE_assert(sc::is_almost_equal(up.get_length_squared(), 1.0f), "up is not a normalized vector.");

            const vec3 direction((view_target_position - view_position).get_normalized());
            return view_direction(view_position, direction, up);
        }

        template<>
        inline mat4 mat4::projection_perspective(
            f32 fov_y,
            f32 screen_width,
            f32 screen_height,
            f32 near_plane,
            f32 far_plane)
        {
            // From GLM.
            // This needs to be modified when support for other renderers is implemented.
            // Builds a left-handed projection matrix - forward is positive Z.
            // Builds a OpenGL clip space (-1, 1), also Z is multiplied by -1 for Vulkan purposes.
            
            JE_assert(fov_y > constants::k_epsilon, "fov_y is invalid.");
            JE_assert(screen_width > constants::k_epsilon && screen_height > constants::k_epsilon, "Screen width and height are invalid.");
            JE_assert(near_plane > constants::k_epsilon && far_plane > constants::k_epsilon && far_plane > near_plane, "Planes are invalid.");

            mat result(mat::identity());

            const f32 tan_half_fovy = tan(fov_y * 0.5f);
            const f32 aspect = screen_width / screen_height;
            const f32 plane_diff_rec = 1.0f / (far_plane - near_plane);

            result[0][0] = 1.0f / (aspect * tan_half_fovy);
            result[1][1] = -1.0f / (tan_half_fovy);
            result[2][2] = (far_plane + near_plane) * plane_diff_rec;
            result[2][3] = 1.0f;
            result[3][2] = -(2.0f * far_plane * near_plane) * plane_diff_rec;

            return result;
        }

        template<>
        inline mat4 mat4::projection_ortho(
            f32 screen_width,
            f32 screen_height,
            f32 near_plane,
            f32 far_plane)
        {
            // From GLM.
            // Builds a left-handed ortho matrix for OpenGL clip space (-1, 1), also Z is multiplied by -1 for Vulkan purposes.

            JE_assert(screen_width > constants::k_epsilon && screen_height > constants::k_epsilon, "Screen width and height are invalid.");
            JE_assert(near_plane > constants::k_epsilon && far_plane > constants::k_epsilon && far_plane > near_plane, "Planes are invalid.");

            const f32 plane_diff_rec = 1.0f / (far_plane - near_plane);

            const f32 right = screen_width * 0.5f;
            const f32 left = -right;
            const f32 top = screen_height * 0.5f;
            const f32 bottom = -top;

            mat result(mat::identity());
            result[0][0] = 2.0f / (right - left);
            result[1][1] = 2.0f / (top - bottom);
            result[2][2] = 2.0f * plane_diff_rec;
            result[3][0] = - (right + left) / (right - left);
            result[3][1] = - (top + bottom) / (top - bottom);
            result[3][2] = - (far_plane + near_plane) * plane_diff_rec;
            return result;
        }
    }
}