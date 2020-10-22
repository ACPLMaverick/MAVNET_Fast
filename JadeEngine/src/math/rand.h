#pragma once

#include "global.h"
#include "scalar.h"
#include "limits.h"

namespace je { namespace math {

    class rand
    {
    public:

        template<typename num_type>
        inline static num_type generate()
        {
            return generate_internal(limits<num_type>::min(), limits<num_type>::max());
        }

        template<typename num_type>
        inline static num_type generate(num_type max)
        {
            return generate_internal(0, max);
        }

        template<typename num_type>
        inline static num_type generate(num_type min, num_type max)
        {
            return generate_internal(min, max);
        }

        template<typename num_type>
        inline static num_type generate_0_1()
        {
            return generate<num_type>(0, 1);
        }
        
    private:

        template<typename num_type>
        static num_type generate_internal(num_type min, num_type max);
    };

}}