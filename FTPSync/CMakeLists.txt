# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (FTPSync VERSION 1.0.0)

if(WIN32)
set(UnifiedOutputPath "E:\\MinecraftServer")

link_libraries(
	Ws2_32.lib
)

endif(WIN32)

if(UNIX)
set(UnifiedOutputPath "/media/maverick/GAM/MinecraftServer")
endif(UNIX)
# "${PROJECT_SOURCE_DIR}/bin"

include_directories(
	./
)

# Include sub-projects.
add_executable (FTPSync "")
add_subdirectory (FTPSync)
add_subdirectory (FtpClient)

set_target_properties( FTPSync
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${UnifiedOutputPath}
	LIBRARY_OUTPUT_DIRECTORY ${UnifiedOutputPath}
	RUNTIME_OUTPUT_DIRECTORY ${UnifiedOutputPath}
)