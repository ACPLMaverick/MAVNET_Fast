add_subdirectory(system)

target_sources(VulkanTutorialApp
    PRIVATE    
        Entity.cpp
		World.cpp

        Entity.h
		World.h
)