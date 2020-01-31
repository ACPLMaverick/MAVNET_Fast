# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
cmake_policy(SET CMP0076 NEW)

project (avrlib VERSION 1.0.0)
set(LIB_NAME avr)

# Default library elements usage definition.
if(NOT DEFINED LIB_USE_BLINKER)
set(LIB_USE_BLINKER 1)
endif()
if(NOT DEFINED LIB_USE_UART)
set(LIB_USE_UART 1)
endif()
if(NOT DEFINED LIB_USE_TIMER)
set(LIB_USE_TIMER 1)
endif()
if(NOT DEFINED LIB_USE_DISPLAY)
set(LIB_USE_DISPLAY 1)
endif()

# Default compilation config elements.
if(NOT DEFINED LIB_ENABLE_ASSERTS)
set(LIB_ENABLE_ASSERTS 1)
endif()

# Default AVR Toolchain definitions. These are valid for ATMega8.
if(NOT DEFINED F_CPU)
set(F_CPU 12000000ULL)
endif()
if(NOT DEFINED MCU)
set(MCU m8)
endif()
if(NOT DEFINED MCU_FOR_COMPILER)
set(MCU_FOR_COMPILER atmega8)
endif()
if(NOT DEFINED MCU_FOR_HEADER)
set(MCU_FOR_HEADER __AVR_ATmega8A__)
endif()
if(NOT DEFINED BAUD)
set(BAUD 9600) 
endif()

# Default compiler definitions.
if(NOT DEFINED CMAKE_SYSTEM_NAME)
set(CMAKE_SYSTEM_NAME Generic)
endif()
if(NOT DEFINED CMAKE_C_COMPILER)
set(CMAKE_C_COMPILER avr-gcc)
endif()
if(NOT DEFINED CMAKE_ASM_COMPILER)
set(CMAKE_ASM_COMPILER avr-gcc)
endif()

# Linker options defined.
set(CMAKE_EXE_LINKER_FLAGS -mmcu=${MCU_FOR_COMPILER})

# Compile options defined.
add_compile_options(
	-mmcu=${MCU_FOR_COMPILER}
	-std=gnu99
	-Os	# TODO Make this kit-specific.
	-Wall	# Enable warnings
	#-Wno-main
	-Wundef
	-pedantic
	-Wstrict-prototypes
	-Werror
	-Wfatal-errors
	-Wl,--relax,--gc-sections
	-g	# Debugger format setting
	-gdwarf-2
	-funsigned-char # a few optimizations
    -funsigned-bitfields
    -fpack-struct
    -fshort-enums
    -ffunction-sections
    -fdata-sections
    -fno-split-wide-types
    -fno-tree-scev-cprop
)

# Custom compiler definitions.
add_definitions(
	-DF_CPU=${F_CPU}
	-DBAUD=${BAUD}
	-DLIB_USE_BLINKER=${LIB_USE_BLINKER}
	-DLIB_USE_UART=${LIB_USE_UART}
	-DLIB_USE_DISPLAY=${LIB_USE_DISPLAY}
	-DLIB_USE_TIMER=${LIB_USE_TIMER}
	-DLIB_ENABLE_ASSERTS=${LIB_ENABLE_ASSERTS}
)

# Include sub-projects.
add_library (${LIB_NAME} "")

# This is kinda a hack I use to force cmake use this CMakeFile's dir as a root of all internal directories
# when this list file is called from another project.
set(WDIR ${CMAKE_CURRENT_LIST_DIR})

# Default config path.
if(NOT DEFINED LIB_CONFIG_PATH)
set(LIB_CONFIG_PATH ${WDIR}/config_default)
endif()

# Include paths.
if (WIN32)
else()
target_include_directories(${LIB_NAME} PUBLIC
	/usr/lib/avr/include
	${WDIR}/include
	${LIB_CONFIG_PATH}
)
endif()

target_compile_definitions(${LIB_NAME} PUBLIC
	${MCU_FOR_HEADER}
)

add_subdirectory (${LIB_CONFIG_PATH} ${WDIR}/build/config)
add_subdirectory (${WDIR}/include ${WDIR}/build/include)
add_subdirectory (${WDIR}/src ${WDIR}/build/src)

# Default output directory definition.
if(NOT DEFINED LIB_OUTPUT_PATH)
set(LIB_OUTPUT_PATH ${WDIR}/lib)
endif()

# Set output path.
set_target_properties( ${LIB_NAME}
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_PATH}
	LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_PATH}
	RUNTIME_OUTPUT_DIRECTORY ${LIB_OUTPUT_PATH}
)
