# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
cmake_policy(SET CMP0076 NEW)

project (AC VERSION 1.0.0)
set(EXE_NAME ac)

# Particular library elements usage definition.
set(LIB_USE_BLINKER 1)
set(LIB_USE_UART 1)
set(LIB_USE_TIMER 1)
set(LIB_USE_DISPLAY 1)

# Lib config directory.
set (LIB_CONFIG_PATH config_avrlib)

# Lib output path definition.
set(LIB_OUTPUT_PATH bin)

# AVR Toolchain definitions. These are valid for ATMega8.
set(F_CPU 12000000ULL)
set(MCU m8)
set(MCU_FOR_COMPILER atmega8)
set(MCU_FOR_HEADER __AVR_ATmega8A__)
set(BAUD 9600) 
set(PROGRAMMER usbasp)

# Output definitions
set(OUTPUT_ENABLED false)
set(OUTPUT_APP cat)
set(OUTPUT_DEVICE /dev/ttyUSB0)
set(OUTPUT_TERMINAL /dev/pts/1)

# Fuse setup.
# Fuses OK (E:FF, H:D9, L:EF)
set(FUSE_L 0xEF)
set(FUSE_H 0xD9)

# All necessary compiler definitions and compile options are added by lib.

# Build avrlib with given parameters.
include(../avrlib/CMakeLists.txt)

# Include sub-projects.
add_executable (${EXE_NAME} "")

# Include paths.
target_include_directories(${EXE_NAME} PUBLIC
	../avrlib/include
	config_avrlib
)

target_link_libraries(${EXE_NAME} PUBLIC
	${CMAKE_SOURCE_DIR}/build/bin/libavr.a
)

add_subdirectory (src)

IF (WIN32)
ELSE()
target_include_directories(${EXE_NAME} PUBLIC
	/usr/lib/avr/include
)
ENDIF()

target_compile_definitions(${EXE_NAME} PUBLIC
	${MCU_FOR_HEADER}
)

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

# Burn fuses.
add_custom_target(target_fuses ALL avrdude -c ${PROGRAMMER} -p ${MCU} -U lfuse:w:${FUSE_L}:m -U hfuse:w:${FUSE_H}:m DEPENDS target_hex)

# Upload the program with avrdude.
add_custom_target(target_upload_hex ALL avrdude -c ${PROGRAMMER} -p ${MCU} -U flash:w:${FIN_EXE_NAME}.hex DEPENDS target_fuses)

# Upload the eeprom with avrdude. TODO Disabled as not necessary.
# add_custom_target(target_upload_eep ALL avrdude -c ${PROGRAMMER} -p ${MCU}  -U eeprom:w:${FIN_EXE_NAME}.eep DEPENDS target_eep)

IF(${OUTPUT_ENABLED})
# message("\n\n ====================================== \n Output from UART:\n")
add_custom_target(target_output ALL ${OUTPUT_APP} ${OUTPUT_DEVICE} >> ${OUTPUT_TERMINAL} DEPENDS target_upload_hex)
ENDIF()