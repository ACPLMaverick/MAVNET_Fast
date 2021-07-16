#include "factory.h"

#if JE_GPU_D3D12
#include "gles/device_d3d12.h"
#endif // JE_GPU_D3D12
#if JE_GPU_VULKAN
#include "vulkan/device_vulkan.h"
#include "vulkan/presenter_vulkan.h"
#include "vulkan/shader_vulkan.h"
#endif // JE_GPU_VULKAN
#if JE_GPU_GLES
#include "gles/device_gles.h"
#endif // JE_GPU_GLES

namespace je { namespace draw { namespace gpu {

    // Implementations.

#define JE_GPU_factory_impl_func(_name_, _backend_) virtual _name_* create_##_name_(const _name_##_params& a_params) override { return create_gpu_object<_name_, _name_##_##_backend_, _name_##_params>(a_params); }
#define JE_GPU_factory_impl(_backend_)                              \
class factory_##_backend_ : public factory                          \
{                                                                   \
public:                                                             \
                                                                    \
    factory_##_backend_(device* a_device) : factory(a_device) {}    \
                                                                    \
    JE_GPU_factory_impl_func(presenter, _backend_)                  \
    JE_GPU_factory_impl_func(shader, _backend_)                     \
};

#if JE_GPU_D3D12
JE_GPU_factory_impl(d3d12)
#endif // JE_GPU_D3D12
#if JE_GPU_VULKAN
JE_GPU_factory_impl(vulkan)
#endif // JE_GPU_VULKAN
#if JE_GPU_GLES
JE_GPU_factory_impl(gles)
#endif // JE_GPU_GLES

    // General functions.

    factory& factory::get()
    {
        JE_assert(s_inst != nullptr, "Factory has not been initialized!");
        return *s_inst;
    }

    device* factory::init(const device_params& a_params)
    {
        device* device = nullptr;
#if JE_GPU_D3D12
        device = new device_d3d12(a_params);
        if(device->init(a_params))
        {
            s_inst = new factory_d3d12(device);
        }
        else
        {
            JE_print("Rendering backend not supported: [%s].", device->get_name());
            JE_safe_delete(device);
        }
#endif // JE_GPU_D3D12

#if JE_GPU_VULKAN
        device = new device_vulkan(a_params);
        if(device->init(a_params))
        {
            s_inst = new factory_vulkan(device);
        }
        else
        {
            JE_print("Rendering backend not supported: [%s].", device->get_name());
            JE_safe_delete(device);
        }
#endif // JE_GPU_VULKAN

#if JE_GPU_GLES
        device = new device_gles(a_params);
        if(device->init(a_params))
        {
            s_inst = new factory_gles(device);
        }
        else
        {
            JE_print("Rendering backend not supported: [%s].", device->get_name());
            JE_safe_delete(device);
        }
#endif // JE_GPU_GLES

        if(device != nullptr)
        {
            JE_print("Created rendering backend using [%s].", device->get_name());
        }
        return device;
    }

    void factory::destroy()
    {
        JE_safe_delete(get().m_device, get().m_device->shutdown());
        JE_safe_delete(s_inst);
    }

    factory* factory::s_inst(nullptr);

}}}