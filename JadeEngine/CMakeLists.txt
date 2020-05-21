# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.17.2 FATAL_ERROR)

set(PROJECT_NAME JadeEngine)
project(${PROJECT_NAME} VERSION 1.0.0)
message("Building ${PROJECT_NAME}.")

# Argument parsing
set(PLATFORM_NAME_WINDOWS Windows)
set(PLATFORM_NAME_LINUX Linux)
set(CONFIGURATION_NAME_DEBUG Debug)
set(CONFIGURATION_NAME_PROFILE Profile)
set(CONFIGURATION_NAME_RELEASE Release)

if(JE_PLATFORM STREQUAL ${PLATFORM_NAME_WINDOWS})
elseif(JE_PLATFORM STREQUAL ${PLATFORM_NAME_LINUX})
else()
	message(FATAL_ERROR "Wrong platform [${JE_PLATFORM}] or no platform specified. Cannot proceed further.")
endif()
message("Platform: ${JE_PLATFORM}.")

if(JE_CONFIGURATION STREQUAL ${CONFIGURATION_NAME_DEBUG})
elseif(JE_CONFIGURATION STREQUAL ${CONFIGURATION_NAME_PROFILE})
elseif(JE_CONFIGURATION STREQUAL ${CONFIGURATION_NAME_RELEASE})
else()
	message("Wrong configuration [${JE_CONFIGURATION}] or no configuration specified. Switching to default one.")
	set(JE_CONFIGURATION ${CONFIGURATION_NAME_RELEASE})
endif()
message("Configuration: ${JE_CONFIGURATION}.")

# Global definitions
set(RUNTIME_WORK_DIR ../bin)	# Adding back here because this is relative to the build directory.
set(SOURCE_DIR src)

set(EXE_NAME JadeEngine_${JE_PLATFORM}_${JE_CONFIGURATION})

# Toolchain definitions
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# Build system
add_executable(${EXE_NAME} "")
add_subdirectory (${SOURCE_DIR})

# Platform-specific definitions, compiler flags, libs, etc.
if(JE_PLATFORM STREQUAL ${PLATFORM_NAME_WINDOWS})
	target_compile_definitions(${EXE_NAME} PUBLIC
		JE_PLATFORM_WINDOWS=1
		JE_PLATFORM_LINUX=0
	)

	target_include_directories(${EXE_NAME} PUBLIC
		"C:\\Program Files\\LLVM\\lib\\clang\\10.0.0\\include"
		"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\MSVC\\14.25.28610\\include"
		"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\MSVC\\14.25.28610\\atlmfc\\include"
		"C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.18362.0\\ucrt"
		"C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.18362.0\\shared"
		"C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.18362.0\\um"
		"C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.18362.0\\winrt"
	)

	target_link_directories(${EXE_NAME} PUBLIC
		"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\MSVC\\14.25.28610\\lib\\x64"
		"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\MSVC\\14.25.28610\\atlmfc\\lib\\x64"
		"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.18362.0\\ucrt\\x64"
		"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.18362.0\\um\\x64"
	)
elseif(JE_PLATFORM STREQUAL {PLATFORM_NAME_LINUX})
	target_compile_definitions(${EXE_NAME} PUBLIC
		JE_PLATFORM_WINDOWS=0
		JE_PLATFORM_LINUX=1
	)

	message(FATAL_ERROR "TODO: Include directories for Linux not specified.")
	message(FATAL_ERROR "TODO: Library directories for Linux not specified.")
endif()

# Configuration-specific definitions, compiler flags, libs, etc.
if(JE_CONFIGURATION STREQUAL ${CONFIGURATION_NAME_DEBUG})
	target_compile_definitions(${EXE_NAME} PUBLIC
		JE_CONFIG_DEBUG=1
		JE_CONFIG_PROFILE=0
		JE_CONFIG_RELEASE=0
	)

	add_compile_options(
		-O0
	)
elseif(JE_CONFIGURATION STREQUAL ${CONFIGURATION_NAME_PROFILE})
	target_compile_definitions(${EXE_NAME} PUBLIC
		JE_CONFIG_DEBUG=0
		JE_CONFIG_PROFILE=1
		JE_CONFIG_RELEASE=0
	)

	add_compile_options(
		-O2
	)
elseif(JE_CONFIGURATION STREQUAL ${CONFIGURATION_NAME_RELEASE})
	target_compile_definitions(${EXE_NAME} PUBLIC
		JE_CONFIG_DEBUG=0
		JE_CONFIG_PROFILE=0
		JE_CONFIG_RELEASE=1
	)

	add_compile_options(
		-O2
	)
endif()

# Global include directories
target_include_directories(${EXE_NAME} PUBLIC
	${SOURCE_DIR}
)

# Global compiler flags
add_compile_options(
	-std=c++17
	-Wall	# Enable warnings
	-Wundef
	-pedantic
	-Wstrict-prototypes
	-Werror
	-Wfatal-errors
	-Wl,--relax,--gc-sections
)

# Global libraries

# Output configuration
set_target_properties(${EXE_NAME}
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${RUNTIME_WORK_DIR}
	LIBRARY_OUTPUT_DIRECTORY ${RUNTIME_WORK_DIR}
	RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_WORK_DIR}
)