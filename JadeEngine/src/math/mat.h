#pragma once

#include "global.h"
#include "vec.h"

// This describes how data is stored in memory.
// In case of accessing the memory, matrices should be always row-major.
#define JE_MATH_MAT_ROW_MAJOR 1
#define JE_MATH_MAT_COL_MAJOR 0

#if (JE_MATH_MAT_ROW_MAJOR) + (JE_MATH_MAT_COL_MAJOR) != 1
#error Incorrect settings for matrices.
#endif

namespace je { namespace math {

    template<size_t num_cols, size_t num_rows>
    class mat
    {
    public:
        static const size_t k_num_cols = num_cols;
        static const size_t k_num_rows = num_rows;

#if JE_MATH_MAT_ROW_MAJOR
        vec<num_cols>& operator[](size_t index)
        {
            JE_assert(index < k_num_rows, "Invalid index.");
            return m_rows[index];
        }

        vec<num_cols> operator[](size_t index) const
        {
            JE_assert(index < k_num_rows, "Invalid index.");
            return m_rows[index];
        }
#else
        vec<num_rows>& operator[](size_t index)
        {
            JE_assert(index < k_num_cols, "Invalid index.");
            return m_rows[index];
        }

        vec<num_rows> operator[](size_t index) const
        {
            JE_assert(index < k_num_cols, "Invalid index.");
            return m_rows[index];
        }
#endif

    protected:
#if JE_MATH_MAT_ROW_MAJOR
        data::static_array<vec<num_cols>, k_num_rows> m_rows;
#else
        data::static_array<vec<num_rows>, k_num_cols> m_rows;
#endif
    };

    using mat4 = mat<4, 4>;
    using mat4x3 = mat<4, 3>;

}}