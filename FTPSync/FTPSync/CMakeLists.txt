# CMakeList.txt : CMake project for FTPSync, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (FTPSync "FTPSync.cpp" "FTPSync.h")
target_sources (FTPSync PRIVATE

	CommandParser.cpp
	FileProcessor.cpp
	FileProcessorLocal.cpp
	FileProcessorRemote.cpp
	MessagePrinter.cpp

	CommandParser.h
	FileList.h
	FileProcessor.h
	FileProcessorLocal.h
	FileProcessorRemote.h
	MessagePrinter.h
)

# TODO: Add tests and install targets if needed.
