#include "dev.h"

#if JE_GPU_D3D12
#include "gles/dev_d3d12.h"
#endif // JE_GPU_D3D12
#if JE_GPU_VULKAN
#include "vulkan/dev_vulkan.h"
#endif // JE_GPU_VULKAN
#if JE_GPU_GLES
#include "gles/dev_gles.h"
#endif // JE_GPU_GLES

namespace je { namespace draw { namespace gpu {

    dev* dev::create_dev(const dev_params& a_initializer)
    {
        dev* device = nullptr;
#if JE_GPU_D3D12
        device = new dev_d3d12();
        if(device->init(a_initializer))
        {
            return device;
        }
        else
        {
            JE_print("Rendering backend not supported: [%s].", device->get_name());
            JE_verify(device->shutdown(), "Failed destroying GPU which in turn failed to create.");
            JE_safe_delete(device);
        }
#endif // JE_GPU_D3D12

#if JE_GPU_VULKAN
        device = new dev_vulkan();
        if(device->init(a_initializer))
        {
            return device;
        }
        else
        {
            JE_print("Rendering backend not supported: [%s].", device->get_name());
            JE_verify(device->shutdown(), "Failed destroying GPU which in turn failed to create.");
            JE_safe_delete(device);
        }
#endif // JE_GPU_VULKAN

#if JE_GPU_GLES
        device = new dev_gles();
        if(device->init(a_initializer))
        {
            return device;
        }
        else
        {
            JE_print("Rendering backend not supported: [%s].", device->get_name());
            JE_verify(device->shutdown(), "Failed destroying GPU which in turn failed to create.");
            JE_safe_delete(device);
        }
#endif // JE_GPU_GLES

        if(device != nullptr)
        {
            JE_print("Created rendering backend using [%s].", device->get_name());
        }

        return device;
    }

    void dev::destroy_dev(dev* a_dev)
    {
        JE_safe_delete(a_dev, a_dev->shutdown());
    }

}}}