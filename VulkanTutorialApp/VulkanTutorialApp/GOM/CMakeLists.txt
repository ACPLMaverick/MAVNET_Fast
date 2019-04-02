add_subdirectory(system)

target_sources(VulkanTutorialApp
    PRIVATE    
		GlobalWorldParameters.cpp
        Entity.cpp
		World.cpp

		GlobalWorldParameters.h
        Entity.h
		World.h
)