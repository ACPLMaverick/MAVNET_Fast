#pragma once

#include "global.h"
#include "mem/allocatable.h"

namespace je { namespace draw { namespace gpu {

    class device; struct device_params;
    class presenter; struct presenter_params;
    class shader; struct shader_params;

    class factory : public mem::allocatable_persistent
    {
    public:

        // This is NOT a thread-safe singleton for now.
        static factory& get();
        static device* init(const device_params& params);
        static void destroy();

        virtual presenter* create_presenter(const presenter_params& params) = 0;
        virtual shader* create_shader(const shader_params& params) = 0;

        template<class destroyed_type>
        void destroy(destroyed_type* a_object)
        {
            JE_safe_delete(a_object, a_object->shutdown(*m_device));
        }

    protected:

        factory(device* a_device)
            : m_device(a_device)
        {
        }

        virtual ~factory() {}

        template<class base_type, class created_type, class params_type>
        base_type* create_gpu_object(const params_type& a_params)
        {
            created_type* object = new created_type(a_params);
            if(object->init(*m_device, a_params))
            {
                return object;
            }
            else
            {
                object->shutdown(*m_device);
                delete object;
                return nullptr;
            }
        }

    protected:

        static factory* s_inst;
        device* m_device;

    };

}}}