#include "rand.h"
#include "constants.h"
#include "sc.h"
#include "limits.h"

#include <random>

namespace je { namespace math {

    using random_engine_type = std::mt19937;
    template <typename int_type> using random_int_distribution_type = std::uniform_int_distribution<int_type>;
    template <typename float_type> using random_float_distribution_type = std::uniform_real_distribution<float_type>;

    static random_engine_type& get_engine()
    {
        static std::random_device device;
        static random_engine_type engine(device());
        return engine;
    }

    template<typename num_type>
    num_type rand::generate_internal(num_type a_min, num_type a_max)
    {
        static random_int_distribution_type<num_type> distribution;
        using prm_type = typename decltype(distribution)::param_type;
        return distribution(get_engine(), prm_type{a_min, a_max});
    }

    template<>
    float rand::generate_internal(float a_min, float a_max)
    {
        JE_math_check_val(a_min);
        JE_math_check_val(a_max);
        static random_float_distribution_type<float> distribution;
        using prm_type = decltype(distribution)::param_type;
        return distribution(get_engine(), prm_type{a_min, a_max});
    }

    template<>
    double rand::generate_internal(double a_min, double a_max)
    {
        JE_math_check_val(a_min);
        JE_math_check_val(a_max);
        static random_float_distribution_type<double> distribution;
        using prm_type = decltype(distribution)::param_type;
        return distribution(get_engine(), prm_type{a_min, a_max});
    }

#define JE_MATH_RAND_GENERATE_DECL(_type_) template _type_ rand::generate_internal(_type_, _type_)

    JE_MATH_RAND_GENERATE_DECL(uint64_t);
    JE_MATH_RAND_GENERATE_DECL(int64_t);
    JE_MATH_RAND_GENERATE_DECL(uint32_t);
    JE_MATH_RAND_GENERATE_DECL(int32_t);
    JE_MATH_RAND_GENERATE_DECL(uint16_t);
    JE_MATH_RAND_GENERATE_DECL(int16_t);
    JE_MATH_RAND_GENERATE_DECL(uint8_t);
    JE_MATH_RAND_GENERATE_DECL(int8_t);

#undef JE_MATH_RAND_GENERATE_DECL
}}