# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("FTPSync")

# Include sub-projects.
add_subdirectory ("FTPSync")

set_target_properties( FTPSync
	PROPERTIES
    # RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
	RUNTIME_OUTPUT_DIRECTORY "E:\\MinecraftServer"
)