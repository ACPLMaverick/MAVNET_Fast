add_subdirectory(resource)

target_sources(VulkanTutorialApp
    PRIVATE    
        main.cpp
        GlobalIncludes.cpp
        HelloTriangle.cpp
        ManagerUid.cpp
        stb_impl.cpp
        tiny_obj_loader_impl.cpp
		Timer_LINUX.cpp
		Timer_WINDOWS.cpp
		Timer.cpp

        GlobalIncludes.h
        HelloTriangle.h
        ManagerUid.h
		Timer.h
)