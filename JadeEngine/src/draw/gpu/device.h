#pragma once

#include "global.h"
#include "params.h"
#include "mem/allocatable.h"

#define JE_GPU_DEBUG_LAYERS (JE_CONFIG_DEBUG)
#define JE_GPU_PROFILE_MARKERS (JE_CONFIG_DEBUG || JE_CONFIG_PROFILE)

namespace je { namespace window {
    class window;
}}

namespace je { namespace draw { namespace gpu {

    class factory;

    enum class queue_type : u8
    {
        k_graphics,
        k_compute,
        k_transfer,
        k_enum_size
    };

    class device : public mem::allocatable_persistent
    {
    public:

        enum class capabilities : u32
        {
            k_none = 0,
            k_debug = (1 << 0),
            k_transform_feedback = (1 << 1),
            k_compute_pipeline = (1 << 2),
            k_compute_queue = (1 << 3),
            k_transfer_queue = (1 << 4),
            k_tesselation = (1 << 5),
            k_geometry_shading = (1 << 6),
            k_raytracing = (1 << 7),
            k_optimus = (1 << 8),
            k_dedicated = (1 << 9)
        };

    public:

        static device* create_device(const device_params& initializer);
        static void destroy_device(device* a_device);

    protected:

        device(const device_params& initializer) {}
        virtual ~device() {}
        
        virtual bool init(const device_params& initializer) = 0;
        virtual void shutdown() = 0;

        virtual const char* get_name() = 0;

        bool has_capabilities(capabilities a_caps) const { return (static_cast<u32>(m_capabilities) & static_cast<u32>(a_caps)) == static_cast<u32>(a_caps); }

        template<class base_type, class created_type, class params_type>
        base_type* create_gpu_object(const params_type& a_params)
        {
            created_type* object = new created_type(a_params);
            if(object->init(*this, a_params))
            {
                return object;
            }
            else
            {
                object->shutdown(*this);
                delete object;
                return nullptr;
            }
        }

    protected:

        capabilities m_capabilities;

    protected:

        friend class factory;

    };

}}}