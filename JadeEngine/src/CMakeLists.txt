file(GLOB_RECURSE ALL_SOURCES
       ./*.cpp
)

target_sources(${EXE_NAME} PRIVATE ${ALL_SOURCES})