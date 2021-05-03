#pragma once

#include "global.h"

#include <cmath>

#define JE_MATH_CHECK_NAN 1 && (JE_CONFIG_DEBUG)

#if JE_MATH_CHECK_NAN
#define JE_math_check_val(_val_) je::math::check::check_val(_val_)
#else
#define JE_math_check_val(_val_)
#endif

namespace je { namespace math {

    class check
    {
    public:

        static inline bool is_nan(f32 a)
        {
            return std::isnan(a);
        }

        static inline bool is_inf(f32 a)
        {
            return std::isinf(a);
        }

        template<typename num_type>
        static inline bool check_val(num_type a)
        {
            // Do not check anything by default.
            return true;
        }
    };

#if JE_MATH_CHECK_NAN
    template<>
    inline bool check::check_val(f32 a)
    {
        const bool val = (is_nan(a) || is_inf(a)) == false;
        JE_assert(val, "Floating-point value is invalid: [%f]", a);
        return val;
    }
#endif

}}