#pragma once

namespace je { namespace math {

    class constants
    {
    public:

        static constexpr const f32 k_pi = 3.14159265358f;
        static constexpr const f32 k_pi_2 = k_pi * 0.5f;
        static constexpr const f32 k_pi_4 = k_pi * 0.25f;
        static constexpr const f32 k_2_pi = 2 * k_pi;
        static constexpr const f32 k_inv_pi = 1.0f / k_pi;
        static constexpr const f32 k_inv_pi_2 = 1.0f / k_pi_2;
        static constexpr const f32 k_inv_pi_4 = 1.0f / k_pi_4;
        static constexpr const f32 k_inv_2_pi = 1.0f / k_2_pi;

        static constexpr const f32 k_epsilon = 1.e-4f;
        static constexpr const f32 k_float_non_fractional = 8388608.0f;

    };

}}