file(GLOB HERE_SOURCES 
       ./*.cpp
)

file(GLOB_RECURSE GENERAL_SOURCES
       data/*.cpp
       fs/*.cpp
       math/*.cpp
       mem/*.cpp
       tests/*.cpp
       thread/*.cpp
       util/*.cpp
       window/*.cpp
)

file(GLOB DRAW_SOURCES
       draw/*.cpp
       draw/gpu/*.cpp
)

if(JE_GPU_VULKAN EQUAL 1)
       file(GLOB_RECURSE DRAW_GPU_VULKAN_SOURCES
              draw/gpu/vulkan/*.cpp
       )
else()
       set(DRAW_GPU_VULKAN_SOURCES )
endif()

if(JE_GPU_GLES EQUAL 1)
       file(GLOB_RECURSE DRAW_GPU_GLES_SOURCES
              draw/gpu/gles/*.cpp
       )
else()
       set(DRAW_GPU_GLES_SOURCES )
endif()

if(JE_GPU_D3D12 EQUAL 1)
       file(GLOB_RECURSE DRAW_GPU_D3D12_SOURCES
              draw/gpu/d3d12/*.cpp
       )
else()
       set(DRAW_GPU_D3D12_SOURCES )
endif()

file(GLOB PLATFORM_SOURCES_A
       platform/*.cpp
)

if(JE_PLATFORM STREQUAL ${PLATFORM_NAME_WINDOWS})
       file(GLOB_RECURSE PLATFORM_SOURCES_B
              platform/windows/*.cpp
       )
elseif(JE_PLATFORM STREQUAL ${PLATFORM_NAME_LINUX})
       file(GLOB_RECURSE PLATFORM_SOURCES_B
              platform/linux/*.cpp
       )
else()
	message(FATAL_ERROR "Wrong platform [${JE_PLATFORM}] or no platform specified. Cannot proceed further.")
endif()

target_sources(${EXE_NAME} PRIVATE 
       ${HERE_SOURCES}
       ${GENERAL_SOURCES}
       ${DRAW_SOURCES}
       ${DRAW_GPU_VULKAN_SOURCES}
       ${DRAW_GPU_GLES_SOURCES}
       ${DRAW_GPU_D3D12_SOURCES}
       ${PLATFORM_SOURCES_A}
       ${PLATFORM_SOURCES_B}
)