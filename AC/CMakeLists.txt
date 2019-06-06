# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (AC VERSION 1.0.0)
set(EXE_NAME ac)

# AVR Toolchain definitions. These are valid for ATMega8.
set(F_CPU 8000000ULL)
set(MCU m8)
set(MCU_FOR_COMPILER atmega8)
set(BAUD 9600) 
set(PROGRAMMER usbasp)

# TODO Fuse setup can go here. For now it's not necessary.

# Compiler definitions.
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_ASM_COMPILER avr-gcc)

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

# Custom compiler definitions. Defining this in defs.h
add_definitions(
	-DF_CPU=${F_CPU}
	-DBAUD=${BAUD}
)

# Include sub-projects.
add_executable (${EXE_NAME} "")
add_subdirectory (src)

# Set output path.
set(UNIFIED_OUTPUT_PATH bin)
set_target_properties( ${EXE_NAME}
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${UNIFIED_OUTPUT_PATH}
	LIBRARY_OUTPUT_DIRECTORY ${UNIFIED_OUTPUT_PATH}
	RUNTIME_OUTPUT_DIRECTORY ${UNIFIED_OUTPUT_PATH}
)

# Rename the output to .elf as we will create multiple files.
set_target_properties(${EXE_NAME} PROPERTIES OUTPUT_NAME ${EXE_NAME}.elf)

set(FIN_EXE_NAME ${UNIFIED_OUTPUT_PATH}/${EXE_NAME})

# Strip binary for upload. (TODO Only in Debug?)
add_custom_target(target_strip ALL avr-strip ${FIN_EXE_NAME}.elf DEPENDS ${EXE_NAME})

# Print size...
add_custom_target(target_print_size ALL avr-size -C --mcu=${MCU_FOR_COMPILER} ${FIN_EXE_NAME}.elf DEPENDS target_strip)

# Transform binary into hex file and strip eeprom part.
add_custom_target(target_hex ALL avr-objcopy -R .eeprom -O ihex ${FIN_EXE_NAME}.elf ${FIN_EXE_NAME}.hex DEPENDS target_strip)
# Transform binary into eep file - EEPROM static variables. TODO Disabled as not necessary. I have no idea what exactly these parameters do atm.
# add_custom_target(target_eep ALL avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc, load" --change-section-lma .eeprom=0 -O ihex ${FIN_EXE_NAME}.elf ${FIN_EXE_NAME}.eep DEPENDS target_strip)

# Upload the program with avrdude.
add_custom_target(target_upload_hex ALL avrdude -c ${PROGRAMMER} -p ${MCU} -U flash:w:${FIN_EXE_NAME}.hex DEPENDS target_hex)

# Upload the eeprom with avrdude. TODO Disabled as not necessary.
# add_custom_target(target_upload_eep ALL avrdude -c ${PROGRAMMER} -p ${MCU}  -U eeprom:w:${FIN_EXE_NAME}.eep DEPENDS target_eep)

# TODO This may be used to burn fuses in the future.
# add_custom_target(target_fuses avrdude ALL -c ${PROGRAMMER} -p ${MCU} -U lfuse:w:${FUSE_L}:m -U hfuse:w:${FUSE_H}:m)