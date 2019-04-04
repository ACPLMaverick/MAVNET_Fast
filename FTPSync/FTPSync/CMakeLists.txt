# CMakeList.txt : CMake project for FTPSync, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (FTPSync "FTPSync.cpp" "FTPSync.h")
target_sources (FTPSync PRIVATE

	CommandParser.cpp
	FileList.cpp
	FileProcessor.cpp
	FileProcessorLocal.cpp
	FileProcessorRemote.cpp
	FileUtil.cpp
	MessagePrinter.cpp

	CommandParser.h
	Error.h
	FileList.h
	FileProcessor.h
	FileProcessorLocal.h
	FileProcessorRemote.h
	FileUtil.h
	MessagePrinter.h
)

# TODO: Add tests and install targets if needed.
