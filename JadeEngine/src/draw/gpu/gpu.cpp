#include "gpu.h"

#if JE_GPU_D3D12
#include "gles/gpu_d3d12.h"
#endif // JE_GPU_D3D12
#if JE_GPU_VULKAN
#include "vulkan/gpu_vulkan.h"
#endif // JE_GPU_VULKAN
#if JE_GPU_GLES
#include "gles/gpu_gles.h"
#endif // JE_GPU_GLES

namespace je { namespace draw {

    gpu* gpu::create_gpu(const gpu_params& a_initializer)
    {
        gpu* device = nullptr;
#if JE_GPU_D3D12
        device = new gpu_d3d12();
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
        device = new gpu_vulkan();
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
        device = new gpu_gles();
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

    void gpu::destroy_gpu(gpu* a_gpu)
    {
        JE_safe_delete(a_gpu, a_gpu->shutdown());
    }

}}