cmake_minimum_required(VERSION 3.0.0)
project(VulkanTutorialApp VERSION 0.1.0)

set(LIB_DIR ../../Libraries)
set(DEF_OS_LINUX OS_LINUX=1)
set(DEF_OS_WINDOWS OS_WINDOWS=1)

cmake_policy(SET CMP0076 NEW)

if(UNIX)
add_definitions(-D${DEF_OS_LINUX})
endif(UNIX)
if(WIN32)
add_definitions(-D${DEF_OS_WINDOWS})
endif(WIN32)

include_directories(
    ./
    ${LIB_DIR}/tinyobjloader
    ${LIB_DIR}/stb
    ${LIB_DIR}/glm-0.9.9-a2
    ${LIB_DIR}/glfw-3.2.1.bin.WIN64/include)

add_definitions(-include Core/GlobalIncludes.h)

link_directories(${LIB_DIR}/glfw-3.2.1.bin.WIN64/lib-linux)

find_package(X11 REQUIRED)

link_libraries(
    libglfw3.a 
    libvulkan.so
    libdl.so 
    libpthread.so
    ${X11_LIBRARIES})
#<PreBuildEvent>
#      <Command>CALL ./Scripts/CompileShaders.bat</Command>
#</PreBuildEvent>

add_executable(VulkanTutorialApp "")
add_subdirectory(Core)
add_subdirectory(GOM)
add_subdirectory(Rendering)
add_subdirectory(Util)

if(MSVC)
    add_definitions(/FI"Core/GlobalIncludes.h")
else()
    add_definitions(-include Core/GlobalIncludes.h)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
