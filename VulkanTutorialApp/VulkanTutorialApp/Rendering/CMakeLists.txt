add_subdirectory(command)
add_subdirectory(descriptor)
add_subdirectory(pipeline)
add_subdirectory(renderPass)
add_subdirectory(renderStep)
add_subdirectory(resource)
add_subdirectory(sampler)

target_sources(VulkanTutorialApp
    PRIVATE    
        Camera.cpp
        Fog.cpp
		Framebuffer.cpp
        Helper.cpp
        LightDirectional.cpp
        SystemDrawable.cpp

        Camera.h
        Fog.h
		Framebuffer.h
        Helper.h
        LightDirectional.h
        SystemDrawable.h
        Manager.h
)