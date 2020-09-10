file(GLOB HERE_SOURCES 
       ./*.cpp
)

file(GLOB_RECURSE GENERAL_SOURCES
       data/*.cpp
       draw/*.cpp
       mem/*.cpp
       tests/*.cpp
       util/*.cpp
)

file(GLOB PLATFORM_SOURCES_A
       platf/*.cpp
)

if(JE_PLATFORM STREQUAL ${PLATFORM_NAME_WINDOWS})
       file(GLOB_RECURSE PLATFORM_SOURCES_B
              platf/windows/*.cpp
       )
elseif(JE_PLATFORM STREQUAL ${PLATFORM_NAME_LINUX})
       file(GLOB_RECURSE PLATFORM_SOURCES_B
              platf/linux/*.cpp
       )
else()
	message(FATAL_ERROR "Wrong platform [${JE_PLATFORM}] or no platform specified. Cannot proceed further.")
endif()

target_sources(${EXE_NAME} PRIVATE 
       ${HERE_SOURCES}
       ${GENERAL_SOURCES}
       ${PLATFORM_SOURCES_A}
       ${PLATFORM_SOURCES_B}
)