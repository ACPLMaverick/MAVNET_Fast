#pragma once

#include "global.h"
#include "gpu_params.h"
#include "mem/allocatable.h"

#define JE_GPU_DEBUG_LAYERS (JE_CONFIG_DEBUG)
#define JE_GPU_PROFILE_MARKERS (JE_CONFIG_DEBUG || JE_CONFIG_PROFILE)

namespace je { namespace window {
    class window;
}}

namespace je { namespace draw {

    class presenter;

    enum class queue_type : u8
    {
        k_graphics,
        k_compute,
        k_transfer,
        k_enum_size
    };

    class gpu : public mem::allocatable_persistent
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

        static gpu* create_gpu(const gpu_params& initializer);
        static void destroy_gpu(gpu* a_gpu);
        virtual ~gpu() {}

        // Creation functions.
        virtual presenter* create_presenter(const presenter_params& params) = 0;

    protected:

        gpu() {}
        
        virtual bool init(const gpu_params& initializer) = 0;
        virtual bool shutdown() = 0;

        virtual const char* get_name() = 0;

        bool has_capabilities(capabilities a_caps) const { return (static_cast<u32>(m_capabilities) & static_cast<u32>(a_caps)) == static_cast<u32>(a_caps); }

    protected:

        capabilities m_capabilities;

    };

}}