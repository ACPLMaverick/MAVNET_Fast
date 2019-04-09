cmake_minimum_required (VERSION 3.8)

target_sources (FTPSync PRIVATE

	FTPSync/CommandParser.cpp
	FTPSync/FileList.cpp
	FTPSync/FileProcessor.cpp
	FTPSync/FileProcessorLocal.cpp
	FTPSync/FileProcessorRemote.cpp
	FTPSync/FileUtil.cpp
	FTPSync/FTPSync.cpp
	FTPSync/MessagePrinter.cpp

	FTPSync/CommandParser.h
	FTPSync/Error.h
	FTPSync/FileList.h
	FTPSync/FileProcessor.h
	FTPSync/FileProcessorLocal.h
	FTPSync/FileProcessorRemote.h
	FTPSync/FileUtil.h
	FTPSync/FTPSync.h
	FTPSync/MessagePrinter.h
)